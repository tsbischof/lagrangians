#include <ostream>
#include <stdlib.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/regex.hpp>

#include "lagrangian.hpp"

using namespace lagrangians;
namespace pt = boost::property_tree;

Lagrangian::Lagrangian
(fs::path const& input_filename)
{
	if ( ! (fs::exists(input_filename) && !fs::is_directory(input_filename)) ) {
		throw std::runtime_error("Could not find input file.");
	}

	pt::ptree property_tree;
	pt::ini_parser::read_ini(input_filename.string(), property_tree);

	this->input_filename = input_filename;
	this->data_directory = this->input_filename.parent_path() / boost::regex_replace(input_filename.filename().string(), boost::regex("\\.inp"), "");

	// config section
	this->system = new EquationSystem(property_tree.get<std::string>("config.system"), property_tree.get<std::string>("config.endpoint"), property_tree.get<std::string>("config.validator", ""));

	this->run_type = property_tree.get<std::string>("config.run_type");
	this->height = property_tree.get<size_t>("config.height");
	this->width = property_tree.get<size_t>("config.width");

	// todo:
	// we should not need to allocate the full image, only the rows needed
	// at any given time. For small images this full-scale allocation is
	// fine.
	if ( width == 0 || height == 0 ) {
		throw(std::runtime_error("Image dimensions must be >0."));
	} else {
		image_type::extent_gen image_extents;
		
		// third dimension includes time, hence variables + 1
		this->image.resize(image_extents[this->width][this->system->n_parameters() + 1]);

		this->status.resize(this->height); 

		for ( size_t i = 0; i < this->status.size(); i++ ) {
			this->status[i] = 0; 
		} 
	}

	auto times = split(property_tree.get<std::string>("config.t"), ',');;
	this->t_start = atof(times[0].c_str());
	this->t_stop = atof(times[1].c_str());
	this->t_step = atof(times[2].c_str());

	// defaults, horizontal, and vertical can have all sorts of information
	get_section(this->default_parameters, property_tree.get_child("defaults"));
	get_section(this->horizontal_parameters, property_tree.get_child("horizontal"));
	get_section(this->vertical_parameters, property_tree.get_child("vertical"));

	this->build_phase_space();
	this->allocate_files();
}

// todo: 
// We want to be able to resume runs. Currently we halt if any files are
// already present, so build logic into the check for them
int Lagrangian::allocate_files
(void)
{
	// working directory
	if ( ! fs::exists(this->data_directory) ) {
		fs::create_directories(this->data_directory);
	} else if ( fs::is_directory(this->data_directory) ) {
		// this is fine: we will check to see what files exist in the directory already
	} else {
		throw std::runtime_error("Output directory name exists already but is not a directory. Remove it to continue.");
	}

	// trajectory
	if ( fs::exists(this->filename("trajectory")) and fs::exists(this->filename("status")) ) {
		this->status_file.open(this->filename("status").string(), std::ios::in | std::ios::out | std::ios::binary);
		this->trajectory_file.open(this->filename("trajectory").string(), std::ios::in | std::ios::out | std::ios::binary);

		for ( auto row = 0; row < this->height; row++ ) {
			this->status_file.read(&(this->status[row]), sizeof(this->status[row]));
		}
	} else { 
		// if a new run, allocate space
		double zero_lf = 0;
		this->trajectory_file.open(this->filename("trajectory").string(), std::ios::out | std::ios::binary);

		auto depth = this->system->variables.size() + 1;

		for ( auto row = 0; row < this->height; row++ ) {
			for ( auto column = 0; column < this->width; column++ ) {
				for ( auto var = 0; var < depth; var++ ) {
					this->trajectory_file.write(reinterpret_cast<char*>(&zero_lf), sizeof(zero_lf));
				}
			}
		}
		
	
		// status
		char zero = 0;
		this->status_file.open(this->filename("status").string(), std::ios::out | std::ios::binary);

		for ( auto i = 0; i < this->height; i++ ) {
			this->status_file.write(&zero, sizeof(zero));
		}
	}

	this->trajectory_file.seekp(std::ios_base::beg);
	this->status_file.seekp(std::ios_base::beg);
	
	return(0);
}

// Given the default, horizontal, and vertical parameters, generate the
// phase space object which allow us to pick initial conditions for any
// point in an image.
void Lagrangian::build_phase_space
(void)
{
	size_t n_parameters = this->system->n_parameters();
	std::vector<double> origin(n_parameters), basis_x(n_parameters), basis_y(n_parameters);

	this->system->get_origin_and_basis(origin, basis_x, basis_y, this->default_parameters, this->horizontal_parameters, this->vertical_parameters);

	this->phase_space = new PhaseSpace(origin, basis_x, basis_y);
}

fs::path Lagrangian::filename
(std::string name)
{
	return(this->data_directory / name);
}

void Lagrangian::run
(void)
{
	int i;
	double t;
	std::vector<double> variables(this->system->variables.size());
	std::vector<double> variables_init(this->system->variables.size());
	std::vector<double> constants(this->system->constants.size());
	std::vector<double> parameters(variables.size() + constants.size());
	size_t row, column;
	double row_frac, column_frac;

	// calculate origin and basis vectors for phase space
	for ( row = 0; row < this->height; row++ ) {
		// complete rows do not need to be worked on
		std::cout << this->input_filename << ": working on row " << row << " of " << this->height << std::endl;

		if ( this->status[row] ) {
			this->trajectory_file.seekp(sizeof(this->image[row][0])*(this->image[row].size()), std::ios_base::cur);
			this->status_file.seekp(sizeof(this->status[row]), std::ios_base::cur);
			continue;
		}

		for ( column = 0; column < this->width; column++ ) {
			row_frac = column/(this->height - 1.0);
			column_frac = row/(this->width - 1.0);

			this->phase_space->point(parameters, row_frac, column_frac);

			for ( i = 0; i < variables.size(); i++ ) {
				variables[i] = parameters[i];
				variables_init[i] = parameters[i];
			}

			for ( i = 0; i < constants.size(); i++ ) {
				constants[i] = parameters[variables.size() + i];
			}

			t = this->t_start;


			while ( (not this->system->endpoint(variables, variables_init)) and ( this->t_stop - t > this->t_step) ) {
				this->system->integrate(variables, constants, this->t_step);
			
				t += this->t_step;

			}

			// store result in image
			for ( i = 0; i < variables.size(); i++ ) {
				this->image[column][i] = variables[i];
			}

			this->image[column][i] = t;		
		}

		for ( column = 0; column < this->width; column++ ) {
			for ( i = 0; i <= variables.size(); i++ ) {
				this->trajectory_file.write(reinterpret_cast<char*>(&(this->image[column][i])), sizeof(this->image[column][i]));
			}
		}

		this->status[row] = 1;
		this->status_file.write(&(this->status[row]), sizeof(this->status[row]));
		this->trajectory_file.flush();
		this->status_file.flush();
	} 
}

std::string Lagrangian::status_string
(void)
{
	unsigned int finished = 0;

	for ( auto index: this->status ) {
		if ( this->status[index] ) {
			finished++;
		}
	}

	return(this->input_filename.string() + ": Finished " + std::to_string(finished) + " of " + std::to_string(this->height) + " rows");
}

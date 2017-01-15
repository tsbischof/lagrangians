#include <sys/stat.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/regex.hpp>

#include "lagrangian.hpp"

using namespace lagrangians;
namespace pt = boost::property_tree;

void 
get_section
(std::map<std::string, std::string> &parameters, pt::ptree const &tree)
{
	for ( auto const& node: tree ) {
		parameters[node.first] = tree.get<std::string>(node.first);
	}
}

Lagrangian::Lagrangian
(fs::path const& input_filename)
{
	if ( ! (fs::exists(input_filename) && !fs::is_directory(input_filename)) ) {
		throw std::runtime_error("Could not find input file.");
	}

	pt::ptree property_tree;
	pt::ini_parser::read_ini(input_filename.filename().string(), property_tree);

	this->input_filename = input_filename;

	// config section
	this->system = new EquationSystem(property_tree.get<std::string>("config.system"), property_tree.get<std::string>("config.endpoint"), property_tree.get<std::string>("config.validator"));

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
		
		// third dimension includes time, hence parameters + 1
		this->image.resize(image_extents[this->height][this->width][this->system->parameters.size() + 1]);

		status_type::extent_gen status_extents;
		this->status.resize(status_extents[this->height]); 

		// todo:
		// clean this up. there must be a nicer way to go about this
		for ( size_t i = 0; i < this->status.num_elements(); i++ ) {
			this->status.data()[i] = false; 
		} 
	}

	this->times = new Range(property_tree.get<std::string>("config.t"));

	// defaults, horizontal, and vertical can have all sorts of information
	get_section(this->default_parameters, property_tree.get_child("defaults"));
	get_section(this->horizontal_parameters, property_tree.get_child("horizontal"));
	get_section(this->vertical_parameters, property_tree.get_child("vertical"));

	this->build_phase_space();
}

// todo: 
// We want to be able to resume runs. Currently we halt if any files are
// already present, so build logic into the check for them
int Lagrangian::allocate_files
(void)
{
	// working directory
	if ( ! fs::exists(this->data_directory()) ) {
		fs::create_directories(this->data_directory());
	} else if ( fs::is_directory(this->data_directory()) ) {
		// this is fine: we will check to see what files exist in the directory already
	} else {
		throw std::runtime_error("Output directory name exists already but is not a directory. Remove it to continue.");
	}

	// trajectory
	
	// status
	
	return(0);
}

// Given the default, horizontal, and vertical parameters, generate the
// phase space object which allow us to pick initial conditions for any
// point in an image.
void Lagrangian::build_phase_space
(void)
{
	std::vector<double> origin, basis_x, basis_y;

	this->system->get_origin_and_basis(origin, basis_x, basis_y, this->default_parameters, this->horizontal_parameters, this->vertical_parameters);

	this->phase_space = new PhaseSpace(origin, basis_x, basis_y);
}

fs::path Lagrangian::data_directory
(void)
{
	fs::path data_directory = this->input_filename.root_directory();
	data_directory /= boost::regex_replace(input_filename.filename().string(), boost::regex("\\.inp"), "");

	return(data_directory);
}

bool Lagrangian::is_valid
(void)
{
	return(true);
}

void Lagrangian::run
(void)
{
	// calculate origin and basis vectors for phase space
/*	for ( size_t row = 0; row < this->height; row++ ) {
		// complete rows do not need to be worked on
		if ( this->status[row] ) {
			continue;
		}

		for ( size_t column = 0; column < this->width; column++ ) {
			double row_frac = column/(this->height - 1.0);
			double column_frac = row/(this->width - 1.0);

			// form the r0 values associated with this point in
			// phase space:
			// r = r0 + r_h*x + r_v*y
			// (origin plus basis in each direction)
			//r = 
			this->image[row][column] = 0;
		}
	} */
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

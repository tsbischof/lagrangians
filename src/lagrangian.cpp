#include <math.h>
#include <ostream>
#include <stdlib.h>
#include <sys/stat.h>

#include <boost/date_time/posix_time/posix_time.hpp>
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
	std::map<std::string, std::string> default_parameters;
	std::map<std::string, std::string> horizontal_parameters;
	std::map<std::string, std::string> vertical_parameters;

	if ( ! (fs::exists(input_filename) && !fs::is_directory(input_filename)) ) {
		throw std::runtime_error("Could not find input file.");
	}

	pt::ptree property_tree;
	pt::ini_parser::read_ini(input_filename.string(), property_tree);

	this->run_type = property_tree.get<std::string>("config.run_type");

	if ( not (this->run_type == "image" or this->run_type == "video") ) {
		throw(std::runtime_error("Unknown run type: " + this->run_type));
	}

	this->input_filename = input_filename;
	this->data_directory = this->input_filename.parent_path() / boost::regex_replace(input_filename.filename().string(), boost::regex("\\.inp"), ".run");

	// config section
	this->system = new EquationSystem(property_tree.get<std::string>("config.system"), property_tree.get<std::string>("config.endpoint", ""), property_tree.get<std::string>("config.validator", ""));

	this->height = property_tree.get<size_t>("config.height");
	this->width = property_tree.get<size_t>("config.width");

	auto times = split(property_tree.get<std::string>("config.t"), ',');;
	this->t_start = atof(times[0].c_str());
	this->t_stop = atof(times[1].c_str());
	this->t_step = atof(times[2].c_str());
	this->t_steps = floor((this->t_stop - this->t_start) / this->t_step);

	if ( this->t_steps == 0 ) {
		throw(std::runtime_error("Must have at least one time step (dt < (t_stop - t_start))"));
	}

	if ( width == 0 || height == 0 ) {
		throw(std::runtime_error("Image dimensions must be >0."));
	} else {
		if ( this->run_type == "image" ) {
			image_type::extent_gen image_extents;
		
			// third dimension includes time, hence variables + 1
			this->image.resize(image_extents[this->width][this->system->variables.size() + 1]);

			this->status.resize(this->height); 
		} else if ( this->run_type == "video" ) {
			video_type::extent_gen video_extents;

			this->video_variables.resize(video_extents[this->height][this->width][this->system->variables.size() + 1]);
			this->video_constants.resize(video_extents[this->height][this->width][this->system->constants.size()]);

			this->status.resize(this->t_steps);
		} 

		for ( size_t i = 0; i < this->status.size(); i++ ) {
			this->status[i] = 0; 
		} 
	}

	// defaults, horizontal, and vertical can have all sorts of information
	get_section(default_parameters, property_tree.get_child("defaults"));
	get_section(horizontal_parameters, property_tree.get_child("horizontal"));
	get_section(vertical_parameters, property_tree.get_child("vertical"));

	this->build_phase_space(default_parameters, horizontal_parameters, vertical_parameters);
}

int Lagrangian::allocate_image
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

int Lagrangian::allocate_video
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
	if ( fs::exists(this->filename("trajectory")) ) {
		for ( auto step = 0; step < this->t_steps; step++ ) {
			this->status[step] = 1;
		}
	} else { 
		// if a new run, allocate space
		double zero_lf = 0;
		this->trajectory_file.open(this->filename("trajectory").string(), std::ios::out | std::ios::binary);

		auto depth = this->video_variables[0][0].size();

		for ( auto step = 0; step <= this->t_steps; step++ ) {
			// one extra frame for t = 0 (<= instead of <)
			for ( auto row = 0; row < this->height; row++ ) {
				for ( auto column = 0; column < this->width; column++ ) {
					for ( auto var = 0; var < depth; var++ ) {
						this->trajectory_file.write(reinterpret_cast<char*>(&zero_lf), sizeof(zero_lf));
					}
				}
			}
		}
		
	
		// status
		char zero = 0;
		this->status_file.open(this->filename("status").string(), std::ios::out | std::ios::binary);

		for ( auto step = 0; step < this->t_steps; step++ ) {
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
(std::map<std::string, std::string>& default_parameters, std::map<std::string, std::string>& horizontal_parameters, std::map<std::string, std::string>& vertical_parameters)
{
	size_t n_parameters = this->system->n_parameters();
	std::vector<double> origin(n_parameters), basis_x(n_parameters), basis_y(n_parameters);

	this->system->get_origin_and_basis(origin, basis_x, basis_y, default_parameters, horizontal_parameters, vertical_parameters);

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
	if ( this->run_type == "image" ) {
		this->run_image();
	} else if ( this->run_type == "video" ) {
		this->run_video();
	} 
}

void Lagrangian::run_image
(void)
{
	size_t row_size = sizeof(this->image[0][0])*this->image[0].size()*this->width;
	size_t row, column;

	this->allocate_image();

	// calculate origin and basis vectors for phase space
	for ( row = 0; row < this->height; row++ ) {
		// complete rows do not need to be worked on
		if ( this->status[row] ) {
			this->trajectory_file.seekp(row_size, std::ios_base::cur);
			this->status_file.seekp(sizeof(this->status[row]), std::ios_base::cur);
			continue;
		}

		/* for a file which has been left open a long time, the file pointer seems
		 * to reset. To fight this we will explicitly check that we are at the correct
		 * location.
		 */
		if ( this->trajectory_file.tellp() != (row * row_size) ) {
			std::cerr << "improper row pointer at row " << row << ". Expected " << row*row_size << " but got " << this->trajectory_file.tellp() << std::endl;
			this->trajectory_file.seekp(row_size*row, std::ios_base::beg);
			this->status_file.seekp(sizeof(this->status[row])*row, std::ios_base::beg);
		}

		std::cerr << boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time()) << ": " << this->input_filename.string() << ": working on row " << row << " of " << this->height << std::endl;

		# pragma omp parallel for schedule(dynamic)
		for ( column = 0; column < this->width; column++ ) {
			double t;
			int i;
			int step;
			double row_frac, column_frac;
			std::vector<double> variables(this->system->variables.size(), 0);
			std::vector<double> variables_init(this->system->variables.size(), 0);
			std::vector<double> constants(this->system->constants.size(), 0);
			std::vector<double> parameters(this->system->n_parameters(), 0);
			row_frac = row/(this->height - 1.0);
			column_frac = column/(this->width - 1.0);	

			this->phase_space->point(parameters, column_frac, row_frac);

			for ( i = 0; i < variables.size(); i++ ) {
				variables[i] = parameters[i];
				variables_init[i] = parameters[i];
			}

			for ( i = 0; i < constants.size(); i++ ) {
				constants[i] = parameters[variables.size() + i];
			}

			t = 0;

			for ( int step = 1; (not this->system->endpoint(variables, variables_init)) and (step <= this->t_steps); step++ ) {
				t = this->t_start + step*this->t_step;
				this->system->integrate(variables, constants, this->t_step);
			}
			// store result in image
			for ( i = 0; i < variables.size(); i++ ) {
				this->image[column][i] = variables[i];
			}

			this->image[column][i] = t;		
		}

		for ( column = 0; column < this->width; column++ ) {
			for ( int depth = 0; depth < this->image[column].size(); depth++ ) {
				this->trajectory_file.write(reinterpret_cast<char*>(&(this->image[column][depth])), sizeof(this->image[column][depth]));
			}
		}

		this->trajectory_file.flush();
		this->status[row] = 1;
		this->status_file.write(&(this->status[row]), sizeof(this->status[row]));
		this->status_file.flush();
	} 
}

void write_frame
(std::fstream& stream_out, boost::multi_array<double, 3> const &frame)
{
	size_t row, column, depth;

	for ( row = 0; row < frame.size(); row++ ) {
		for ( column = 0; column < frame[row].size(); column++ ) {
			for ( depth = 0; depth < frame[row][column].size(); depth++ ) {
				stream_out.write(reinterpret_cast<char const*>(&(frame[row][column][depth])), sizeof(frame[row][column][depth]));
			}
		}
	}

 	stream_out.flush();
}

void Lagrangian::run_video
(void)
{
	size_t row, column, depth;
	double row_frac, column_frac;
	int step;

	this->allocate_video();

	// initialize the video
	# pragma omp parallel for schedule(dynamic) private(row, column)
	for ( row = 0; row < this->height; row++ ) {
		for ( column = 0; column < this->width; column++ ) {
			std::vector<double> parameters(this->system->n_parameters());
			row_frac = row/(this->height - 1.0);
			column_frac = column/(this->width - 1.0);

			this->phase_space->point(parameters, column_frac, row_frac);
			for ( auto i = 0; i < this->system->variables.size(); i++ ) {
				this->video_variables[row][column][i] = parameters[i];
			} 

			for ( auto i = 0; i < this->system->constants.size(); i++ ) {
				this->video_constants[row][column][i] = parameters[i + this->system->variables.size()];
			}

			this->video_variables[row][column][this->video_variables[row][column].size() - 1] = this->t_start;
		}
	}

	// write the initial conditions to file
	write_frame(this->trajectory_file, this->video_variables);

	for ( step = 1; step <= this->t_steps; step++ ) {
		if ( this->status[step] ) {
			// current we cannot restart (involves assumptions about reading from file, etc
			// so if the step is done, we just skip and move ahead
			continue;
		}

		std::cerr << boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time()) << ": " << this->input_filename.string() << ": working on time step " << step << " of " << this->t_steps << std::endl;

		# pragma omp parallel for schedule(dynamic) private(row, column)
		for ( row = 0; row < this->height; row++ ) {
			for ( column = 0; column < this->width; column++ ) {
				// todo: pass mutliarray by reference instead of doing this copy step
				std::vector<double> variables(this->video_variables[row][column].size());
				std::vector<double> constants(this->video_constants[row][column].size());

				for ( auto i = 0; i < variables.size(); i++ ) {
					variables[i] = this->video_variables[row][column][i];
				} 

				for ( auto i = 0; i < constants.size(); i++ ) {
					constants[i] = this->video_constants[row][column][i];
				}

				this->system->integrate(variables, constants, this->t_step);

				for ( auto i = 0; i < variables.size(); i++ ) {
					this->video_variables[row][column][i] = variables[i];
				}

				this->video_variables[row][column][this->video_variables[row][column].size() - 1] = this->t_start + step*this->t_step;
			}
		} 

		// write video to file
		write_frame(this->trajectory_file, this->video_variables);
	} 
}

std::string Lagrangian::status_string
(void)
{
	/* this currently only works for image status */
	std::string status_filename = this->filename("status").string();
	size_t finished = 0;

	if ( fs::exists(this->filename("trajectory").string()) and not fs::exists(status_filename) ) {
		finished = this->height;
	} else if ( not fs::exists(status_filename) ) {
		finished = 0;
	} else {
		std::ifstream status_file(this->filename("status").string(), std::ios::binary | std::ios::in );
		char val;

		for ( size_t i = 0; i < this->height; i++ ) {
			status_file.read(&val, sizeof(val));
			if ( val ) {
				finished++;
			}
		}

		status_file.close();
	}

	return(this->input_filename.string() + ": Finished " + std::to_string(finished) + " of " + std::to_string(this->height) + " rows");
}

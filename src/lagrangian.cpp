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
	this->system = property_tree.get<std::string>("config.system");

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
		// todo:
		// replace "9" with the actual number of parameters (or a dummy
		// value which covers all use cases?). Note this is the number
		// of values specific to the system, plus 1 (time)
		this->image.resize(image_extents[this->height][this->width][9]);

		status_type::extent_gen status_extents;
		this->status.resize(status_extents[this->height]); 

		// todo:
		// clean this up. there must be a nicer way to go about this
		for ( size_t i = 0; i < this->status.num_elements(); i++ ) {
			this->status.data()[i] = false; 
		} 
	}

	//this->validator = property_tree.get_optional<std::string>("config.validator").get_value_or("");
	this->times.from_string(property_tree.get<std::string>("config.t"));

	// defaults, horizontal, and vertical can have all sorts of information
	get_section(this->default_parameters, property_tree.get_child("defaults"));
	get_section(this->horizontal_parameters, property_tree.get_child("horizontal"));
	get_section(this->vertical_parameters, property_tree.get_child("vertical"));
}

// todo: 
// We want to be able to resume runs. Currently we halt if any files are
// already present, so build logic into the check for the 
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
	for ( auto row = 0; row < this->height; row++ ) {
		// complete rows do not need to be worked on
		if ( this->status[row] ) {
			continue;
		}

		for ( auto column = 0; column < this->width; column++ ) {
			double row_frac = column/(this->height - 1.0);
			double column_frac = row/(this->width - 1.0);

			// form the r0 values associated with this point in
			// phase space:
			// r = r0 + r_h*x + r_v*y
			// (origin plus basis in each direction)
			this->image[row][column] = 
		}
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

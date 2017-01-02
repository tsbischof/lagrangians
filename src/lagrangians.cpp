#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

#include "lagrangian.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

std::vector<fs::path> find_input_files
(fs::path root_directory) 
{
	std::vector<fs::path> input_files;
	static const boost::regex input_regex("\\.inp$");

	for ( auto& filename: fs::recursive_directory_iterator(root_directory) ) {
		if ( boost::regex_search(filename.path().string(), input_regex) ) {
			input_files.push_back(filename.path());
		}
	}

	return(input_files);
}

int main
(int argc, char *argv[])
{
	po::options_description description("lagrangians: tools for creating art from physics");
	po::positional_options_description positional_options;

	description.add_options()
		("help", "produce help message")
		("status", "display the status of the runs")
		("input-files", po::value< std::vector<fs::path> >(), "input files to process. If none are specified, the run directory will be searched for all available options.")
	;

	positional_options.add("input-files", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			options(description).
			positional(positional_options).run(), vm);
	po::notify(vm);

	if ( vm.count("help") ) { 
		std::cout << description << std::endl;
	       	return(1); 
	} 
	
	std::vector<fs::path> input_files;

	if ( vm.count("input-files") ) {
		input_files = vm["input-files"].as<std::vector<fs::path> >();
	} else {
		input_files = find_input_files(fs::current_path());
	}

	for ( auto const &input_filename: input_files ) {
		try {
			lagrangians::Lagrangian lagrangian(input_filename);
			if ( vm.count("status") ) {
				std::cout << lagrangian.status_string() << std::endl;
			} else {
				lagrangian.run();
			}
		} catch ( std::exception &ex ) {
			std::cout << "Error while processing " << input_filename.string() << ": " << ex.what() << std::endl;
		}
	}
	return(0);
}

#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main
(int argc, char *argv[])
{
	po::options_description description("Allowed options");
	po::positional_options_description positional_options;

	description.add_options()
		("help", "produce help message")
		("input-files", po::value< std::vector<std::string> >(), "input files to process. If none are specified, the run directory will be searched for all available options.")
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
	} else if ( vm.count("input-files") ) {
		for ( auto const& input_file: vm["input-files"].as<std::vector<std::string>  >() ) {
			std::cout << input_file << std::endl;
		}
		return(0);
	} else {
		// Find all available input files in the run directory?
		return(0);
	}
}

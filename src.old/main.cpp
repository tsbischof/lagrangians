#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

/* To install a new integrator, add its include here, and modify
 * integrators in main().
 */
#include "installed.hpp"
#include "double_pendulum.hpp"
#include "double_well.hpp"
#include "harmonic_oscillator.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	// To do: write the raw image every so often, as a backup. Allow restart from the dumped file.
	// To do: extend time limit by identifying whether the pixel is at the maximum value and running the appropriate starting conditions.
	// To do: read configuration from an input file.

	if (argc != 2) {
		cout << "Usage: lagrangians input_file" << endl;
		return(1);
	}

	string config_filename = argv[1];
//	string config_filename = "harm.inp";
	cout << "Reading input from " << config_filename << "." << endl;
	ifstream config_file(config_filename.c_str());

	if ( !config_file ) {
		cout << "Input file " << argv[1] << " could not be read." << endl;
		return(1);
	}

	po::options_description general;
	general.add_options()
			("integrator", "harmonic_oscillator, double_well, double_pendulum")
			;

	po::variables_map options;
	store(po::parse_config_file(config_file, general, true), options);
	config_file.close();

	void (*dispatcher)(string);
	bool success;
	dispatcher = get_dispatcher(options["integrator"].as<string>(), &success);
	if ( success ) {
		dispatcher(config_filename);
	} else {
		cout << "Integrator " << options["integrator"].as<string>()
						<< " is not installed." << endl;
		return(1);
	}

	return(0);
}

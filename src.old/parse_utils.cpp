#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>

#include "grapher.hpp"
#include "parse_utils.hpp"
#include "installed.hpp"

using namespace std;
namespace po = boost::program_options;

bool contains(vector<string> *list, string element) {
	unsigned int i;

	for (i = 0; i < list->size(); i++) {
		if ( (*list)[i] == element ) {
			return(true);
		}
	}
	return(false);
}

void parse_plot(string line, string *x, string *y) {
	boost::regex e("([^[:space:]]+),\\W*([^[:space:]]+)");
	boost::smatch what;
	if ( boost::regex_match(line, what, e) ) {
		*x = what[1].str();
		*y = what[2].str();
	}
}

void remove_whitespace(string *s) {
	s->erase(remove(s->begin(), s->end(), ' '), s->end());
}

double stod(string s) {
	char temp[s.size()];
	for (unsigned int i = 0; i < s.size(); i++) {
		temp[i] = s[i];
	}
	return(atof(temp));
}

bool parse_limits(string line, vector<double> *limits, string name) {
	boost::regex e("([0-9.-]+),\\W*([0-9.-]+),\\W*([0-9.-]+)");
	boost::smatch what;
	if ( boost::regex_match(line, what, e) ) {
		(*limits)[0] = stod(what[1].str());
		(*limits)[1] = stod(what[2].str());
		(*limits)[2] = stod(what[3].str());

		cout << "Found valid limits " << line
					<< " for " << name << "." << endl;

		return(true);
	} else {
		cout << "Fatal: Invalid limits " << line
						<< " for " << name << "." << endl;

		return(false);
	}
}

bool parse_config(string config_filename, po::options_description *options,
		vector<string> *valid_variables, vector<string> *valid_rules,
		string *rule,
		vector<double> *x_limits, vector<double> *y_limits,
		vector<double> *t_limits, string *x, string *y, string *comment) {

	options->add_options()
			("t", "")
			("plot", "")
			("rule", "")
			("comment", po::value<string>(comment)->default_value(""))
			;

	ifstream config_file(config_filename.c_str());
	po::variables_map config;
	store(po::parse_config_file(config_file, *options, true), config);
	config_file.close();

	*comment = config["comment"].as<string>();

	/* We need to verify that the options selected are actually reasonable
	 * for this simulation.
	 */
	bool good_config = true; // if any condition is unsatisfied, kill the run.

	// Check that the rule is allowed for this simulation.
	if ( contains(valid_rules, config["rule"].as<string>()) ) {
		*rule = config["rule"].as<string>();
		cout << "Found valid rule " << config["rule"].as<string>()
				<< "." << endl;
	} else {
		cout << "Fatal: Found invalid rule " << config["rule"].as<string>()
				<< "." << endl;
		good_config = false;
	}

	// Determine if x and y are valid variables.
	parse_plot(config["plot"].as<string>(), x, y);
	if ( contains(valid_variables, *x) and
	     contains(valid_variables, *y) ) {
		if (*x == *y) {
			cout << "Warning: axes are plotting the same variable "
					<< *x << "." << endl;
		}
		cout << "Found valid plot " << config["plot"].as<string>()
				<< "." << endl;
	} else {
		cout << "Fatal: Plot parameter " << config["plot"].as<string>()
				<< " is invalid." << endl;
		good_config = false;
	}

	// Do the limits for x, y, t.
	if ( not (parse_limits(config[*x].as<string>(), x_limits, *x) and
			  parse_limits(config[*y].as<string>(), y_limits, *y) and
			  parse_limits(config["t"].as<string>(), t_limits, "t") ) ) {
		good_config = false;
	}

	// That should be everything, so check for errors and move on.
	if ( not good_config ) {
		cout << "Terminating simulation due to errors in configuration file "
				<< config_filename << "." << endl;
	}

	return(good_config);
}

string name_from_config(string config_filename) {
	boost::regex e("(.*)\\..*$");
	boost::smatch what;
	if ( boost::regex_match((string)config_filename, what, e) ) {
		return(what[1].str());
	} else {
		return((string)config_filename);
	}
}

void setup_config(Grapher *mygrapher, string config_filename,
		vector<string> *variable_order,
		string integrator, string rule,
		string x, vector<double> *x_limits,
		string y, vector<double> *y_limits,
		vector<double> *t_limits, string comment) {

	cout << "Creating the grapher object and selecting the desired options." << endl;
	multimap<string, vector<double> > parms;
	parms.insert(make_pair(x, *x_limits));
	parms.insert(make_pair(y, *y_limits));
	parms.insert(make_pair("t", *t_limits));

	mygrapher->set_parms(parms);
	mygrapher->set_x(x);
	mygrapher->set_y(y);
	mygrapher->set_comment(comment);

	// Change these to the function calls.
	bool result;
	void (*integratorptr)(vector<double> *, double);
	integratorptr = get_integrator(integrator, &result);
	mygrapher->set_integrator( integratorptr );

	bool (*ruleptr)(vector<double> *, vector<double> *);
	ruleptr = get_rule(rule, &result);
	mygrapher->set_rule( ruleptr );

	mygrapher->set_parm_order(*variable_order);
	mygrapher->set_name(name_from_config(config_filename));
	cout << "Finished creating the grapher object and ready to run simulations." << endl;
}

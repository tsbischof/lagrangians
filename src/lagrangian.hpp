#ifndef LAGRANGIAN_HPP_
#define LAGRANGIAN_HPP_

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/multi_array.hpp>

#include "equation_system.hpp"
#include "phase_space.hpp"
#include "range.hpp"
#include "util.hpp"

namespace fs = boost::filesystem;

namespace lagrangians {
	typedef boost::multi_array<double, 2> image_type;

	class Lagrangian
	{
		fs::path input_filename;

		std::string run_type;
		image_type image;
		std::vector<char> status;
		size_t width;
		size_t height;

//		std::string validator;
//		Range* times;
		double t_start;
		double t_stop;
		double t_step;

		EquationSystem* system;
		PhaseSpace* phase_space;

		fs::path data_directory;

		std::fstream trajectory_file;
		std::fstream status_file;

		public:
			Lagrangian(fs::path const&);
			void build_phase_space(std::map<std::string, std::string>& default_parameters, std::map<std::string, std::string>& horizontal_parameters, std::map<std::string, std::string>& vertical_parameters);
			void run(void);
			std::string status_string(void);
		private:
			int allocate_files(void);	
			fs::path filename(std::string);
	};

	class LagrangianVideo : Lagrangian {
		public:
			int run(void);
	};

	class LagrangianImage : Lagrangian {
		public:
			int run(void);
	};
}; 

#endif

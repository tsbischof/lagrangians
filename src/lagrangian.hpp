#ifndef LAGRANGIAN_HPP_
#define LAGRANGIAN_HPP_

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
	typedef boost::multi_array<double, 3> image_type;
	typedef boost::multi_array<bool, 1> status_type;

	class Lagrangian
	{
		fs::path input_filename;

		std::string run_type;
		image_type image;
		status_type status;
		size_t width;
		size_t height;

//		std::string validator;
		Range* times;

		std::map<std::string, std::string> default_parameters;
		std::map<std::string, std::string> horizontal_parameters;
		std::map<std::string, std::string> vertical_parameters;

		EquationSystem* system;
		PhaseSpace* phase_space;

		fs::path trajectory_filename;
		fs::path status_filename;

		public:
			Lagrangian(fs::path const&);
			fs::path data_directory(void);

			void build_phase_space(void);

			void run(void);
			std::string status_string(void);
		private:
			int allocate_files(void);	
			bool is_valid(void);
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

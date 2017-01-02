#include <vector>

#include <boost/algorithm/string.hpp>

#include "range.hpp"

namespace lagrangians
{
	Range* Range::from_string
	(std::string const &value)
	{
		// We have two options:
		// 1. lower, upper
		// 2. lower, upper, step
		// If step is not given we just want to keep the user from 
		// doing anything but looking at the values 
		std::vector<std::string> values;
		boost::split(values, value, boost::is_any_of(","));

		if ( values.size() == 2 ) {
			this->start = std::stof(values[0]);
			this->stop = std::stof(values[1]);
			this->step = this->stop - this->start;
		} else if ( values.size() == 3 ) {
			this->start = std::stof(values[0]);
			this->stop = std::stof(values[1]);
			this->step = std::stof(values[2]);
		} else {
			throw(std::runtime_error("Expected 2 or 3 values in time definition (start, stop, [step])."));
		}

		return(this);
	}

} // namespace lagrangians

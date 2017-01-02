#ifndef RANGE_H_
#define RANGE_H_

#include <string>

namespace lagrangians
{
	class Range
	{
		double start;
		double stop;
		double step;

		public:
			Range* from_string(std::string const &);

//		public:
//			iterator	
	};
}

#endif

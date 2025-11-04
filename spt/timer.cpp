/// @file timer.cpp

#include "timer.hpp"

#include <chrono>

/// @brief Return the nanosecond timestamp from the start of the POSIX epoch
/// @return Number of nanoseconds from the start of the POSIX epoch
long long time_ns()
{
	return std::chrono::time_point_cast<std::chrono::nanoseconds>(
			   std::chrono::system_clock::now())
		.time_since_epoch()
		.count();
}

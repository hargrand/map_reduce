/**
 * @file perf.cpp
 * @brief Performance test for the native C++ Collection class.
 * @details This program runs a series of performance tests on the map/reduce
 *          operations of the native C++ `Collection` class. It takes command-line
 *          arguments to specify the output file and the sizes of the collections
 *          to test. The results are written to a CSV file.
 */

#include "../spt/natv_collection.hpp"
#include "../spt/perf_common.hpp"
#include "../spt/timer.hpp"

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

/**
 * @brief Main entry point for the performance test program.
 * @details Parses command-line arguments, runs performance tests for various
 *          collection sizes, and writes the results to a file. The tests
 *          involve creating a collection of random doubles and performing
 *          map/reduce operations.
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return 0 on successful execution.
 */
int main(int argc, char **argv)
{
	std::vector<std::string> args(argv, argv + argc);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	test_case tc = parse_args(args);
	std::vector<result<double>> results;
	for (auto size : tc.test_cases)
		results.push_back(
			run_test<Collection<double>, double>(
				size,
				[&](std::size_t)
				{ return dist(gen); }));
	write_results(tc, results);

	return 0;
}

/**
 * @file map_reduce.cpp
 */

#include "../spt/natv_collection.hpp"
#include "../spt/support.hpp"

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

/**
 *  @brief Main entry point of the program.
 *  @details Demonstrates the usage of the Collection class and its map/reduce-style operations.
 *  @return 0 on successful execution.
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

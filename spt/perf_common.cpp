/**
 * @file perf_common.cpp
 * @brief Implements common utility functions for the performance test executables.
 */

#include "perf_common.hpp"

#include <iostream>
#include <fstream>
#include <random>

/**
 * @brief Prints the command-line usage instructions for the performance test executables.
 * @param name The name of the executable, typically argv[0].
 */
void usage(const std::string &name)
{
	std::cout << "Usage: " << name << "outfile size0 size1 size2 ... sizeN" << std::endl;
	std::cout << "  outfile - CSV output file to write results to" << std::endl;
	std::cout << "  size<n> - Size of test sample to assess" << std::endl;
	std::cout << "Example: " << name << " results.csv 1000 10000 100000" << std::endl;
}

/**
 * @brief Parses command-line arguments into a test_case struct.
 * @param args A vector of strings representing the command-line arguments.
 * @return A populated test_case struct. Exits if arguments are invalid.
 */
test_case parse_args(const std::vector<std::string> &args)
{
	if (args.size() < 3)
	{
		usage(args[0]);
		exit(1);
	}
	test_case tc;
	tc.output_file = args[1];
	for (std::size_t i = 2; i < args.size(); ++i)
		tc.test_cases.push_back(std::stoi(args[i]));

	return tc;
}

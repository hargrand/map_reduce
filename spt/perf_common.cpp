/// @file support.cpp

#include "perf_common.hpp"

#include <iostream>
#include <fstream>
#include <random>

/// @brief Prints the command-line usage instructions to the console.
/// @param name The name of the executable.
void usage(const std::string &name)
{
	std::cout << "Usage: " << name << "outfile size0 size1 size2 ... sizeN" << std::endl;
	std::cout << "  outfile - CSV output file to write results to" << std::endl;
	std::cout << "  size<n> - Size of test sample to assess" << std::endl;
}

/// @brief Parses command-line arguments into a test_case struct.
/// @param args A vector of strings representing the command-line arguments.
/// @return A populated test_case struct.
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

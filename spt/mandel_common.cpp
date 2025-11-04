/// @file mandel_common.cpp

#include "mandel_common.hpp"

#include <iostream>

/**
 * @brief Prints the command-line usage instructions for the program.
 *
 * This function outputs a standard usage message to the console, explaining
 * how to run the program with the correct command-line arguments. It includes
 * an example to guide the user.
 *
 * @param name The name of the executable, typically argv[0].
 */
void usage(const std::string &name)
{
	std::cout << "Usage: " << name << "outfile size0 size1 size2 ... sizeN" << std::endl;
	std::cout << "  outfile - CSV output file to write results to" << std::endl;
	std::cout << "  size<n> - Size of test sample to assess" << std::endl;

	std::cout << "Example: " << name << " mandelbrot.png 1024 1024 4096 -2.0 -2.0 4.0" << std::endl;
}

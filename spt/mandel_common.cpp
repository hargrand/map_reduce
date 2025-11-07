/**
 * @file mandel_common.cpp
 * @brief Implements common utility functions for the Mandelbrot executables.
 */

#include "mandel_common.hpp"

#include <iostream>

/**
 * @brief Prints the command-line usage instructions for the Mandelbrot executables.
 * @details This function outputs a standard usage message to the console, explaining
 *          how to run the Mandelbrot programs with the correct command-line arguments.
 * @param name The name of the executable, typically argv[0].
 */
void usage(const std::string &name)
{
	std::cout << "Usage: " << name << " <outfile> <width> <height> <max_iters> <view_left> <view_bottom> <view_height>" << std::endl;
	std::cout << "  outfile     - PNG output file to write results to" << std::endl;
	std::cout << "  width       - Width of the output image in pixels" << std::endl;
	std::cout << "  height      - Height of the output image in pixels" << std::endl;
	std::cout << "  max_iters   - Maximum number of iterations for the Mandelbrot calculation" << std::endl;
	std::cout << "  view_left   - The leftmost coordinate of the view in the complex plane" << std::endl;
	std::cout << "  view_bottom - The bottommost coordinate of the view in the complex plane" << std::endl;
	std::cout << "  view_height - The height of the view in the complex plane" << std::endl;
	std::cout << "Example: " << name << " mandelbrot.png 1024 1024 4096 -2.0 -2.0 4.0" << std::endl;
}

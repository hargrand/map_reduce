/**
 * @file mandel_common.hpp
 * @brief Provides common utility functions for the Mandelbrot executables.
 */

#ifndef MANDEL_COMMON_HPP
#define MANDEL_COMMON_HPP

#include <string>

/**
 * @brief Prints the command-line usage instructions for the Mandelbrot executables.
 * @details This function outputs a standard usage message to the console, explaining
 *          how to run the Mandelbrot programs with the correct command-line arguments.
 * @param name The name of the executable, typically argv[0].
 */
extern void usage(const std::string &name);

#endif // MANDEL_COMMON_HPP

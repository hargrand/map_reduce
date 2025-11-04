#ifndef MANDEL_COMMON_HPP
#define MANDEL_COMMON_HPP

#include <string>

/**
 * @brief Prints the command-line usage instructions for the program.
 *
 * This function outputs a standard usage message to the console, explaining
 * how to run the program with the correct command-line arguments. It includes
 * an example to guide the user.
 *
 * @param name The name of the executable, typically argv[0].
 */
extern void usage(const std::string &name);

#endif // MANDEL_COMMON_HPP

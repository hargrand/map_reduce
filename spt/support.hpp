#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <fstream>

/// @brief Holds the performance results of a single test run.
template <typename T>
struct result
{
	/// @brief The size of the collections used in the test.
	std::size_t size;

	/// @brief Value computed in the test
	T value;

	/// @brief Time taken to generate the first collection (in nanoseconds).
	long long gen_time_1;

	/// @brief Time taken to generate the second collection (in nanoseconds).
	long long gen_time_2;

	/// @brief Time taken for the element-wise multiplication (zip) operation.
	long long zip_time;

	/// @brief Time taken for the sum (reduce) operation.
	long long reduce_time;
};

/// @brief Defines a set of tests to be run.
struct test_case
{
	/// @brief The path to the output CSV file.
	std::string output_file;

	/// @brief A vector of collection sizes to be tested.
	std::vector<std::size_t> test_cases;
};

/// @brief Return the nanosecond timestamp from the start of the POSIX epoch
/// @return Number of nanoseconds from the start of the POSIX epoch
extern long long time_ns();

/// @brief Parses command-line arguments into a test_case struct.
/// @param args A vector of strings representing the command-line arguments.
/// @return A populated test_case struct.
extern test_case parse_args(const std::vector<std::string> &args);

/// @brief Runs a single performance test for a given collection size.
/// @param size The number of elements for the collections in the test.
/// @param gen The random number generator to use for creating collection elements.
/// @return A result struct containing the performance metrics of the test.
template <typename COLL, typename T, typename FN>
result<T> run_test(std::size_t size, FN fn)
{
	result<T> res;
	res.size = size;

	long long start = time_ns();
	COLL u(size, fn);
	res.gen_time_1 = time_ns() - start;

	start = time_ns();
	COLL v(size, fn);
	res.gen_time_2 = time_ns() - start;

	start = time_ns();
	COLL w = u * v;
	res.zip_time = time_ns() - start;

	start = time_ns();
	res.value = sum(w);
	res.reduce_time = time_ns() - start;

	report(res);

	return res;
}

/// @brief Prints the results to the console.
/// @tparam T The type of the result.
/// @param res The result struct to be printed.
template <typename T>
extern void report(const result<T> &res)
{
	std::cout << "******************" << std::endl;
	std::cout << "size: " << res.size << std::endl;
	std::cout << "value: " << res.value << std::endl;
	std::cout << "gen_time_1 (ns): " << res.gen_time_1 << std::endl;
	std::cout << "gen_time_2 (ns): " << res.gen_time_2 << std::endl;
	std::cout << "zip_time (ns): " << res.zip_time << std::endl;
	std::cout << "reduce_time (ns): " << res.reduce_time << std::endl;
	std::cout << "******************" << std::endl;
}

/// @brief Writes the collected test results to a CSV file.
/// @tparam T The type of the result.
/// @param tc The test_case struct containing the output file name.
/// @param results A vector of result structs to be written to the file.
template <typename T>
void write_results(const test_case &tc, const std::vector<result<T>> &results)
{
	std::ofstream out(tc.output_file);
	if (!out.is_open())
	{
		std::cout << "Error opening file: " << tc.output_file << std::endl;
		exit(1);
	}

	out << "size,value,gen_time_1,gen_time_2,zip_time,reduce_time" << std::endl;
	for (auto res : results)
	{
		out << res.size << ","
			<< res.value << ","
			<< res.gen_time_1 * 1e-6 << ","
			<< res.gen_time_2 * 1e-6 << ","
			<< res.zip_time * 1e-6 << ","
			<< res.reduce_time * 1e-6 << std::endl;
	}
}

#endif // SUPPORT_HPP

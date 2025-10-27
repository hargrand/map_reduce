/**
 * @file map_reduce.cu
 */

#include "../spt/cuda_collection.cuh"
#include "../spt/support.hpp"

#include <curand_kernel.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

/**
 *  @brief Main entry point of the program.
 *  @details Demonstrates the usage of the Collection class and its map/reduce-style operations.
 *  @return 0 on successful execution.
 */
int main(int argc, char **argv)
{
	std::vector<std::string> args(argv, argv + argc);

	test_case tc = parse_args(args);
	std::vector<result<double>> results;

	curandState *d_rand_state;

	std::size_t max_size = tc.test_cases.empty()
							   ? 0
							   : *std::max_element(tc.test_cases.begin(), tc.test_cases.end());

	cudaMalloc(&d_rand_state, max_size * sizeof(curandState));

	for (auto size : tc.test_cases)
	{
		// Initialize cuRAND states
		auto init_rand = [=] __device__(unsigned int i)
		{
			curand_init(1234, i, 0, &d_rand_state[i]);
			return curand_uniform_double(&d_rand_state[i]);
		};
		results.push_back(
			run_test<Collection<double>, double>(
				size,
				init_rand));
	}
	cudaFree(d_rand_state);
	write_results(tc, results);

	return 0;
}

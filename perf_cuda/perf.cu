/**
 * @file perf.cu
 * @brief Performance test for the CUDA Collection class.
 * @details This program runs a series of performance tests on the map/reduce
 *          operations of the CUDA `Collection` class. It takes command-line
 *          arguments to specify the output file and the sizes of the collections
 *          to test. Test data is generated on the GPU using cuRAND. The results
 *          are written to a CSV file.
 */

#include "../spt/cuda_collection.cuh"
#include "../spt/perf_common.hpp"
#include "../spt/timer.hpp"

#include <curand_kernel.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

/**
 * @brief Main entry point for the CUDA performance test program.
 * @details Parses command-line arguments, initializes cuRAND states on the GPU,
 *          runs performance tests for various collection sizes, and writes the
 *          results to a file. The tests involve creating a collection of random
 *          doubles directly on the device and performing map/reduce operations.
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return 0 on successful execution.
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

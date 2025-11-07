/**
 * @file main.cu
 * @brief Main entry point for the CUDA Mandelbrot set generator.
 * @details This program generates a PNG image of the Mandelbrot set using CUDA
 *          for acceleration. It takes command-line arguments specifying the
 *          output file, image dimensions, iteration limit, and viewing window.
 */

#include "mandel.cuh"
#include "../spt/image.hpp"
#include "../spt/assert.hpp"
#include "../spt/mandel_common.hpp"
#include "../spt/timer.hpp"
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Main function to generate the Mandelbrot set image using CUDA.
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return 0 on successful image creation, 1 on argument error, or a non-zero error code on file write failure.
 */
int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv + argc);

    if (argc != 8)
    {
        usage(argv[0]);
        return 1;
    }

    std::string filename = args[1];
    unsigned int image_width = std::stoul(args[2]);
    unsigned int image_height = std::stoul(args[3]);
    unsigned int max_iters = std::stoul(args[4]);
    double view_left = std::stod(args[5]);
    double view_bottom = std::stod(args[6]);
    double view_height = std::stod(args[7]);

    mandelbrot mandel(view_left, view_bottom, view_height);
    PNGImage png(image_width, image_height, 8);

    try
    {
        auto start = time_ns();
        std::vector<Color> colors = mandel.create_image(image_width, image_height, max_iters);
        png.write(filename, colors);
        auto duration = (time_ns() - start) * 1e-9;
        std::cout << "Successfully created PNG file: " << filename
                  << " in " << duration << " seconds" << std::endl;
    }
    catch (const assertion_error &e)
    {
        std::cerr << e.msg() << std::endl;
    }

    return png.error_code();
}
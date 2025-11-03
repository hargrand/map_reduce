#include "mandel.hpp"
#include "../spt/image.hpp"
#include "../spt/natv_collection.hpp"

#include <iostream>
#include <string>
#include <vector>

void usage(const std::string &name)
{
    std::cout << "Usage: " << name << "outfile size0 size1 size2 ... sizeN" << std::endl;
    std::cout << "  outfile - CSV output file to write results to" << std::endl;
    std::cout << "  size<n> - Size of test sample to assess" << std::endl;

    std::cout << "Example: " << name << " mandelbrot.png 1024 1024 4096 -2.0 -2.0 4.0" << std::endl;
}

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

    std::vector<Color> colors = mandel.create_image(image_width, image_height, max_iters);
    try
    {
        png.write(filename, colors);
        std::cout << "Successfully created PNG file: " << filename << std::endl;
    }
    catch (const assertion_error &e)
    {
        std::cerr << e.msg() << std::endl;
    }

    return png.error_code();
}
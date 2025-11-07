#include "mandel.cuh"
#include "../spt/cuda_collection.cuh"

/**
 * @brief Calculates the Mandelbrot iteration count for a given point on the GPU.
 * @details This `__device__` function determines if a point (x0, y0) is in the
 *          Mandelbrot set by iterating the equation z_{n+1} = z_n^2 + c. It
 *          returns the number of iterations it takes for the magnitude of z to
 *          exceed 2, up to a maximum limit. This function is executed on the
 *          CUDA device.
 * @param x0 The real part of the complex number c.
 * @param y0 The imaginary part of the complex number c.
 * @param max_iters The maximum number of iterations to perform.
 * @return The number of iterations before escaping, or max_iters if it does not.
 */
std::size_t __device__ get_count(
    double x0,
    double y0,
    std::size_t max_iters)
{
    double x = 0.0;
    double y = 0.0;
    std::size_t i = 0;
    double x_temp;

    while (i < max_iters && (x * x + y * y) < 4.0)
    {
        x_temp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = x_temp;
        ++i;
    }
    return i;
}

/**
 * @brief Constructs a mandelbrot object.
 */
mandelbrot::mandelbrot(
    double view_left,
    double view_bottom,
    double view_height)
    : _view_left(view_left),
      _view_bottom(view_bottom),
      _view_height(view_height)
{
}

/**
 * @brief Creates the image data for the Mandelbrot set using CUDA.
 */
std::vector<Color> mandelbrot::create_image(
    std::size_t width,
    std::size_t height,
    std::size_t max_iters) const
{
    double scale = view_height() / height;
    double top = view_top();
    double left = view_left();

    auto mandel_fn = [=] __device__(std::size_t idx) -> std::size_t
    {
        std::size_t row = idx / width;
        std::size_t col = idx % width;
        double y = top - (row * scale);
        double x = left + (col * scale);

        return get_count(x, y, max_iters);
    };

    auto color_fn = [=] __device__(std::size_t val) -> Color
    {
        if (val == max_iters)
            return Color{0, 0, 0};

        unsigned char red = ((val >> 16) & 0xff);
        unsigned char green = ((val >> 8) & 0xff);
        unsigned char blue = (val & 0xff);

        return Color{red, green, blue};
    };

    Collection<std::size_t> data(width * height, mandel_fn);
    Collection<Color> colors(data.map<Color>(color_fn));

    return colors.to_vector();
}

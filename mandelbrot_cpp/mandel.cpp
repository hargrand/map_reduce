#include "mandel.hpp"
#include "../spt/natv_collection.hpp"

mandelbrot::mandelbrot(
    double view_left,
    double view_bottom,
    double view_height)
    : _view_left(view_left),
      _view_bottom(view_bottom),
      _view_height(view_height)
{
}

unsigned int mandelbrot::get_count(
    double x0,
    double y0,
    unsigned int max_iters)
{
    double x = 0.0;
    double y = 0.0;
    unsigned int i = 0;
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

std::vector<Color> mandelbrot::create_image(
    unsigned int width,
    unsigned int height,
    unsigned int max_iters) const
{
    double scale = view_height() / height;
    double top = view_top();
    double left = view_left();

    auto mandel_fn = [&](std::size_t idx) -> unsigned int
    {
        unsigned int row = idx / width;
        unsigned int col = idx % width;
        double y = top - (row * scale);
        double x = left + (col * scale);

        return get_count(x, y, max_iters);
    };

    auto color_fn = [&](unsigned int val) -> Color
    {
        if (val == max_iters)
            return Color();

        unsigned char red = ((val >> 16) & 0xff);
        unsigned char green = ((val >> 8) & 0xff);
        unsigned char blue = (val & 0xff);

        return Color(red, green, blue);
    };

    Collection<unsigned int> data(width * height, mandel_fn);
    Collection<Color> colors(data.map<Color>(color_fn));

    return colors.to_vector();
}

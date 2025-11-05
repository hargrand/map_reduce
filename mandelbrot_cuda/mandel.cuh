#ifndef MANDEL_H
#define MANDEL_H

#include "../spt/image.hpp"
#include <vector>

class mandelbrot
{
private:
    const double _view_left;
    const double _view_bottom;
    const double _view_height;

public:
    mandelbrot(
        double view_left,
        double view_bottom,
        double view_height);

    std::vector<Color> create_image(
        std::size_t width,
        std::size_t height,
        std::size_t max_iters) const;

    inline double view_left() const { return _view_left; }
    inline double view_bottom() const { return _view_bottom; }
    inline double view_top() const { return _view_bottom + _view_height; }
    inline double view_height() const { return _view_height; }
};

#endif
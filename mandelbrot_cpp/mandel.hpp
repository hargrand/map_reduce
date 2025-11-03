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

    static unsigned int get_count(double x0, double y0, unsigned int max_iters);

public:
    mandelbrot(
        double view_left,
        double view_bottom,
        double view_height);

    std::vector<Color> create_image(
        unsigned int width,
        unsigned int height,
        unsigned int max_iters) const;

    inline double view_left() const { return _view_left; }
    inline double view_bottom() const { return _view_bottom; }
    inline double view_top() const { return _view_bottom + _view_height; }
    inline double view_height() const { return _view_height; }
};

// #include "../spt/image.hpp"

// struct mandel_config
// {
//     double left;
//     double bottom;
//     double height;
//     unsigned int max_iters;
// };

// extern void create_image(const mandel_config &mdl,
//                          const image_config &img);

#endif
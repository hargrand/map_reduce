/**
 * @file mandel.hpp
 * @brief Defines the mandelbrot class for generating Mandelbrot set data.
 */

#ifndef MANDEL_HPP
#define MANDEL_HPP

#include "../spt/image.hpp"
#include <vector>

/**
 * @class mandelbrot
 * @brief Represents the Mandelbrot set calculation logic.
 * @details This class encapsulates the parameters of the view into the
 *          Mandelbrot set and provides a method to generate the image data.
 */
class mandelbrot
{
private:
    /** @brief The leftmost coordinate of the view. */
    const double _view_left;

    /** @brief The bottommost coordinate of the view. */
    const double _view_bottom;

    /** @brief The height of the view. */
    const double _view_height;

public:
    /**
     * @brief Constructs a mandelbrot object with a specified view.
     * @param view_left The leftmost coordinate of the view.
     * @param view_bottom The bottommost coordinate of the view.
     * @param view_height The height of the view.
     */
    mandelbrot(
        double view_left,
        double view_bottom,
        double view_height);

    /**
     * @brief Generates the color data for the Mandelbrot set image.
     * @param width The width of the image in pixels.
     * @param height The height of the image in pixels.
     * @param max_iters The maximum number of iterations for the calculation.
     * @return A vector of Color objects representing the pixels of the image.
     */
    std::vector<Color> create_image(
        std::size_t width,
        std::size_t height,
        std::size_t max_iters) const;

    /**
     * @brief Gets the leftmost coordinate of the view.
     * @return The leftmost coordinate.
     */
    inline double view_left() const { return _view_left; }

    /**
     * @brief Gets the bottommost coordinate of the view.
     * @return The bottommost coordinate.
     */
    inline double view_bottom() const { return _view_bottom; }

    /**
     * @brief Calculates and gets the topmost coordinate of the view.
     * @return The topmost coordinate.
     */

    inline double view_top() const { return _view_bottom + _view_height; }
    /**
     * @brief Gets the height of the view.
     * @return The view height.
     */
    inline double view_height() const { return _view_height; }
};

#endif // MANDEL_HPP
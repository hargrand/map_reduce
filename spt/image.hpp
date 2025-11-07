/**
 * @file image.hpp
 * @brief Defines the PNGImage class for creating and writing PNG files.
 * @details This header provides the `Color` struct and the `PNGImage` class,
 *          which encapsulates the logic for writing a buffer of pixel data
 *          to a PNG file using the libpng library.
 */

#include <png.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

#ifndef IMAGE_HPP
#define IMAGE_HPP

/**
 * @struct Color
 * @brief Represents an RGB color with 8-bit channels.
 */
struct Color
{
    /** @brief The red component of the color (0-255). */
    unsigned char red;

    /** @brief The green component of the color (0-255). */
    unsigned char green;

    /** @brief The blue component of the color (0-255). */
    unsigned char blue;
};

/**
 * @class PNGImage
 * @brief A class for creating and writing PNG image files.
 * @details This class wraps the functionality of the libpng library to provide a
 *          simple interface for writing a vector of `Color` data to a PNG file.
 *          It handles memory management and error reporting.
 */
class PNGImage
{
private:
    /** @brief The width of the image in pixels. */
    const unsigned int _width;

    /** @brief The height of the image in pixels. */
    const unsigned int _height;

    /** @brief The bit depth of each color channel (e.g., 8). */
    const unsigned int _bit_depth;

    /** @brief Stores the last error code encountered. */
    int _error_code;

    /** @brief Pointer to the libpng write structure. */
    png_structp _png_ptr;

    /** @brief Pointer to the libpng info structure. */
    png_infop _info_ptr;

    /** @brief Array of pointers to the image rows. */
    png_bytep *_rows_ptr;

    /** @brief File pointer for the output PNG file. */
    FILE *_file_ptr;

    /** @brief A static map of error codes to descriptive messages. */
    static const std::unordered_map<int, std::string> error_messages;

    /** @brief Initializes the internal libpng structures. */
    void init_image();

    /** @brief Allocates memory for the row pointers and image rows. */
    void make_rows();

    /** @brief Opens the output PNG file for writing. */
    void open_png_output_file(const std::string &filename);

    /** @brief Populates the image rows from a vector of Color data. */
    void build_image(const std::vector<Color> &colors);

    /** @brief Frees all allocated resources and closes the file. */
    void cleanup(int error_code);

public:
    /**
     * @brief Constructs a PNGImage object with specified dimensions.
     * @param image_width The width of the image in pixels.
     * @param image_height The height of the image in pixels.
     * @param image_bit_depth The bit depth of each color channel (typically 8).
     */
    PNGImage(unsigned int image_width,
             unsigned int image_height,
             int image_bit_depth);

    /**
     * @brief Destructor for the PNGImage class.
     * @details Ensures that all resources are cleaned up properly.
     */
    virtual ~PNGImage();

    /**
     * @brief Custom error handler for libpng that throws an exception.
     * @param png_ptr Pointer to the png_struct.
     * @param msg The error message from libpng.
     */
    static void error_handler(png_structp png_ptr, png_const_charp msg);

    /**
     * @brief Gets the last error code.
     * @return The integer error code. 0 means no error.
     */
    inline int error_code() const { return _error_code; }

    /**
     * @brief Gets the message for the current error code.
     * @return A string describing the last error.
     */
    inline std::string error_message() const { return error_messages.at(error_code()); }

    /**
     * @brief Gets the width of the image.
     * @return The image width in pixels.
     */
    inline unsigned int width() const { return _width; }

    /**
     * @brief Gets the height of the image.
     * @return The image height in pixels.
     */
    inline unsigned int height() const { return _height; }

    /**
     * @brief Gets the bit depth of the image.
     * @return The bit depth per channel.
     */
    inline unsigned int bit_depth() const { return _bit_depth; }

    /**
     * @brief Writes the provided color data to a PNG file.
     * @param filename The path to the output PNG file.
     * @param colors A vector of `Color` structs representing the image pixels.
     */
    void write(const std::string &filename, const std::vector<Color> &colors);
};

#endif

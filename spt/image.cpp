/**
 * @file image.cpp
 * @brief Implements the PNGImage class for creating and writing PNG files.
 * @details This file contains the implementation of the PNGImage class, which uses
 *          the libpng library to write image data to a PNG file.
 */

#include "image.hpp"
#include "assert.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <stddef.h>
#include <iostream>
#include <string>
#include <algorithm>

/// @brief Error code for no error.
#define NO_ERROR 0
/// @brief Error message for no error.
#define NO_ERROR_MSG "No error"

/// @brief Error code for failure to allocate libpng write struct.
#define PNG_ALLOC_WRITE_STRUCT_FAIL_ERR -1
/// @brief Error message for failure to allocate libpng write struct.
#define PNG_ALLOC_WRITE_STRUCT_FAIL_MSG "Could not allocate PNG write struct"

/// @brief Error code for failure to allocate libpng info struct.
#define PNG_ALLOC_INFO_STRUCT_FAIL_ERR -2
/// @brief Error message for failure to allocate libpng info struct.
#define PNG_ALLOC_INFO_STRUCT_FAIL_MSG "Could not allocate PNG info struct"

/// @brief Error code for a general libpng error during creation.
#define PNG_ERROR_HANDLING_FAIL_ERR -3
/// @brief Error message for a general libpng error during creation.
#define PNG_ERROR_HANDLING_FAIL_MSG "An error occurred during PNG creation"

/// @brief Error code for failure to open the output file.
#define PNG_FILE_OPEN_FAIL_ERR -4
/// @brief Error message for failure to open the output file.
#define PNG_FILE_OPEN_FAIL_MSG "Could not open file for writing"

/// @brief Error code for failure to allocate memory for row pointers.
#define PNG_MAKE_ROWS_FAIL_ERR -5
/// @brief Error message for failure to allocate memory for row pointers.
#define PNG_MAKE_ROWS_FAIL_MSG "Could not allocate memory for row pointers"

/// @brief Error code for failure to allocate memory for a single row.
#define PNG_MAKE_ROW_FAIL_ERR -6
/// @brief Error message for failure to allocate memory for a single row.
#define PNG_MAKE_ROW_FAIL_MSG "Could not allocate memory for row"

/// @brief Error code for failure to allocate memory for a data row.
#define DATA_ROW_ALLOCATION_FAIL_ERR -7
/// @brief Error message for failure to allocate memory for a data row.
#define DATA_ROW_ALLOCATION_FAIL_MSG "Could not allocate memory for row in image data"

/**
 * @class PNGException
 * @brief Custom exception for libpng errors.
 * @details This exception is thrown by the custom error handler to allow for
 *          C++-style exception handling instead of using `longjmp`.
 */
class PNGException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

/// @brief A map of error codes to their string representations.
const std::unordered_map<int, std::string> PNGImage::error_messages = {
    {NO_ERROR, NO_ERROR_MSG},
    {PNG_ALLOC_WRITE_STRUCT_FAIL_ERR, PNG_ALLOC_WRITE_STRUCT_FAIL_MSG},
    {PNG_ALLOC_INFO_STRUCT_FAIL_ERR, PNG_ALLOC_INFO_STRUCT_FAIL_MSG},
    {PNG_ERROR_HANDLING_FAIL_ERR, PNG_ERROR_HANDLING_FAIL_MSG},
    {PNG_FILE_OPEN_FAIL_ERR, PNG_FILE_OPEN_FAIL_MSG},
    {PNG_MAKE_ROWS_FAIL_ERR, PNG_MAKE_ROWS_FAIL_MSG},
    {PNG_MAKE_ROW_FAIL_ERR, PNG_MAKE_ROW_FAIL_MSG},
    {DATA_ROW_ALLOCATION_FAIL_ERR, DATA_ROW_ALLOCATION_FAIL_MSG}};

/**
 * @brief Constructs a PNGImage object.
 */
PNGImage::PNGImage(unsigned int image_width,
                   unsigned int image_height,
                   int image_bit_depth)
    : _width(image_width),
      _height(image_height),
      _bit_depth(image_bit_depth),
      _error_code(NO_ERROR),
      _png_ptr(nullptr),
      _info_ptr(nullptr),
      _rows_ptr(nullptr),
      _file_ptr(nullptr)
{
}

/**
 * @brief Destroys the PNGImage object and cleans up resources.
 */
PNGImage::~PNGImage()
{
    cleanup(_error_code);
}

/**
 * @brief Frees all allocated resources and sets the final error code.
 */
void PNGImage::cleanup(int error_code)
{
    _error_code = error_code;

    // Close the file
    if (_file_ptr)
    {
        fclose(_file_ptr);
        _file_ptr = nullptr;
    }

    // --- Cleanup ---
    if (_rows_ptr)
    {
        for (unsigned int y = 0; y < _height; y++)
            if (_rows_ptr[y])
                delete[] _rows_ptr[y];
        delete[] _rows_ptr;
        _rows_ptr = nullptr;
    }

    png_destroy_write_struct(&_png_ptr, &_info_ptr);
    _png_ptr = nullptr;
    _info_ptr = nullptr;
}

/**
 * @brief Custom libpng error handler that throws a PNGException.
 */
void PNGImage::error_handler(png_structp, png_const_charp msg)
{
    // Throw an exception instead of using longjmp.
    throw PNGException(msg);
}

/**
 * @brief Initializes the libpng write and info structures.
 */
void PNGImage::init_image()
{
    // Initialize the PNG write structures
    // Provide custom error handler to use exceptions instead of setjmp/longjmp
    _png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, &PNGImage::error_handler, nullptr);
    if (!_png_ptr)
    {
        cleanup(PNG_ALLOC_WRITE_STRUCT_FAIL_ERR);
        return;
    }

    _info_ptr = png_create_info_struct(_png_ptr);
    if (!_info_ptr)
    {
        cleanup(PNG_ALLOC_INFO_STRUCT_FAIL_ERR);
        return;
    }
}

/**
 * @brief Opens the output file and initializes libpng for writing.
 */
void PNGImage::open_png_output_file(const std::string &filename)
{
    // Open the file for writing in binary mode
#if defined(_MSC_VER)
    fopen_s(&_file_ptr, filename.c_str(), "wb");
#else
    _file_ptr = fopen(filename.c_str(), "wb");
#endif
    if (!_file_ptr)
    {
        cleanup(PNG_FILE_OPEN_FAIL_ERR);
        return;
    }

    // Initialize the PNG boilerplate
    this->init_image();
    if (this->error_code())
        return;

    // Set up the output control
    png_init_io(_png_ptr, _file_ptr);
}

/**
 * @brief Sets up PNG headers and allocates memory for image rows.
 */
void PNGImage::make_rows()
{
    try
    {
        // Set the PNG header information
        png_set_IHDR(_png_ptr, _info_ptr, _width,
                     _height, _bit_depth,
                     PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        // Write the header to the file
        png_write_info(_png_ptr, _info_ptr);

        // Allocate memory for the row pointers
        _rows_ptr = new png_bytep[_height];
        if (!_rows_ptr)
        {
            cleanup(PNG_MAKE_ROWS_FAIL_ERR);
            return;
        }

        // Initialize row pointers to nullptr for safer cleanup on allocation failure.
        std::fill(_rows_ptr, _rows_ptr + _height, nullptr);

        // Allocate memory for each row
        for (unsigned int y = 0; y < _height; y++)
        {
            _rows_ptr[y] = new png_byte[png_get_rowbytes(_png_ptr, _info_ptr)];
            if (!_rows_ptr[y])
            {
                cleanup(PNG_MAKE_ROW_FAIL_ERR);
                return;
            }
        }
    }
    catch (const PNGException &e)
    {
        std::cerr << "libpng error: " << e.what() << std::endl;
        cleanup(PNG_ERROR_HANDLING_FAIL_ERR);
    }
}

/**
 * @brief Converts the vector of Color data into the row format required by libpng.
 */
void PNGImage::build_image(const std::vector<Color> &colors)
{
    std::size_t expected_size = _width * _height;

    assert_equal(colors.size(), expected_size, "Image size mismatch");

    make_rows();
    assert_equal(error_code(), NO_ERROR, error_message());

    std::vector<Color>::const_iterator color = colors.cbegin();

    for (unsigned int row = 0; row < _height; ++row)
    {
        png_bytep image_row = _rows_ptr[row];
        for (unsigned int col = 0; col < _width; ++col, ++color)
        {
            png_bytep px = &(image_row[col * 3]);
            px[0] = color->red;
            px[1] = color->green;
            px[2] = color->blue;
        }
    }
}

/**
 * @brief Main function to write the image data to a specified PNG file.
 */
void PNGImage::write(const std::string &filename, const std::vector<Color> &colors)
{
    // Open the file for writing in binary mode
    open_png_output_file(filename);
    assert_equal(error_code(), NO_ERROR, error_message());

    build_image(colors);
    assert_equal(error_code(), NO_ERROR, error_message());

    try
    {
        // Write the image data to the file
        png_write_image(_png_ptr, _rows_ptr);

        // End the write process
        png_write_end(_png_ptr, NULL);
    }
    catch (const PNGException &e)
    {
        std::cerr << "libpng error during write: " << e.what() << std::endl;
        cleanup(PNG_ERROR_HANDLING_FAIL_ERR);
    }
    cleanup(NO_ERROR);
}

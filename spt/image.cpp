#include "image.hpp"
#include "assert.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <iostream>
#include <string>
#include <algorithm>

#define NO_ERROR 0
#define NO_ERROR_MSG "No error"

#define PNG_ALLOC_WRITE_STRUCT_FAIL_ERR -1
#define PNG_ALLOC_WRITE_STRUCT_FAIL_MSG "Could not allocate PNG write struct"

#define PNG_ALLOC_INFO_STRUCT_FAIL_ERR -2
#define PNG_ALLOC_INFO_STRUCT_FAIL_MSG "Could not allocate PNG info struct"

#define PNG_ERROR_HANDLING_FAIL_ERR -3
#define PNG_ERROR_HANDLING_FAIL_MSG "An error occurred during PNG creation"

#define PNG_FILE_OPEN_FAIL_ERR -4
#define PNG_FILE_OPEN_FAIL_MSG "Could not open file for writing"

#define PNG_MAKE_ROWS_FAIL_ERR -5
#define PNG_MAKE_ROWS_FAIL_MSG "Could not allocate memory for row pointers"

#define PNG_MAKE_ROW_FAIL_ERR -6
#define PNG_MAKE_ROW_FAIL_MSG "Could not allocate memory for row"

#define DATA_ROW_ALLOCATION_FAIL_ERR -7
#define DATA_ROW_ALLOCATION_FAIL_MSG "Could not allocate memory for row in image data"

const std::unordered_map<int, std::string> PNGImage::error_messages = {
    {NO_ERROR, NO_ERROR_MSG},
    {PNG_ALLOC_WRITE_STRUCT_FAIL_ERR, PNG_ALLOC_WRITE_STRUCT_FAIL_MSG},
    {PNG_ALLOC_INFO_STRUCT_FAIL_ERR, PNG_ALLOC_INFO_STRUCT_FAIL_MSG},
    {PNG_ERROR_HANDLING_FAIL_ERR, PNG_ERROR_HANDLING_FAIL_MSG},
    {PNG_FILE_OPEN_FAIL_ERR, PNG_FILE_OPEN_FAIL_MSG},
    {PNG_MAKE_ROWS_FAIL_ERR, PNG_MAKE_ROWS_FAIL_MSG},
    {PNG_MAKE_ROW_FAIL_ERR, PNG_MAKE_ROW_FAIL_MSG},
    {DATA_ROW_ALLOCATION_FAIL_ERR, DATA_ROW_ALLOCATION_FAIL_MSG}};

Color::Color(unsigned char r, unsigned char g, unsigned char b)
    : red(r), green(g), blue(b)
{
}

Color::Color() : Color(0, 0, 0)
{
}

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

PNGImage::~PNGImage()
{
    cleanup(_error_code);
}

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

void PNGImage::init_image()
{
    // Initialize the PNG write structures
    _png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

    // Set up error handling
    if (setjmp(png_jmpbuf(_png_ptr)))
    {
        cleanup(PNG_ERROR_HANDLING_FAIL_ERR);
        return;
    }
}

void PNGImage::open_png_output_file(const std::string &filename)
{
    // Open the file for writing in binary mode
    _file_ptr = fopen(filename.c_str(), "wb");
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

void PNGImage::make_rows()
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
            px[0] = color->get_red();
            px[1] = color->get_green();
            px[2] = color->get_blue();
        }
    }
}

// Function to write image data to a PNG file
void PNGImage::write(const std::string &filename, const std::vector<Color> &colors)
{
    // Open the file for writing in binary mode
    open_png_output_file(filename);
    assert_equal(error_code(), NO_ERROR, error_message());

    build_image(colors);
    assert_equal(error_code(), NO_ERROR, error_message());

    // Set up the output control
    // Write the image data to the file
    png_write_image(_png_ptr, _rows_ptr);

    // End the write process
    png_write_end(_png_ptr, NULL);
    cleanup(NO_ERROR);
}

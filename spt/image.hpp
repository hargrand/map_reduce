#include <png.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

#ifndef IMAGE_HPP
#define IMAGE_HPP

struct Color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

class PNGImage
{
private:
    const unsigned int _width;
    const unsigned int _height;
    const unsigned int _bit_depth;
    int _error_code;
    png_structp _png_ptr;
    png_infop _info_ptr;
    png_bytep *_rows_ptr;
    FILE *_file_ptr;

    static const std::unordered_map<int, std::string> error_messages;

    void init_image();
    void make_rows();
    void open_png_output_file(const std::string &filename);
    void build_image(const std::vector<Color> &colors);
    void cleanup(int error_code);

public:
    PNGImage(unsigned int image_width,
             unsigned int image_height,
             int image_bit_depth);

    virtual ~PNGImage();

    static void error_handler(png_structp png_ptr, png_const_charp msg);
    inline int error_code() const { return _error_code; }
    inline std::string error_message() const { return error_messages.at(error_code()); }
    inline unsigned int width() const { return _width; }
    inline unsigned int height() const { return _height; }
    inline unsigned int bit_depth() const { return _bit_depth; }
    void write(const std::string &filename, const std::vector<Color> &colors);
};

#endif

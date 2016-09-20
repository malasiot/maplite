#ifndef __PNG_WRITER_HPP__
#define __PNG_WRITER_HPP__

#include <vector>
#include <cstdint>

// encode RGBA buffer to PNG in-memory buffer
bool save_png(uint8_t *pixels, int w, int h, std::vector<uint8_t> &data) ;

// Base64 encoded transparent PNG tile
extern const char *g_empty_transparent_png_256 ;

#endif

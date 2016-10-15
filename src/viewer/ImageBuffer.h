#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include <cairo.h>
#include <string>

class ImageBuffer {

public:
    enum Format { ARGB32, RGB24, RGB565 } ;

    ImageBuffer(unsigned int width, unsigned int height, Format fmt = ARGB32) ;

    unsigned width() const ;
    unsigned height() const ;

    unsigned char *pixels() const ;

    void saveToPNG(const std::string &fileName) ;
    void saveToPNGBuffer(std::string &data);

    ~ImageBuffer();
private:

    friend class Renderer ;

    cairo_surface_t *surface_ ;
};







#endif

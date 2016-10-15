#include "ImageBuffer.h"

#include <cassert>
#include <cstring>
#include <stdlib.h>
#include <iostream>

using namespace std ;

ImageBuffer::ImageBuffer(unsigned int width, unsigned int height, ImageBuffer::Format fmt)
{
    cairo_format_t ft ;

    switch ( fmt )
    {
        case ARGB32:
            ft = CAIRO_FORMAT_ARGB32 ;
            break ;
        case RGB24:
            ft = CAIRO_FORMAT_RGB24 ;
            break ;
        case  RGB565 :
            ft = CAIRO_FORMAT_RGB16_565 ;
            break ;
    }

    surface_ = cairo_image_surface_create(ft, width, height) ;

}

ImageBuffer::~ImageBuffer()
{
    cairo_surface_destroy(surface_) ;
}

unsigned char *ImageBuffer::pixels() const
{
    assert(surface_) ;
    return cairo_image_surface_get_data(surface_) ;
}

unsigned int ImageBuffer::width() const {
    return cairo_image_surface_get_width(surface_) ;
}

unsigned int ImageBuffer::height() const {
    return cairo_image_surface_get_height(surface_) ;
}

void ImageBuffer::saveToPNG(const string &fileName) {

    assert(surface_) ;

    cairo_surface_write_to_png(surface_, fileName.c_str()) ;
}

#define DEFAULT_CHUNK_SIZE 4096

struct PNGWriteContext {

    unsigned char *buffer ;
    unsigned int capacity ;
    unsigned int sz ;
};

static cairo_status_t
write_png_stream_to_byte_array (void *in_closure, const unsigned char *data,
                                                unsigned int length)
{
    PNGWriteContext &ctx = *(PNGWriteContext *) in_closure;

    unsigned int capacity = ctx.capacity ;
    while ( ctx.capacity < ctx.sz + length )
        ctx.capacity *= 2 ;

    if ( capacity < ctx.capacity )
        ctx.buffer = (unsigned char *)realloc(ctx.buffer, ctx.capacity) ;

    memcpy (ctx.buffer + ctx.sz, data, length);

    ctx.sz += length ;

    return CAIRO_STATUS_SUCCESS;
}

void ImageBuffer::saveToPNGBuffer(string &data) {

    assert(surface_) ;

    PNGWriteContext ctx ;
    ctx.capacity =  DEFAULT_CHUNK_SIZE ;
    ctx.buffer = (unsigned char *)malloc(ctx.capacity) ;
    ctx.sz = 0 ;


    cairo_surface_write_to_png_stream(surface_, write_png_stream_to_byte_array, &ctx) ;

    data.assign(ctx.buffer, ctx.buffer + ctx.sz) ;
    free(ctx.buffer) ;
}

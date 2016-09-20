#include "png_writer.hpp"
#include <png.h>

using namespace std ;

static void png_write_callback(png_structp  png_ptr, png_bytep data, png_size_t length) {
    std::vector<uint8_t> *p = (std::vector<uint8_t>*)png_get_io_ptr(png_ptr);
    p->insert(p->end(), data, data + length);
}

bool save_png(uint8_t *pixels, int w, int h, vector<uint8_t> &data)
{
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if ( !png ) return false;

    png_infop info = png_create_info_struct(png);

    if (!info) {
        png_destroy_write_struct(&png, &info);
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, &info);
        return false ;
    }

    png_set_IHDR(png, info, w, h, 8 /* depth */, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_set_packing(png);

    png_bytep rows[h] ;

    for (int i = 0; i < h; ++i)
        rows[i] = (png_bytep)(pixels + i * w * 4);

    png_set_rows(png, info, &rows[0]);
    png_set_write_fn(png, (png_voidp)&data, png_write_callback, NULL);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png, &info);

    return true ;
}

const char *g_empty_transparent_png_256 =
        "iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAYAAABccqhmAAAABGdBTUEAALGPC/xhBQAAAAFzUkdC\
        AK7OHOkAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAZiS0dE\
        AP8A/wD/oL2nkwAAAAlwSFlzAAAASAAAAEgARslrPgAAARVJREFUeNrtwTEBAAAAwqD1T+1rCKAA\
        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHgDATwAAdgpQwQAAAAldEVYdGRhdGU6Y3JlYXRlADIw\
        MTYtMDQtMDFUMDk6MTE6MjMrMDM6MDCHKZUkAAAAJXRFWHRkYXRlOm1vZGlmeQAyMDE2LTA0LTAx\
        VDA5OjExOjIzKzAzOjAw9nQtmAAAAABJRU5ErkJggg==" ;

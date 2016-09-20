#include "jp2_decoder.hpp"
#include "logger.hpp"

#include <fstream>
#include <memory>

#include <cstring>
#include <png.h>

using namespace std ;

class DefaultLogger: public Logger
{
public:
    DefaultLogger() {
        addAppender(make_shared<LogStreamAppender>(Trace, make_shared<LogPatternFormatter>("%In function %c, %F:%l: %m"), std::cerr)) ;
    }
};


Logger &get_current_logger() {
    static DefaultLogger g_server_logger_ ;
    return g_server_logger_ ;
}

void write_pgm(const string &fname, uint32_t w, uint32_t h, uint8_t *data) {

    stringstream pgm_header ;
    pgm_header << "P5 " << w << ' ' << h << ' ' << 255 << endl ;

    ofstream ostrm(fname.c_str(), ios::binary) ;

    ostrm << pgm_header.str() ;

    for( uint32_t i=0 ; i<h ; i++)
    {
        uint8_t *src_ptr = data + i * w ;

        ostrm.write((const char *)src_ptr, w) ;
    }
}

static void png_write_callback(png_structp  png_ptr, png_bytep data, png_size_t length) {
    std::vector<uint8_t> *p = (std::vector<uint8_t>*)png_get_io_ptr(png_ptr);
    p->insert(p->end(), data, data + length);
}

static bool save_png(uint8_t *pixels, int w, int h, vector<uint8_t> &data)
{
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if ( !png ) return false;

    png_infop info = png_create_info_struct(png);

    if (!info) {
        png_destroy_write_struct(&png, &info);
        return false;
    }

    png_set_IHDR(png, info, w, h, 8 /* depth */, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_colorp palette = (png_colorp)png_malloc(png, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));

    if (!palette) {
        png_destroy_write_struct(&png, &info);
        return false;
    }

    png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
    png_set_packing(png);

    png_bytepp rows = (png_bytepp)png_malloc(png, h * sizeof(png_bytep));

    for (int i = 0; i < h; ++i)
        rows[i] = (png_bytep)(pixels + i * w * 4);

    png_set_rows(png, info, &rows[0]);
    png_set_write_fn(png, (png_voidp)&data, png_write_callback, NULL);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_free(png, palette);
    png_destroy_write_struct(&png, &info);

    delete[] rows;

    return true ;
}

int main(int argc, char *argv[]) {

    RasterTileCache cache(10000000) ;
    JP2Decoder decoder(&cache) ;

    decoder.open("/home/malasiot/GPS/mapping/hillshade.jp2") ;

    uint8_t *data = new uint8_t [2000*2000*4] ;
      memset(data, 0, 2000*2000*4) ;

//    decoder.read(120, 120, 2000, 2000, data, 2000*4) ;

     decoder.read(0, 0, 800, 800, 256, 256, data, 256*4) ;

      //write_pgm("/tmp/out.pgm", 256, 256, data ) ;

    vector<uint8_t> buf ;
    save_png(data, 256, 256, buf )  ;

    ofstream strm("/tmp/oo.png", ios::binary) ;
    strm.write((const char *)buf.data(), buf.size()) ;


    return 0;


}

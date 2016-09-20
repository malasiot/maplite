#ifndef __JP2_DECODER_H__
#define __JP2_DECODER_H__

// JPEG2000 decoder using openjpeg library
// currently only supporting single band 8 bit data (grayscale)

#include <stdio.h>

#include <string>

#include "raster_tile_source.hpp"
#include "raster_tile_cache.hpp"

class JP2Decoder: public RasterTileSource {
public:

    JP2Decoder(RasterTileCache *cache) ;

    // open file and read stream headers
    bool open(const std::string &file_name) ;

    // read a tile into memory
   RasterTileData read_tile(uint32_t ti, uint32_t tj) ;

private:

    bool is_valid_ ;
    uint32_t resolutions_ ;
    std::string file_name_ ;

} ;


#endif

#ifndef __RASTER_TILE_SOURCE_HPP__
#define __RASTER_TILE_SOURCE_HPP__

#include <cstdint>
#include <memory>

template< typename T >
struct array_deleter
{
  void operator ()( T const * p)
  {
    delete[] p;
  }
};

struct RasterTileData {

    RasterTileData() {}

    RasterTileData(const std::shared_ptr<uint8_t> &buffer, uint32_t stride, uint32_t height): height_(height), data_(buffer), stride_(stride) {}

    std::shared_ptr<uint8_t> data_ ;
    uint32_t stride_ ;
    uint32_t height_ ;
};

class RasterTileCache ;

class RasterTileSource {
public:

    RasterTileSource(RasterTileCache *cache): cache_(cache) {}

    // read a single tile (should be overidden by the driver)
    // the tile is in RGBA format
    virtual RasterTileData read_tile(uint32_t ti, uint32_t tj) = 0;

    // copy a region of the raster to an output buffer without scaling
    // the output buffer is in RGBA format. The function will put an alpha value of 255 for pixels that are copied
    // so that pixels out of raster will be made transparent

    virtual bool read(int32_t ox, int32_t oy, uint32_t wx, uint32_t wy, uint8_t *buffer, uint32_t stride) ;

    // read a region of the raster and scale it to fit the output buffer
    bool read(float src_ox, float src_oy, float src_wx, float src_wy,
                                uint32_t dst_wx, uint32_t dst_wy, uint8_t *buffer, uint32_t stride) ;

public:

    uint32_t image_width_ ;
    uint32_t image_height_ ;

    uint32_t tile_width_ ;
    uint32_t tile_height_ ;
    uint32_t n_tiles_x_ ;
    uint32_t n_tiles_y_ ;

    float georef_[6] ;

protected:

    RasterTileCache *cache_ ;
} ;


#endif

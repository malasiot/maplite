#ifndef __TILE_KEY_HPP__
#define __TILE_KEY_HPP__

struct TileKey {

    TileKey(uint32_t x, uint32_t y, uint8_t z, bool is_top_left = false):
        x_(x), y_(is_top_left ? (1 << z) - 1 -y : y), z_(z) {}
    
    TileKey toGoogle() const { return TileKey(x_, y_, z_, true) ; }
    
    uint32_t x() const { return x_ ; }
    uint32_t y() const { return y_ ; }
    uint8_t z() const { return z_ ; }

    uint32_t x_, y_ ;
    uint8_t z_ ;
} ;

#endif

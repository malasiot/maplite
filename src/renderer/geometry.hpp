#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cstdint>


struct LatLon {
    double lat_, lon_ ;
};


struct Coord {
    Coord(): x_(0), y_(0) {}
    Coord(double x, double y): x_(x), y_(y) {}
    double x_, y_ ;
};

struct BBox {

    double width() const { return maxx_ - minx_ ; }
    double height() const { return maxy_ - miny_ ; }

    bool contains(double x, double y) const { return ( x >= minx_ && y >= miny_ && x < maxx_ && y < maxy_ ) ; }
    bool intersects(const BBox &other) const {
        return (minx_ < other.maxx_ && maxx_ > other.minx_ && miny_ < other.maxy_ && maxy_ > other.miny_) ;
    }

    double minx_, miny_, maxx_, maxy_ ;
};




#endif

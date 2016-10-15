#ifndef __RASTER_H__
#define __RASTER_H__

#include <vector>
#include <boost/shared_ptr.hpp>

struct RasterData {
    unsigned int width, height, stride ;
    char *image ;
    double minx, miny, maxx, maxy ;
    double px ;
    double py ;

};



#endif

#ifndef __MAP_CONFIG_H__
#define __MAP_CONFIG_H__

#include <vector>
#include <cstdint>
#include <string>

#include "geometry.hpp"

struct ZoomInterval {
    ZoomInterval(): min_zoom_(-1), max_zoom_(-1), simplify_threshold_(0) {}

    int min_zoom_ ;
    int max_zoom_ ;
    float simplify_threshold_ ;
} ;

struct ZoomRange {
    std::vector<ZoomInterval> intervals_ ;

    bool isInZoomRange(uint z) {
        for (auto iv: intervals_) {
            if ( iv.min_zoom_ == -1 && z <= iv.max_zoom_ ) return true ;
            else if ( iv.max_zoom_ == -1 && z >= iv.min_zoom_ ) return true ;
            else if ( z >= iv.min_zoom_ && z <= iv.max_zoom_ ) return true ;
        }
        return false ;
    }
} ;


struct Layer {
    Layer() {}

    ZoomRange zr_ ;
    std::string name_ ;
};


struct MapConfig {
    MapConfig(): minz_(10), maxz_(16), has_bbox_(false) {}

    std::vector<Layer> layers_ ;
    BBox bbox_ ;
    std::string name_ ;
    std::string description_ ;
    std::string attribution_ ;
    int minz_, maxz_ ;
    bool has_bbox_ ;

    bool parse(const std::string &fileName) ;
};

#endif

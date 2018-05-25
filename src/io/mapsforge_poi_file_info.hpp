#ifndef MAPSFORGE_POI_FILE_INFO_HPP
#define MAPSFORGE_POI_FILE_INFO_HPP

#include <boost/optional.hpp>
#include "geometry.hpp"

struct POIFileInfo {
    double min_lat_, min_lon_, max_lat_, max_lon_ ;
    std::string comment_, languages_, created_by_ ;
    time_t date_ ;
    int version_ = 2 ;
    bool has_ways_, debug_ ;
    uint8_t flags_ = 0;
} ;

#endif

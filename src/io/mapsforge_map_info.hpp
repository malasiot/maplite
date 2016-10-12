#ifndef __MAPSFORGE_MAP_INFO_HPP__
#define __MAPSFORGE_MAP_INFO_HPP__

#include <string>
#include <memory>
#include <fstream>
#include <map>
#include <vector>
#include <mutex>

struct MapFileInfo {

    MapFileInfo(): flags_(0) {}

    uint32_t version_ ;
    uint64_t file_size_, header_size_ ;
    uint64_t date_ ;
    float min_lat_, min_lon_, max_lat_, max_lon_ ;
    uint8_t start_zoom_level_ ;
    float start_lon_, start_lat_ ;
    int16_t tile_sz_ ;
    std::string projection_ ;
    std::string lang_preference_, comment_, created_by_ ;
    uint8_t flags_ ;

    uint8_t min_zoom_level_ ;
    uint8_t max_zoom_level_ ;
};

#endif



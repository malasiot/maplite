#ifndef __GPX_READER_HPP__
#define __GPX_READER_HPP__

#include "feature_collection.hpp"

class GPXReader {
public:
    static bool load_from_file(const std::string &file_name, geojson::FeatureCollection &col) ;
    static bool load_from_string(const std::string &bytes, geojson::FeatureCollection &col) ;
};

// TODO: configure which fields to parse

#endif



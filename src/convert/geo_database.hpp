#ifndef __GEO_DATABASE_HPP__
#define __GEO_DATABASE_HPP__

#include <string>
#include <fstream>
#include "osm_document.hpp"

#include <geos/index/strtree/STRtree.h>

// flat file database of geometric primitives

class GeoDatabase {

    GeoDatabase() ;
    bool open(const std::string &fileName) ;

    bool addLineGeometry(OSM::DocumentReader &doc, const OSM::Way &way, uint8_t zmin, uint8_t zmax) ;
    bool addMultiLineGeometry(OSM::DocumentReader &doc, const std::vector<OSM::Way> &ways, int64_t id, int ft, uint8_t zmin, uint8_t zmax) ;
    bool addPointGeometry(const OSM::Node &poi, uint8_t zmin, uint8_t zmax) ;
    bool addPolygonGeometry(OSM::DocumentReader &doc, const OSM::Polygon &poly, int64_t id, int ft, uint8_t zmin, uint8_t zmax) ;

private:
    struct DataRecord {
        DataRecord() = default ;
        osm_id_t id_ ;
        int type_ ;
        uint8_t zmin_, zmax_ ;
        uint64_t offset_ ;
    };

    // in memory we keep only feature id and offset in the file
    std::vector<DataRecord> data_ ;

    // spatial index
    geos::index::strtree::STRtree index_ ;
    std::fstream strm_ ;
};



#endif

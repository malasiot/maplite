#ifndef __GEO_DATABASE_HPP__
#define __GEO_DATABASE_HPP__

#include <string>
#include <fstream>
#include "osm_document.hpp"
#include "tag_filter_context.hpp"
#include "geometry.hpp"

#include <geos/geom/Geometry.h>
#include <geos/index/strtree/STRtree.h>

#include <unordered_map>

// flat file database of geometric primitives
// spatial index and key-value list is kept in memory

class GeoDatabase {
public:
    // create temporary file
    GeoDatabase() ;

    bool addLineGeometry(OSM::DocumentReader &doc, const OSM::Way &way, uint8_t zmin, uint8_t zmax) ;
    bool addGeometry(geos::geom::Geometry *geom, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax) ;
    bool addMultiLineGeometry(OSM::DocumentReader &doc, const std::vector<OSM::Way> &ways, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax) ;
    bool addPointGeometry(const OSM::Node &poi, uint8_t zmin, uint8_t zmax) ;
    bool addPolygonGeometry(OSM::DocumentReader &doc, const OSM::Polygon &poly, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax) ;
    bool addTags(const TagWriteList &tags, osm_id_t id, osm_feature_t ftype);
    bool addTag(osm_id_t id, osm_feature_t ftype, const std::string &key, const std::string &val, uint8_t zmin=0, uint8_t zmax=0);
    bool addBoxGeometry(const BBox &box, osm_id_t id);

private:
    struct DataRecord {
        DataRecord() = default ;
        osm_id_t id_ ;
        osm_feature_t type_ ;
        uint8_t zmin_, zmax_ ;
        uint64_t offset_ ;
    };

    struct Tag {
        std::string key_, val_ ;
        uint8_t zmin_, zmax_ ;
    };

    // in memory we keep only feature id and offset in the file
    std::vector<DataRecord> data_ ;

    // spatial index
    geos::index::strtree::STRtree index_ ;

    // temporary file stream
    std::fstream strm_ ;

    // tag lists
    std::unordered_map<osm_id_t, Tag> ntags_, wtags_, rtags_ ;

    geos::geom::Envelope envelope_ ;
};



#endif

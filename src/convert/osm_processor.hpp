#ifndef __OSM_PROCESSOR_HPP__
#define __OSM_PROCESSOR_HPP__

#include "database.hpp"
#include "dictionary.hpp"
#include "osm_document.hpp"
#include "geometry.hpp"

#include "tag_filter_config_parser.hpp"
#include "tag_filter_context.hpp"

#include <map>
#include <spatialite.h>

// The class will pass all feature from each OSM file from the filter and store tags and geometries passing the rules to a spatialite database.

using std::string ;
using std::vector ;
using std::map ;

class OSMProcessor {
public:

    OSMProcessor() ;

    // Get handle to database

    SQLite::Connection &db() { return db_ ; }

    // create spatialite database tables (pois, lines, polygons)
    bool createGeometriesTable(const std::string &desc);

    // process each file
    bool processOsmFile(const string &path, TagFilter &filter) ;

    // requires as input a shape file containing the land polygon and clip box (bounding box of the map)
    bool processLandPolygon(const string &shape_file, const BBox &clip_box) ;

    // covers all map with land
    bool addDefaultLandPolygon(const BBox &clip_box) ;

    // computes envelop of all geometries in the database
    BBox getBoundingBoxFromGeometries();

    // iterate POI tags
    void forAllPOITags(std::function<void (const std::string &, const std::string &)>);
    // iterate way tags
    void forAllWayTags(std::function<void (const std::string &, const std::string &)>);

    void getTags(osm_id_t id, osm_feature_t ft, uint8_t minz, uint8_t maxz, Dictionary &tags);

    // iterate geometries within bounding box extended by buffer. Optionally clip and simplify geometries and compute centroid.
    bool forAllGeometries(const std::string &tableName, const BBox &bbox,
                          uint8_t minz, uint8_t maxz, bool clip, double buffer,
                          double tol, bool centroid,
                          std::function<void (gaiaGeomCollPtr, osm_id_t, osm_feature_t, uint8_t, uint8_t, double, double)> f);
private:
    bool create(const string &filePath) ;

    string insertFeatureSQL(const std::string &desc, const std::string &geomCmd = "?") ;

    bool addLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &doc, const OSM::Way &way, uint8_t zmin, uint8_t zmax) ;
    bool addMultiLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &doc, const std::vector<OSM::Way> &ways, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax) ;
    bool addPointGeometry(SQLite::Statement &cmd, const OSM::Node &poi, uint8_t zmin, uint8_t zmax) ;
    bool addPolygonGeometry(SQLite::Statement &cmd,  OSM::DocumentReader &doc, const OSM::Polygon &poly, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax) ;

    void addTags(const TagWriteList &tags, osm_id_t id, osm_feature_t ft);
    void addTag(osm_id_t id, osm_feature_t ftype, const std::string &key, const std::string &val, uint8_t zmin, uint8_t zmax);

private:

    SQLite::Connection db_ ;

    struct Tag {
        std::string key_, val_ ;
        uint8_t zmin_, zmax_ ;
    };

    std::unordered_multimap<osm_id_t, Tag> ntags_, wtags_, rtags_ ;

};

#endif

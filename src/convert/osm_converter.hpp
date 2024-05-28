#ifndef __OSM_CONVERTER_HPP__
#define __OSM_CONVERTER_HPP__

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

class OSMConverter {
public:

    OSMConverter(const std::string &out_file_path, const std::string &tags) ;

    // Get handle to database

    SQLite::Connection &db() { return db_ ; }

    // create spatialite database tables (pois, lines, polygons)
    bool createGeometriesTable(const std::string &desc);

    // process each file
    bool processOsmFile(const string &path) ;

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
    bool create(const string &filePath, const std::string &tags) ;

    string insertFeatureSQL(const std::string &desc, const std::string &geomCmd = "?") ;

    bool addLineGeometry(SQLite::Statement &cmd, OSM::Storage &doc, const OSM::Way &way) ;
    bool addMultiLineGeometry(SQLite::Statement &cmd, OSM::Storage &doc, const std::vector<OSM::Way> &ways, const Dictionary &tags, osm_id_t id, osm_feature_t ft) ;
    bool addPointGeometry(SQLite::Statement &cmd, const OSM::Node &poi) ;
    bool addPolygonGeometry(SQLite::Statement &cmd,  OSM::Storage &doc, const OSM::Polygon &poly, const Dictionary &tags, osm_id_t id, osm_feature_t ft) ;

    void addTags(const TagWriteList &tags, osm_id_t id, osm_feature_t ft);
    void addTag(osm_id_t id, osm_feature_t ftype, const std::string &key, const std::string &val, uint8_t zmin, uint8_t zmax);

private:

    SQLite::Connection db_ ;
    std::set<string> tags_ ;


};

#endif

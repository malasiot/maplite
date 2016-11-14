#ifndef __OSM_PROCESSOR_HPP__
#define __OSM_PROCESSOR_HPP__

#include "database.hpp"
#include "dictionary.hpp"
#include "osm_document.hpp"
#include "geometry.hpp"
#include "geo_database.hpp"

#include "tag_filter_config_parser.hpp"
#include "tag_filter_context.hpp"

#include <map>

// The class will pass all feature from each OSM file from the filter and store tags and geometries passing the rules to a spatialite database.

using std::string ;
using std::vector ;
using std::map ;

class OSMProcessor {
public:

    // Create a new database deleting any old file if it exists.

    bool create(const string &filePath) ;

    // Get handle to database

    SQLite::Connection &db() { return db_ ; }

    bool createGeometriesTable(const std::string &desc);
    bool createTagsTable() ;

    bool processOsmFile(const string &path, TagFilter &filter) ;

    // requires as input a shape file containing the land polygon and clip box (bounding box of the map)
    bool processLandPolygon(const string &shape_file, const BBox &clip_box) ;

    // covers all map with land
    bool addDefaultLandPolygon(const BBox &clip_box) ;

    BBox getBoundingBoxFromGeometries();

private:
    string insertFeatureSQL(const std::string &desc, const std::string &geomCmd = "?") ;

    bool addLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &doc, const OSM::Way &way, uint8_t zmin, uint8_t zmax) ;
    bool addMultiLineGeometry(SQLite::Statement &cmd, OSM::DocumentReader &doc, const std::vector<OSM::Way> &ways, int64_t id, int ft, uint8_t zmin, uint8_t zmax) ;
    bool addPointGeometry(SQLite::Statement &cmd, const OSM::Node &poi, uint8_t zmin, uint8_t zmax) ;
    bool addPolygonGeometry(SQLite::Statement &cmd,  OSM::DocumentReader &doc, const OSM::Polygon &poly, int64_t id, int ft, uint8_t zmin, uint8_t zmax) ;

    bool addTags(SQLite::Statement &cmd, const TagWriteList &tags, int64_t id, int ftype) ;

private:

    SQLite::Connection db_ ;
};

#endif

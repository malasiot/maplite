#ifndef __OSM_PROCESSOR_HPP__
#define __OSM_PROCESSOR_HPP__

#include "database.hpp"
#include "dictionary.hpp"
#include "osm_document.hpp"

#include "filter_config.hpp"

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

    SQLite::Database &handle() const { return *db_ ; }

    bool createGeometriesTable(const std::string &desc);
    bool createTagsTable() ;

    std::string insertFeatureSQL(const std::string &desc, const std::string &geomCmd = "?") ;

    bool processOsmFiles(const vector<string> &files, const FilterConfig &cfg) ;
    bool processShpFile(const string &file_name, const string &table_name, int srid, const string &char_enc) ;

private:

    bool matchRule(const OSM::Filter::RulePtr &rule, OSM::Filter::Context &ctx, TagWriteList &tw, bool &cont) ;

    bool addLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const OSM::Way &way ) ;
    bool addMultiLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const std::vector<OSM::Way> &ways, const std::string &id ) ;
    bool addPOIGeometry(SQLite::Command &cmd, const OSM::Node &poi ) ;
    bool addPolygonGeometry(SQLite::Command &cmd, const OSM::Document &doc, const OSM::Polygon &poly, const std::string &id) ;

    bool addTags(SQLite::Command &cmd, const TagWriteList &tags, const string &id) ;

private:

    std::shared_ptr<SQLite::Database> db_ ;

};

#endif

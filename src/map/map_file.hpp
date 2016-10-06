#ifndef __MAP_FILE_H__
#define __MAP_FILE_H__

#include "database.hpp"
#include "dictionary.hpp"
#include "osm_document.hpp"

#include "filter_config.hpp"

#include <map>

// The map file is a spatialite database. It is used as temporary storage for OSM data structured per layer and filtered using the configuration file.

using std::string ;
using std::vector ;
using std::map ;

class MapFile {
public:
    MapFile() ;
    ~MapFile() ;

    // Create a new database deleting any old file if it exists.

    bool create(const string &filePath) ;

    // Get handle to database

    SQLite::Database &handle() const { return *db_ ; }

    bool hasLayer(const std::string &layerName) const;

    bool createGeometriesTable(const std::string &desc);
    bool createTagsTable() ;

    std::string insertFeatureSQL(const std::string &desc, const std::string &geomCmd = "?") ;

    bool processOsmFiles(const vector<string> &files, const FilterConfig &cfg) ;
    bool processShpFile(const string &file_name, const string &table_name, int srid, const string &char_enc) ;

//    bool queryTile(const MapConfig &cfg, VectorTileWriter &tile) const ;

private:

    bool match_rule(const OSM::Filter::RulePtr &rule, OSM::Filter::Context &ctx, TagWriteList &tw, bool &cont) ;

    bool addLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const OSM::Way &way ) ;
    bool addMultiLineGeometry(SQLite::Command &cmd, OSM::Document &doc, const std::vector<OSM::Way> &ways, const std::string &id ) ;
    bool addPOIGeometry(SQLite::Command &cmd, const OSM::Node &poi ) ;
    bool addPolygonGeometry(SQLite::Command &cmd, const OSM::Document &doc, const OSM::Polygon &poly, const std::string &id) ;

    bool addTags(SQLite::Command &cmd, const TagWriteList &tags, const string &id) ;

    bool addOSMLayerPoints(OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                           const vector<NodeRuleMap > &node_idxs) ;

    bool addOSMLayerLines(OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                          const vector<NodeRuleMap> &way_idxs,
                          vector<OSM::Way> &chunk_list,
                          const vector<NodeRuleMap > &rule_map) ;

    bool addOSMLayerPolygons(const OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                             vector<OSM::Polygon> &polygons, const vector<NodeRuleMap > &poly_idxs) ;

    static string serializeTags(const Dictionary &tags) ;
    static void deserializeTags(const std::string &src, Dictionary &tags) ;


    SQLite::Database *db_ ;

public:

    std::string geom_column_name_ ;

};





#endif

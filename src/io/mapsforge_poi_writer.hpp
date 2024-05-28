#ifndef __MAPSFORGE_POI_WRITER_HPP__
#define __MAPSFORGE_POI_WRITER_HPP__

#include <string>
#include <memory>
#include <fstream>
#include <map>
#include <vector>
#include <mutex>

#include "dictionary.hpp"
#include "geometry.hpp"
#include "database.hpp"

#include "osm_document.hpp"
#include "osm_processor.hpp"

#include "mapsforge_poi_file_info.hpp"
#include "poi_categories.hpp"

#include <boost/optional.hpp>


struct ILatLon {
public:
    ILatLon() = default;
    ILatLon(double lat, double lon): lat_(round(lat*1.0e6)), lon_(round(lon*1.0e6)) {}
    int64_t lat_, lon_ ;
};


struct POIData {
    POIData() = default ;
    POIData(const ILatLon &c, osm_id_t id):
        coords_(c), id_(id) {}

    ILatLon coords_ ;
    Dictionary tags_ ;
    osm_id_t id_ ;
};

struct POIWriteOptions {
    bool debug_ ;

};

class POIWriter
{
public:

    // create an empty file
    void create(const std::string &file_path);

    // write data from spatialite database to mapsforge binary format
    void write(OSMProcessor &db, const POICategoryContainer &categories, POIWriteOptions &options) ;

    void setBoundingBox(const BBox &box) ;
    void setPreferredLanguages(const std::string &langs) ;
    void setComment(const std::string &comment) ;
    void setCreator(const std::string &creator) ;
    void setDebug(bool debug);
    void setDate(time_t t) ;

    BBox getBoundingBox() const {
        return BBox(info_.min_lon_, info_.min_lat_, info_.max_lon_, info_.max_lat_) ;
    }

private:

    void writeMetadata();
    void writeCategories(const POICategoryContainer &categories) ;
    void writePOIData(POIWriteOptions &options) ;

    POIFileInfo info_ ;
    SQLite::Connection db_ ;
    std::map<std::string, uint> cat_id_map_ ;

};



#endif



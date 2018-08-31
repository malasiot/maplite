#ifndef __MAPSFORGE_MAP_WRITER_HPP__
#define __MAPSFORGE_MAP_WRITER_HPP__

#include <string>
#include <memory>
#include <fstream>
#include <map>
#include <vector>
#include <mutex>

#include "dictionary.hpp"
#include "geometry.hpp"
#include "database.hpp"
#include "mapsforge_map_info.hpp"
#include "osm_document.hpp"
#include "osm_processor.hpp"

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

struct WayDataBlock {
    std::vector<std::vector<ILatLon>> coords_ ; // line string or polygon
};

struct WayDataContainer {

    Dictionary tags_ ;

    std::vector<WayDataBlock> blocks_ ;
    osm_id_t id_ ;
    boost::optional<ILatLon> label_pos_ ;
    enum Encoding { SingleDelta, DoubleDelta } encoding_ ;
    uint16_t subtile_mask_ ;
    BBox box_ ; // box of enclosed geometries
};

struct WriteOptions {

    std::vector<int> zoom_interval_conf_ = { 5,0,7, 10,8,11, 14,12,21 } ;

    void setZoomInterval(const std::string &s) ;

    float bbox_enlargement_ = 20 ;
    bool debug_ = false ;
    float simplification_factor_ = 2.5 ;
    bool label_positions_ = true ;
    bool polygon_clipping_ = true ;
    bool way_clipping_ = true ;
};

class MapFileWriter
{
public:

    // create an empty file
    void create(const std::string &file_path);

    // write data from spatialite database to mapsforge binary format
    void write(OSMProcessor &db, WriteOptions &options) ;

    void setBoundingBox(const BBox &box) ;
    void setBoundingBoxFromGeometries(OSMProcessor &db) ;
    void setStartPosition(float lat, float lon) ;
    void setStartZoom(uint8_t zoom) ;
    void setPreferredLanguages(const std::string &langs) ;
    void setComment(const std::string &comment) ;
    void setCreator(const std::string &creator) ;
    void setDebug(bool debug);

    void setZoomIntervalConf(const std::string &str) ;

    BBox getBoundingBox() const {
        return BBox(info_.min_lon_, info_.min_lat_, info_.max_lon_, info_.max_lat_) ;
    }

private:

    struct SubFileInfo {
        uint8_t base_zoom_ ;
        uint8_t min_zoom_ ;
        uint8_t max_zoom_ ;
        uint64_t offset_, size_, foffset_ ;
        std::vector<int64_t> index_ ; // tile offsets
        int32_t min_tx_, min_ty_, max_tx_, max_ty_ ; // range of tiles corresponding to the index
    };

private:
    void writeHeader(OSMProcessor &db, WriteOptions &options) ;
    void writeMapInfo();
    void writeTagList(const std::vector<std::string> &tags) ;
    void writeSubFileInfo(const WriteOptions &options);
    void writeSubFiles(OSMProcessor &db, const WriteOptions &options);
    void fetchTileData(int32_t tx, int32_t ty, int32_t tz, uint8_t min_zoom, uint8_t max_zoom, OSMProcessor &db, const WriteOptions &options,
                       std::vector<POIData> &pois, std::vector<std::vector<uint32_t> > &pois_per_level,
                       std::vector<WayDataContainer> &ways, std::vector<std::vector<uint32_t> > &ways_per_level);
    std::string writeTileData(int32_t tx, int32_t ty, int32_t tz, const WriteOptions &options, const std::vector<POIData> &pois,
                           const std::vector<std::vector<uint32_t> > &pois_per_level,
                           const std::vector<WayDataContainer> &ways, const std::vector<std::vector<uint32_t> > &ways_per_level);
    std::string writePOIData(const std::vector<POIData> &pois, const std::vector<std::vector<uint32_t>> &pois_per_level, const ILatLon &orig) ;
    std::string writeWayData(const std::vector<WayDataContainer> &ways, const std::vector<std::vector<uint32_t>> &ways_per_level, const ILatLon &orig) ;
    void computeSubTileMask(int tx, int ty, int tz, WayDataContainer &way, float bbox_enlargement) ;
private:

    MapFileInfo info_ ;
    std::fstream strm_ ;

    std::vector<std::string> way_tags_, poi_tags_ ;
    std::map<std::string, uint32_t> poi_tag_mapping_, way_tag_mapping_ ;
    std::vector<SubFileInfo> sub_files_ ;
    bool has_debug_info_ ;

};



#endif



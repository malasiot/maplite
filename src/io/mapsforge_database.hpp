#ifndef __MAPSFORGE_DATABASE_HPP__
#define __MAPSFORGE_DATABASE_HPP__

#include <string>
#include <memory>
#include <fstream>
#include <map>
#include <vector>
#include <mutex>

#include "dictionary.hpp"
#include "cache.hpp"
#include "tile_key.hpp"
#include "geometry.hpp"
#include "database.hpp"

#include <boost/optional.hpp>

struct POI {
    POI() = default ;
    POI(double lat, double lon): lat_(lat), lon_(lon) {}

    double lat_, lon_ ;
    Dictionary tags_ ;
};

struct Way {
    std::vector<std::vector<LatLon>> coords_ ; // line or polygon
    Dictionary tags_ ;
    int layer_ ;
    bool is_closed_ ;
};

struct VectorTile {
    bool is_sea_ ;
    std::vector<POI> pois_ ;
    std::vector<Way> ways_ ;
};

struct MapFileInfo {

    MapFileInfo(): flags_(0) {}

    uint32_t version_ ;
    uint64_t file_size_, header_size_ ;
    uint64_t date_ ;
    float min_lat_, min_lon_, max_lat_, max_lon_ ;
    uint8_t start_zoom_level_ ;
    float start_lon_, start_lat_ ;
    int16_t tile_sz_ ;
    std::string projection_ ;
    std::string lang_preference_, comment_, created_by_ ;
    uint8_t flags_ ;

    uint8_t min_zoom_level_ ;
    uint8_t max_zoom_level_ ;
};

struct WriteOptions {

    std::vector<uint8_t> zoom_interval_conf_ = { 5,0,7, 10,8,11, 14,12,21 } ;
    float bbox_enlargement_ = 20 ;
    bool debug_ = false ;
    float simplification_factor_ = 2.5 ;
    bool label_positions_ = true ;
    bool polygon_clipping_ = true ;
    bool way_clipping_ = true ;
};

struct TileData;
class MapFile ;
typedef std::tuple<uint32_t, uint32_t, uint8_t, MapFile *> cache_key_type; // the tile is encoded by its index and a dataset id
typedef Cache<cache_key_type, std::shared_ptr<TileData>> TileIndex ;

class MapFile
{
public:

    void open(const std::string &file_path, const std::shared_ptr<TileIndex> &index) ;

    const MapFileInfo &getMapFileInfo() const { return info_ ; }

    // read tile data corresponding to given tile key +- offset around it
    VectorTile readTile(const TileKey &, int offset = 1);

    // create an empty map file

    void create(const std::string &file_path);

    void write(SQLite::Database &db, WriteOptions &options) ;

    void setBoundingBox(float min_lat, float min_lon, float max_lat, float max_lon) ;
    void setBoundingBoxFromGeometries(SQLite::Database &db) ;
    void setStartPosition(float lat, float lon) ;
    void setStartZoom(uint8_t zoom) ;
    void setPreferredLanguages(const std::string &langs) ;
    void setComment(const std::string &comment) ;
    void setCreator(const std::string &creator) ;
    void setDebug(bool debug);

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

    void readHeader() ;
    void readMapInfo() ;
    void readTagList(std::vector<std::string> &tags) ;
    void readSubFileInfo() ;
    void readTileIndex() ;

    uint64_t readTileData(const SubFileInfo &info, int64_t offset, std::shared_ptr<TileData> &) ;
    uint64_t readPOI(POI &poi, float lat, float lon);
    uint64_t readWays(std::vector<Way> &ways, float lat, float lon);
    void readWayNodesDoubleDelta(std::vector<LatLon> &coord_list, double tx0, double ty0) ;
    void readWayNodesSingleDelta(std::vector<LatLon> &coord_list, double tx0, double ty0) ;
    void exportTileDataOSM(const VectorTile &data, const std::string &filename) ;
    void readTiles() ;


    void writeHeader(SQLite::Database &db, WriteOptions &options) ;
    void writeMapInfo();
    void writeTagList(const std::vector<std::string> &tags) ;
    void writeSubFileInfo(const WriteOptions &options);
    void writeSubFiles(SQLite::Database &db, const WriteOptions &options);
    uint64_t writeTileData(int32_t tx, int32_t ty, int32_t tz, uint8_t min_z, uint8_t max_z, SQLite::Database &db, const WriteOptions &options);

private:

    std::shared_ptr<TileIndex> index_ ;
    MapFileInfo info_ ;
    std::fstream strm_ ;

    std::string map_file_path_ ;
    std::vector<std::string> way_tags_ ;
    std::vector<std::string> poi_tags_ ;
    std::vector<SubFileInfo> sub_files_ ;
    bool has_debug_info_ ;
    std::mutex mtx_ ;
};



#endif



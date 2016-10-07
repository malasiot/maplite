#ifndef __MAPSFORGE_MAP_READER_HPP__
#define __MAPSFORGE_MAP_READER_HPP__

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

struct POI {
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

    uint32_t version_ ;
    uint64_t file_size_ ;
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

struct TileData;
class MapFile ;
typedef std::tuple<uint32_t, uint32_t, uint8_t, MapFile *> cache_key_type; // the tile is encoded by its index and a dataset id
typedef Cache<cache_key_type, std::shared_ptr<TileData>> TileIndex ;

class MapFile
{
public:

    MapFile() {}

    /**
     * Opens map file, reads map info and create tile index
     *
     * @throws ReadException on error
     *
     */

    void open(const std::string &file_path) ;

    ~MapFile() {}

    const MapFileInfo &getMapFileInfo() const { return info_ ; }

    // read tile data corresponding to given tile key +- offset around it
    VectorTile readTile(const TileKey &, int offset = 1);

    void readTiles() ;

private:

    struct SubFileInfo {
        uint8_t base_zoom_ ;
        uint8_t min_zoom_ ;
        uint8_t max_zoom_ ;
        uint64_t offset_, size_ ;
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

    uint32_t read_uint32() ;
    uint64_t read_uint64() ;
    int32_t  read_int32() ;
    int64_t  read_int64() ;
    int16_t  read_int16() ;
    uint16_t read_uint16() ;
    uint8_t  read_uint8() ;
    int8_t   read_int8() ;
    std::string read_utf8() ;
    uint64_t read_var_uint64() ;
    int64_t  read_var_int64() ;
    int64_t  read_offset();

private:

    std::shared_ptr<TileIndex> index_ ;
    MapFileInfo info_ ;
    std::ifstream strm_ ;
    std::string map_file_path_ ;
    std::vector<std::string> way_tags_ ;
    std::vector<std::string> poi_tags_ ;
    std::vector<SubFileInfo> sub_files_ ;
    bool has_debug_info_ ;
    std::mutex mtx_ ;


};



#endif



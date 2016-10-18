#include "mapsforge_map_reader.hpp"
#include "serializer.hpp"
#include "tms.hpp"

#include <fstream>
#include <iomanip>
#include <cstring>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>


using namespace std ;

void MapFileReader::open(const std::string &file_path)
{
    strm_.open(file_path.c_str(), ios::in | ios::binary) ;

    if ( !strm_ ) throw std::runtime_error("error openning file") ;

    readHeader() ;
    readTileIndex() ;
}

struct TileData {
    TileData(uint32_t tx, uint32_t ty, uint32_t tz, bool is_sea_tile):
        x_(tx), y_(ty), z_(tz), is_sea_(is_sea_tile) {}

    uint32_t x_, y_, z_ ;
    bool is_sea_ ;
    std::vector<std::vector<POI>> pois_per_level_ ;
    std::vector<std::vector<Way>> ways_per_level_ ;

    uint64_t payload() const {
        uint64_t total = sizeof(TileData) ;
        for( uint i=0 ; i<pois_per_level_.size() ; i++) {
            uint n = pois_per_level_[i].size() ;
            for( uint j=0 ; j<n ; j++ ) {
                const POI &poi = pois_per_level_[i][j] ;
                DictionaryIterator it(poi.tags_) ;

            }
        }
    }
};

VectorTile MapFileReader::readTile(const TileKey &key, int offset)
{

    BBox query_box ;
    tms::tileBounds(key.x(), key.y(), key.z(), query_box.minx_, query_box.miny_, query_box.maxx_, query_box.maxy_) ;
    // crop zoom level to map zoom interval

    TileKey gt = key.toGoogle() ;

    uint8_t zoom ;
    zoom = std::min(info_.max_zoom_level_, gt.z()) ;
    zoom = std::max(info_.min_zoom_level_, zoom) ;

    // find subfile containing requested zoom level

    uint sub_file_idx = 0 ;

    for(uint i=0 ; i<sub_files_.size() ; i++ ) {
        const SubFileInfo &info = sub_files_[i] ;
        if ( zoom >= info.min_zoom_ && zoom <= info.max_zoom_ ) {
            sub_file_idx = i ;
            break ;
        }
    }

    const SubFileInfo &si = sub_files_[sub_file_idx] ;

    uint32_t rows = si.max_ty_ - si.min_ty_ + 1 ;
    uint32_t cols = si.max_tx_ - si.min_tx_ + 1 ;
    uint64_t tile_count = rows * cols ;


    bool use_bitmask = false;

    int32_t base_tile_min_x = si.max_tx_, base_tile_min_y = si.max_ty_ ;
    int32_t base_tile_max_x = si.min_tx_, base_tile_max_y = si.min_ty_ ;

    // we visit all tiles around the query tile and calculate which blocks should be loaded

    for( int32_t tx = gt.x() - offset ; tx <= gt.x() + offset; tx++ ) {
        for( int32_t ty = gt.y() - offset ; ty <= gt.y() + offset; ty++ ) {

            int32_t block_min_x, block_min_y;
            int32_t block_max_x, block_max_y;

            if ( gt.z() < si.base_zoom_ ) {
                // calculate the XY numbers of the upper left and lower right sub-tiles
                const int zoom_diff = (int8_t)si.base_zoom_ - (int8_t)gt.z() ;
                block_min_x = (tx << zoom_diff) ;
                block_min_y = (ty << zoom_diff) ;
                block_max_x = block_min_x + ( 1 << zoom_diff ) - 1 ;
                block_max_y = block_min_y + ( 1 << zoom_diff ) - 1 ;
            } else if ( gt.z() > si.base_zoom_ ) {
                // calculate the XY numbers of the parent base tile
                const int zoom_diff = (int8_t)gt.z() - (int8_t)si.base_zoom_  ;
                block_min_x = tx >> zoom_diff ;
                block_min_y = ty >> zoom_diff ;
                block_max_x = block_min_x ;
                block_max_y = block_min_y  ;

                use_bitmask = true;
            } else {
                block_min_x = tx ;
                block_min_y = ty ;
                block_max_x = block_min_x ;
                block_max_y = block_min_y  ;
            }

            base_tile_min_x = std::min(base_tile_min_x, block_min_x) ;
            base_tile_max_x = std::max(base_tile_max_x, block_max_x) ;
            base_tile_min_y = std::min(base_tile_min_y, block_min_y) ;
            base_tile_max_y = std::max(base_tile_max_y, block_max_y) ;
        }
    }

    VectorTile tile ;

    // load required base tiles from file or cache

    for( int bty = base_tile_min_y ; bty<= base_tile_max_y ; bty++ )
        for( int btx = base_tile_min_x ; btx<= base_tile_max_x ; btx++ )
        {
            if ( bty < si.min_ty_ || bty > si.max_ty_ ||
                 btx < si.min_tx_ || btx > si.max_tx_ ) continue ;

            BBox bbox ;
            TileKey bt(btx, bty, si.base_zoom_, true) ;
            tms::tileBounds(bt.x(), bt.y(), bt.z(), bbox.minx_, bbox.miny_, bbox.maxx_, bbox.maxy_) ;

            bool ignore_ways = !bbox.intersects(query_box);

            int row = bty - si.min_ty_ ;
            int col = btx - si.min_tx_ ;

            uint64_t idx = cols * row + col ; // this is the index to tile index array

            int64_t tile_offset = si.index_[idx] ;

            bool is_sea_tile = ( tile_offset & 0x8000000000LL ) != 0 ;
            tile_offset = tile_offset & 0x7FFFFFFFFFLL ;

            BaseTile base_tile(TileKey(btx, bty, si.base_zoom_, true)) ;

            std::shared_ptr<TileData> data ;

            cache_key_type key(btx, bty, si.base_zoom_, this) ;

            if ( g_tile_index_ ) {
                g_tile_index_->fetch(key,
                                     [this, is_sea_tile, si, tile_offset] ( const cache_key_type &key, cache_value_type  &val ) -> uint64_t {
                    // if not in cache load from disk

                    uint32_t tx = std::get<0>(key) ;
                    uint32_t ty = std::get<1>(key) ;
                    uint32_t tz = std::get<2>(key) ;
                    val.reset(new TileData(tx, ty, tz, is_sea_tile)) ;
                    return readTileData(si, tile_offset, val) ;
                },
                data) ;
            }
            else {
                data.reset(new TileData(btx, bty, si.base_zoom_, is_sea_tile)) ;
                readTileData(si, tile_offset, data) ;
            }

            // copy all ways and pois at zoom level equal or lower the requested zoom level

            if ( !data ) continue ;

            for(int z=zoom ; z>=si.min_zoom_ ; z-- ) {
                int idx = z - (int)si.min_zoom_ ;

                std::copy(data->pois_per_level_[idx].begin(), data->pois_per_level_[idx].end(),
                          std::back_inserter(base_tile.pois_)) ;
                if ( !ignore_ways )
                    std::copy(data->ways_per_level_[idx].begin(), data->ways_per_level_[idx].end(),
                              std::back_inserter(base_tile.ways_)) ;
            }


            if ( is_sea_tile ) {
                base_tile.is_sea_ = true ;
                BBox lbox ;
                tms::tileLatLonBounds(bt.x(), bt.y(), bt.z(), lbox.miny_, lbox.minx_, lbox.maxy_, lbox.maxx_) ;

                Way sea ;
                sea.tags_.add("natural", "sea") ;
                sea.tags_.add("area", "yes") ;
                sea.coords_.resize(1) ;
                sea.coords_[0].emplace_back(lbox.miny_, lbox.minx_) ;
                sea.coords_[0].emplace_back(lbox.maxy_, lbox.minx_) ;
                sea.coords_[0].emplace_back(lbox.maxy_, lbox.maxx_) ;
                sea.coords_[0].emplace_back(lbox.miny_, lbox.maxx_) ;
                sea.coords_[0].emplace_back(lbox.miny_, lbox.minx_) ;

                base_tile.ways_.emplace_back(sea) ;
            }

            tile.base_tiles_.emplace_back(base_tile) ;

        }

//    exportTileDataOSM(tile, "/tmp/oo.osm");

    return tile ;
}

std::shared_ptr<TileIndex> MapFileReader::g_tile_index_ ;

void MapFileReader::initTileCache(uint64_t bytes) {
    g_tile_index_.reset(new TileIndex(bytes)) ;
}

void MapFileReader::readHeader()
{
    MapFileISerializer s(strm_) ;

    // read the magic bytes

    char header[20] ;
    s.read_bytes((uint8_t *)header, 20) ;

    if ( strncmp(header, "mapsforge binary OSM", 20) != 0 )
        throw std::runtime_error( "Invalid or corrupted header") ;

    // read map info

    uint32_t header_length = s.read_uint32() ;

    readMapInfo() ;

    readTagList(poi_tags_) ;
    readTagList(way_tags_) ;

    readSubFileInfo() ;
}

void MapFileReader::readMapInfo()
{
    MapFileISerializer s(strm_) ;

    info_.version_ = s.read_uint32() ;
    info_.file_size_ = s.read_uint64() ;
    info_.date_ = s.read_uint64() ;

    // bounding box

    info_.min_lat_ = s.read_int32() / 1.0e6 ;
    info_.min_lon_ = s.read_int32() / 1.0e6 ;
    info_.max_lat_ = s.read_int32() / 1.0e6 ;
    info_.max_lon_ = s.read_int32() / 1.0e6 ;

    info_.tile_sz_ = s.read_int16() ;

    if ( info_.tile_sz_ <= 0 )
        throw std::runtime_error(str(boost::format("Invalid tile size (%d px)") % info_.tile_sz_)) ;

    info_.projection_ = s.read_utf8() ;
    info_.flags_ = s.read_uint8() ;

    has_debug_info_ = info_.flags_ & 0x80 ;

    if ( info_.flags_ & 0x40 ) {
        info_.start_lat_ = s.read_int32() / 1.0e6 ;
        info_.start_lon_ = s.read_int32() / 1.0e6;
    }

    if ( info_.flags_ & 0x20 )
        info_.start_zoom_level_ = s.read_uint8() ;
    else
        info_.start_zoom_level_ = 10 ;

    if ( info_.flags_ & 0x10 )
        info_.lang_preference_ = s.read_utf8() ;
    else
        info_.lang_preference_ = "en" ;

    if ( info_.flags_ & 0x08 )
        info_.comment_ = s.read_utf8() ;

    if ( info_.flags_ & 0x04 )
        info_.created_by_ = s.read_utf8() ;
}

void MapFileReader::readTagList(std::vector<string> &tags)
{
    MapFileISerializer s(strm_) ;

    uint16_t n_tags = s.read_uint16() ;

    for( uint i=0 ; i<n_tags ; i++ ) {
        tags.push_back(s.read_utf8()) ;
    }
}

void MapFileReader::readSubFileInfo()
{
    MapFileISerializer s(strm_) ;

    uint8_t num_zoom_intervals = s.read_uint8() ;

    sub_files_.resize(num_zoom_intervals) ;

    info_.min_zoom_level_ = std::numeric_limits<uint8_t>::max();
    info_.max_zoom_level_ = std::numeric_limits<uint8_t>::min();

    for( uint i=0 ; i<num_zoom_intervals ; i++ ) {
        SubFileInfo &info = sub_files_[i] ;

        info.base_zoom_ = s.read_uint8() ;
        info.min_zoom_ = s.read_uint8() ;
        info.max_zoom_ = s.read_uint8() ;
        info.offset_ = s.read_uint64() ;
        info.size_ = s.read_uint64() ;

        info_.min_zoom_level_ = std::min(info_.min_zoom_level_, info.min_zoom_) ;
        info_.max_zoom_level_ = std::max(info_.max_zoom_level_, info.max_zoom_) ;

        int32_t min_ty, max_ty ;

        tms::tilesWithinBounds(info_.min_lat_, info_.min_lon_, info_.max_lat_, info_.max_lon_, info.base_zoom_,
                               info.min_tx_, min_ty, info.max_tx_, max_ty) ;

        // comvert to Google tile coordinates
        info.max_ty_ = (1 << info.base_zoom_) - min_ty - 1 ;
        info.min_ty_ = (1 << info.base_zoom_) - max_ty - 1 ;
    }
}

void MapFileReader::readTileIndex()
{
    MapFileISerializer s(strm_) ;

    for( uint i=0 ; i<sub_files_.size() ; i++ ) {

        SubFileInfo &info = sub_files_[i] ;
        strm_.seekg(info.offset_) ;

        if ( has_debug_info_ ) {
            char signature[16] ;
            strm_.read(signature, 16) ;
        }

        // determine tiles covered by the bounding box on the base zoom level

        uint32_t rows = info.max_ty_ - info.min_ty_ + 1 ;
        uint32_t cols = info.max_tx_ - info.min_tx_ + 1 ;
        uint64_t tile_count = rows * cols ;

        for( uint j=0 ; j<tile_count ; j++ ) {
            int64_t offset = s.read_offset() ;
            info.index_.push_back(offset) ;
        }
    }
}

void MapFileReader::readTiles()
{
    for( uint i=0 ; i<sub_files_.size() ; i++ ) {
        SubFileInfo &info = sub_files_[i] ;

        // determine tiles covered by the bounding box on the base zoom level

        uint32_t rows = info.max_ty_ - info.min_ty_ + 1 ;
        uint32_t cols = info.max_tx_ - info.min_tx_ + 1 ;
        uint64_t tile_count = rows * cols ;

        for( uint j=0 ; j<tile_count ; j++ ) {
            int64_t tile_offset = info.index_[j] ;

            int32_t row = j / cols ;
            int32_t col = j % cols ;

            int32_t tx = col + info.min_tx_ ;
            int32_t ty = row + info.min_ty_ ;

            bool is_sea_tile = ( tile_offset & 0x8000000000LL ) != 0 ;

            std::shared_ptr<TileData> data(new TileData(tx, ty, info.base_zoom_, is_sea_tile)) ;

            if ( !is_sea_tile ) {
                tile_offset = tile_offset & 0x7FFFFFFFFFLL ;
                readTileData(info, tile_offset, data) ;
            }
        }
    }
}

uint64_t MapFileReader::readTileData(const SubFileInfo &info, int64_t offset, std::shared_ptr<TileData> &data) {

    MapFileISerializer s(strm_) ;

    strm_.seekg(info.offset_ + offset) ;

    if ( has_debug_info_ ) {
        char signature[32] ;
        strm_.read(signature, 32) ;
        cout << signature << endl ;
    }

    // read zoom table

    int nz = info.max_zoom_ - info.min_zoom_ + 1 ;

    data->pois_per_level_.resize(nz) ;
    data->ways_per_level_.resize(nz) ;

    vector<uint64_t> num_pois_per_level, num_ways_per_level ;
    uint64_t total_pois = 0, total_ways = 0 ;

    for ( uint i=0 ; i<nz ; i++ ) {
        uint64_t n_pois = s.read_var_uint64() ;
        uint64_t n_ways = s.read_var_uint64() ;

        total_pois += n_pois ;
        total_ways += n_ways ;

        num_pois_per_level.push_back(n_pois) ;
        num_ways_per_level.push_back(n_ways) ;
    }

    // get the relative offset to the first stored way in the block

    uint64_t first_way_offset = s.read_var_uint64() ;

    // convert to absolute position

    uint64_t first_way_pos = (uint64_t)strm_.tellg() + first_way_offset ;

    // read pois

    double min_lat, min_lon, max_lat, max_lon ;
    tms::tileLatLonBounds(data->x_, (1 << data->z_)-data->y_ - 1, data->z_, min_lat, min_lon, max_lat, max_lon) ;

    uint64_t bytes = 0 ;

    for(uint i=0 ; i<nz ; i++) {
        uint64_t n = num_pois_per_level[i] ;

        data->pois_per_level_[i].resize(n) ;

        for( uint j=0 ; j<n ; j++ )
            bytes += readPOI(data->pois_per_level_[i][j], max_lat, min_lon) ;
    }

    // read ways

    strm_.seekg(first_way_pos) ;

    for(uint i=0 ; i<nz ; i++) {
        uint64_t n = num_ways_per_level[i] ;


        for( uint j=0 ; j<n ; j++ )
        {
            vector<Way> ways ;
            bytes += readWays(ways, max_lat, min_lon) ; // we break ways split into multiple blocks into separate ways

            for(uint k=0 ; k<ways.size() ; k++ )
                data->ways_per_level_[i].push_back(std::move(ways[k])) ;
        }
    }

    return bytes ;
}

static void decode_key_value(const string &kv, string &key, string &val) {

    using boost::tokenizer;
    using boost::escaped_list_separator;

    boost::char_separator<char> sep("=");
    typedef tokenizer<boost::char_separator<char> > stokenizer;

    stokenizer tok(kv, sep);

    vector<string> tokens ;

    for( stokenizer::iterator beg = tok.begin(); beg!=tok.end(); ++beg)
        if ( !beg->empty() ) tokens.push_back(*beg) ;

    if ( tokens.size() == 2 ) {
        key = tokens[0] ;
        val = tokens[1] ;
    }
}

uint64_t MapFileReader::readPOI(POI &poi, float lat_orig, float lon_orig) {

    MapFileISerializer s(strm_) ;

    uint64_t bytes = 0 ;

    if ( has_debug_info_ ) {
        uint8_t signature[32] ;
        s.read_bytes(signature, 32) ;
        cout << signature << endl ;
    }

    double lat_diff = (double)s.read_var_int64()/1.0e6 ;
    double lon_diff = (double)s.read_var_int64()/1.0e6 ;
    poi.lat_ = lat_orig + lat_diff ;
    poi.lon_ = lon_orig + lon_diff ;

    bytes += sizeof( double ) * 2 ;

    uint8_t tflag = s.read_uint8() ;

    int8_t layer = int((tflag & 0xf0) >> 4) - 5 ;
    uint8_t ntags = tflag & 0x0f ;

    for( uint i=0 ; i<ntags ; i++ ) {
        uint64_t tag_index = s.read_var_uint64() ;

        string kv = poi_tags_[tag_index], tag, val ;
        decode_key_value(kv, tag, val);

        poi.tags_.add(tag, val) ;
    }

    uint8_t cflag = s.read_uint8() ;

    if ( cflag & 0x80 )
        poi.tags_.add("name", s.read_utf8()) ;

    if ( cflag & 0x40 )
        poi.tags_.add("addr:housenumber", s.read_utf8()) ;

    if ( cflag & 0x20 )
        poi.tags_.add("ele", std::to_string(s.read_var_int64())) ;

    if ( layer != 0 )
        poi.tags_.add("layer", std::to_string(layer)) ;

    bytes += poi.tags_.capacity() ;

    return bytes ;
}

uint64_t MapFileReader::readWays(vector<Way> &ways, float lat_orig, float lon_orig) {

    MapFileISerializer s(strm_) ;

    uint64_t bytes = 0 ;

    if ( has_debug_info_ ) {
        char signature[32] ;
        strm_.read(signature, 32) ;
        cout << signature << endl ;
    }

    uint64_t data_sz = s.read_var_uint64() ;
    uint16_t tile_bitmap = s.read_uint16() ;

    uint8_t tflag = s.read_uint8() ;

    int8_t layer = int((tflag & 0xf0) >> 4) - 5 ;
    uint8_t ntags = tflag & 0x0f ;

    Dictionary tags ;

    for( uint i=0 ; i<ntags ; i++ ) {
        uint64_t tag_index = s.read_var_uint64() ;

        string kv = way_tags_[tag_index], tag, val ;
        decode_key_value(kv, tag, val);

        tags.add(tag, val) ;
    }

    uint8_t cflag = s.read_uint8() ;

    if ( cflag & 0x80 )
        tags.add("name", s.read_utf8()) ;

    if ( cflag & 0x40 )
        tags.add("addr:housenumber", s.read_utf8()) ;

    if ( cflag & 0x20 )
        tags.add("ref", s.read_utf8()) ;

    boost::optional<LatLon> label_pos ;

    if ( cflag & 0x10 ) {
        double label_position_lat_diff = s.read_var_int64()/1.0e6 ;
        double label_position_lon_diff = s.read_var_int64()/1.0e6 ;

        label_pos = LatLon(label_position_lat_diff + lat_orig, label_position_lon_diff + lon_orig) ;
    }

    bytes += tags.capacity() ;

    uint64_t n_data_blocks = 1 ;

    if ( cflag & 0x08 )
        n_data_blocks = s.read_var_uint64() ;

    ways.resize(n_data_blocks) ;

    for(uint i=0 ; i<n_data_blocks ; i++) {
        Way way ;
        way.tags_ = tags ;
        way.layer_ = layer ;
        way.label_pos_ = label_pos ;

        uint64_t n_way_coord_blocks = s.read_var_uint64() ;

        way.coords_.resize(n_way_coord_blocks) ;

        for(uint j=0 ; j<n_way_coord_blocks ; j++ ) {
            uint64_t num_way_nodes = s.read_var_uint64() ;

            way.coords_[j].resize(num_way_nodes) ;

            bytes += num_way_nodes * 2 * sizeof(double) ;

            if ( cflag & 0x04 ) // double delta encoding
                readWayNodesDoubleDelta(way.coords_[j], lat_orig, lon_orig);
            else
                readWayNodesSingleDelta(way.coords_[j], lat_orig, lon_orig);
        }

        const vector<LatLon> &coords = way.coords_[0] ;
        double lat_diff = fabs(coords.front().lat_ - coords.back().lat_) ;
        double lon_diff = fabs(coords.front().lon_ - coords.back().lon_) ;

        way.is_closed_ = ( lat_diff < 1.0e-5 && lon_diff < 1.0e-5 ) ;


        ways[i] = std::move(way) ;
    }

    return bytes ;

}

void MapFileReader::readWayNodesDoubleDelta(std::vector<LatLon> &coord_list, double tx0, double ty0)
{
    MapFileISerializer s(strm_) ;

    double lat = s.read_var_int64()/1.0e6 + tx0 ;
    double lon = s.read_var_int64()/1.0e6 + ty0 ;

    coord_list[0] = LatLon{lat, lon} ;

    float previous_delta_lat = 0, previous_delta_lon = 0 ;

    for( uint i=1 ; i<coord_list.size() ; i++ ) {

        double delta_lat = previous_delta_lat + s.read_var_int64()/1.0e6 ;
        double delta_lon = previous_delta_lon + s.read_var_int64()/1.0e6 ;

        lat += delta_lat ;
        lon += delta_lon ;

        previous_delta_lat = delta_lat ;
        previous_delta_lon = delta_lon ;

        coord_list[i] = LatLon{lat, lon} ;
    }
}

void MapFileReader::readWayNodesSingleDelta(std::vector<LatLon> &coord_list, double tx0, double ty0)
{
    MapFileISerializer s(strm_) ;

    double lat = s.read_var_int64()/1.0e6 + tx0 ;
    double lon = s.read_var_int64()/1.0e6 + ty0 ;

    coord_list[0] = LatLon{lat, lon} ;

    for(uint i=1 ; i<coord_list.size() ; i++ ) {

        lat += s.read_var_int64()/1.0e6 ;
        lon += s.read_var_int64()/1.0e6 ;

        coord_list[i] = LatLon{lat, lon} ;
    }
}

static string escape_xml_string(const std::string &src) {
    std::string buffer;
    buffer.reserve(src.size());
    for(size_t pos = 0; pos != src.size(); ++pos) {
        switch(src[pos]) {
        case '&':  buffer.append("&amp;");       break;
        case '\"': buffer.append("&quot;");      break;
        case '\'': buffer.append("&apos;");      break;
        case '<':  buffer.append("&lt;");        break;
        case '>':  buffer.append("&gt;");        break;
        default:   buffer.append(&src[pos], 1); break;
        }
    }
    return buffer ;
}

void MapFileReader::exportTileDataOSM(const VectorTile &data, const string &filename)
{
    ofstream strm(filename.c_str()) ;

    strm << "<?xml version='1.0' encoding='UTF-8'?>\n" ;
    strm << "<osm version='0.6' generator='JOSM'>\n" ;

    int64_t count = -10000000 ;

    for( const BaseTile &bt: data.base_tiles_ ) {
        for(int i=0 ; i<bt.pois_.size() ; i++ ) {
            const POI &poi = bt.pois_[i] ;

            strm << "<node id='" << count++ << "' visible='true' lat='" << setprecision(12) << poi.lat_ <<
                    "' lon='" << setprecision(12) << poi.lon_  ;

            if ( poi.tags_.empty() ) strm <<  "' />\n" ;
            else
            {
                strm << "' >\n" ;

                DictionaryIterator it(poi.tags_) ;

                while ( it ) {
                    strm << "<tag k='" << it.key() << "' v='" << escape_xml_string(it.value()) << "' />\n" ;
                    ++it ;
                }

                strm << "</node>\n" ;
            }
        }
    }

    int64_t wcount = -20000000 ;

    int64_t count0 = count ;

    for( const BaseTile &bt: data.base_tiles_ ) {
        for(int i=0 ; i<bt.ways_.size() ; i++ )
        {
            const Way &way = bt.ways_[i] ;

            for ( uint k = 0 ; k<way.coords_.size() ; k++ ) {

                const vector<LatLon> &coords = way.coords_[k] ;

                for( uint j=0 ; j<coords.size() ; j++ ) {
                    strm << "<node id='" << count + j << "' visible='true' lat='" << setprecision(12) << coords[j].lat_ <<
                            "' lon='" << setprecision(12) << coords[j].lon_  <<  "' />\n" ;
                }

                count += coords.size() ;
            }

        }
    }

    count = count0 ;

    for( const BaseTile &bt: data.base_tiles_ ) {
        for(int i=0 ; i<bt.ways_.size() ; i++ )
        {
            const Way &way = bt.ways_[i] ;

            for ( uint k = 0 ; k<way.coords_.size() ; k++ ) {

                const vector<LatLon> &coords = way.coords_[k] ;

                strm << "<way id='" << -100000000 + wcount++ << "' action='modify' visible='true'>\n" ;

                DictionaryIterator it(way.tags_) ;

                while ( it ) {
                    strm << "<tag k='" << it.key() << "' v='" << escape_xml_string(it.value()) << "' />\n" ;
                    ++it ;
                }

                for( uint j=0 ; j<coords.size() ; j++ ) {
                    strm << "<nd ref='" << count + j << "'/>\n" ;
                }

                strm << "</way>\n" ;

                count += coords.size() ;
            }
            if ( way.coords_.size() > 1 ) {

            }

        }
    }




    strm << "</osm>" ;
}



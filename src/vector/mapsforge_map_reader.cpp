#include "mapsforge_map_reader.hpp"
#include "geom_helpers.hpp"

#include <fstream>
#include <iomanip>
#include <cstring>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>

using namespace std ;


enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul
};

static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order =
{ { 0, 1, 2, 3 } };

static const bool platform_is_little_endian = ( o32_host_order.value == 0x03020100ul ) ;

static void byte_swap_32(uint32_t &data)
{
    union u {uint32_t v; uint8_t c[4];};
    u un, vn;
    un.v = data ;
    vn.c[0]=un.c[3];
    vn.c[1]=un.c[2];
    vn.c[2]=un.c[1];
    vn.c[3]=un.c[0];
    data = vn.v ;
}

static void byte_swap_64(uint64_t &data)
{
    union u {uint64_t v; uint8_t c[8];};
    u un, vn;
    un.v = data ;
    vn.c[0]=un.c[7];
    vn.c[1]=un.c[6];
    vn.c[2]=un.c[5];
    vn.c[3]=un.c[4];
    vn.c[4]=un.c[3];
    vn.c[5]=un.c[2];
    vn.c[6]=un.c[1];
    vn.c[7]=un.c[0];
    data = vn.v ;
}

static void byte_swap_16(uint16_t &nValue)
{
    nValue = ((( nValue>> 8)) | (nValue << 8));
}


namespace mapsforge {

class ReadException: public runtime_error {
public:
    ReadException(const string &file, const string &reason):
        runtime_error("Error reading map file " + file + ": " + reason) {}
};

class FileTruncatedException: public ReadException {
public:
    FileTruncatedException(const std::string &map_file): ReadException(map_file, "File truncated") {}
};

uint32_t MapFile::read_uint32()
{
    uint32_t val ;
    if ( !strm_.read((char *)&val, 4) )
        throw FileTruncatedException(map_file_path_) ;

    if ( platform_is_little_endian )
        byte_swap_32(val) ;

    return val ;
}

int32_t MapFile::read_int32() {
    return read_uint32() ;
}


uint64_t MapFile::read_uint64()
{
    uint64_t val ;

    if ( !strm_.read((char *)&val, 8) )
        throw FileTruncatedException(map_file_path_) ;

    if ( platform_is_little_endian )
        byte_swap_64(val) ;

    return val ;
}

int64_t MapFile::read_int64() {
    return read_uint64() ;
}

uint16_t MapFile::read_uint16()
{
    uint16_t val ;
    if ( !strm_.read((char *)&val, 2) )
        throw FileTruncatedException(map_file_path_) ;

    if ( platform_is_little_endian )
        byte_swap_16(val) ;

    return val ;
}

int16_t MapFile::read_int16() {
    return read_uint16() ;
}

uint8_t MapFile::read_uint8()
{
    uint8_t val ;
    if ( !strm_.read((char *)&val, 1) )
        throw FileTruncatedException(map_file_path_) ;

    return val ;
}

int8_t MapFile::read_int8() {
    return read_uint8() ;
}

uint64_t MapFile::read_var_uint64()
{
    uint64_t val = UINT64_C(0);
    unsigned int shift = 0;
    uint8_t byte ;

    while ( 1 )
    {
        if ( !strm_.get((char &)byte) ) throw FileTruncatedException(map_file_path_) ;

        val |= ( byte & 0x7F ) << shift ;
        shift += 7 ;
        if ( ( byte & 0x80 ) == 0 ) break ;

        if ( shift > 63 )
            throw ReadException(map_file_path_, "Variable length integer is too long") ;
    }

    return val ;
}
#if 0

int64_t
sleb_extract(unsigned char *data, uint64_t *dotp)
{
    uint64_t	dot = *dotp;
    int64_t		res = 0;
    int		more = 1;
    int		shift = 0;
    int		val;

    data += dot;

    while (more) {
        /*
         * Pull off lower 7 bits
         */
        val = (*data) & 0x7f;

        /*
         * Add prepend value to head of number.
         */
        res = res | (val << shift);

        /*
         * Increment shift & dot pointer
         */
        shift += 7;
        dot++;

        /*
         * Check to see if hi bit is set - if not, this
         * is the last byte.
         */
        more = ((*data++) & 0x80) >> 7;
    }
    *dotp = dot;

    /*
     * Make sure value is properly sign extended.
     */
    res = (res << (64 - shift)) >> (64 - shift);

    return (res);
#endif
    int64_t MapFile::read_var_int64()
    {
        int64_t val = INT64_C(0);

        unsigned int shift = 0;
        uint8_t byte ;

        while (1)
        {
            if ( !strm_.get((char &)byte) ) throw FileTruncatedException(map_file_path_) ;

            if ( ( byte & 0x80) == 0 ) break ;

            val |= ( byte & 0x7F ) << shift;

            shift += 7 ;
        }

        if ( ( byte & 0x40 ) != 0 )
            val = -(val | ((byte & 0x3f) << shift)) ;
        else
            val |= ( byte & 0x3f ) << shift ;


        return val ;
    }


    int64_t MapFile::read_offset() {
        char buffer[5] ;

        if ( !strm_.read(buffer, 5) )
            throw FileTruncatedException(map_file_path_) ;

        int64_t v = (buffer[0] & 0xffL) << 32 | (buffer[1] & 0xffL) << 24 | (buffer[2] & 0xffL) << 16 | (buffer[3] & 0xffL) << 8 | (buffer[4] & 0xffL) ;

        return v ;
    }

    std::string MapFile::read_utf8()
    {
        string str ;
        uint64_t len = read_var_uint64() ;
        str.resize(len) ;
        if ( !strm_.read(&str[0], len) )
            throw FileTruncatedException(map_file_path_) ;

        return str ;
    }

    void MapFile::open(const std::string &file_path)
    {
        strm_.open(file_path.c_str(), ios::binary) ;

        if ( !strm_ ) throw ReadException(file_path, "error openning file") ;

        readHeader() ;
        readTileIndex() ;
    }

#if 0
    QueryParameters MapDatabase::calculateBlocks(const TileId &tile, const BoundingBox &bbox, int zoomLevel, int physicalZoomLevel)
    {
        qint64 fromPhysicalTileX;
        qint64 fromPhysicalTileY;
        qint64 toPhysicalTileX;
        qint64 toPhysicalTileY;
        bool useTileBitmask = false;

        if (tile.zoomLevel < physicalZoomLevel) {
            // calculate the XY numbers of the upper left and lower right sub-tiles
            const int zoomLevelDifference = physicalZoomLevel - tile.zoomLevel;
            fromPhysicalTileX = tile.tileX << zoomLevelDifference;
            fromPhysicalTileY = tile.tileY << zoomLevelDifference;
            toPhysicalTileX = fromPhysicalTileX + (1 << zoomLevelDifference) - 1;
            toPhysicalTileY = fromPhysicalTileY + (1 << zoomLevelDifference) - 1;
        } else if (tile.zoomLevel > physicalZoomLevel) {
            // calculate the XY numbers of the parent base tile
            const int zoomLevelDifference = tile.zoomLevel - physicalZoomLevel;
            fromPhysicalTileX = tile.tileX >> zoomLevelDifference;
            fromPhysicalTileY = tile.tileY >> zoomLevelDifference;
            toPhysicalTileX = fromPhysicalTileX;
            toPhysicalTileY = fromPhysicalTileY;
            useTileBitmask = true;
        } else {
            // use the tile XY numbers of the requested tile
            fromPhysicalTileX = tile.tileX;
            fromPhysicalTileY = tile.tileY;
            toPhysicalTileX = fromPhysicalTileX;
            toPhysicalTileY = fromPhysicalTileY;
        }

        // calculate the blocks in the file which need to be read
        const qint64 fromBlockX = qMax<qint64>(fromPhysicalTileX - MercatorProjection::boundaryTileLeft(bbox, physicalZoomLevel), 0L);
        const qint64 fromBlockY = qMax<qint64>(fromPhysicalTileY - MercatorProjection::boundaryTileTop(bbox, physicalZoomLevel), 0L);
        const qint64 toBlockX = qMin<qint64>(toPhysicalTileX, MercatorProjection::boundaryTileRight(bbox, physicalZoomLevel)) - MercatorProjection::boundaryTileLeft(bbox, physicalZoomLevel) + 1;
        const qint64 toBlockY = qMin<qint64>(toPhysicalTileY, MercatorProjection::boundaryTileBottom(bbox, physicalZoomLevel)) - MercatorProjection::boundaryTileTop(bbox, physicalZoomLevel) + 1;

        if (useTileBitmask) {
            return QueryParameters(zoomLevel, fromBlockX, fromBlockY, toBlockX, toBlockY, tile, tile.zoomLevel - physicalZoomLevel);
        }

        return QueryParameters(zoomLevel, fromBlockX, fromBlockY, toBlockX, toBlockY);
    }
#endif

    VectorTile MapFile::readTile(int32_t tx, int32_t ty, uint8_t tz)
    {
        // crop zoom level to map zoom interval

        uint8_t zoom ;
        zoom = std::min(info_.max_zoom_level_, tz) ;
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

        int32_t base_tile_min_x, base_tile_min_y;
        int32_t base_tile_max_x, base_tile_max_y;

        bool use_bitmask = false;

        if ( tz < si.base_zoom_ ) {
            // calculate the XY numbers of the upper left and lower right sub-tiles
            const int zoom_diff = (int8_t)si.base_zoom_ - (int8_t)tz ;
            base_tile_min_x = tx << zoom_diff ;
            base_tile_min_y = ty << zoom_diff ;
            base_tile_max_x = base_tile_min_x + ( 1 << zoom_diff ) - 1 ;
            base_tile_max_y = base_tile_min_y + ( 1 << zoom_diff ) - 1 ;
        } else if ( tz > si.base_zoom_ ) {
            // calculate the XY numbers of the parent base tile
            const int zoom_diff = (int8_t)tz - (int8_t)si.base_zoom_  ;
            base_tile_min_x = tx >> zoom_diff ;
            base_tile_min_y = ty >> zoom_diff ;
            base_tile_max_x = base_tile_min_x ;
            base_tile_max_y = base_tile_min_y  ;

            use_bitmask = true;
        } else {
            base_tile_min_x = tx ;
            base_tile_min_y = ty ;
            base_tile_max_x = base_tile_min_x ;
            base_tile_max_y = base_tile_min_y  ;
        }

        // load required base tile from file or cache

        for( uint bty = base_tile_min_y ; bty<= base_tile_max_y ; bty++ )
            for( uint btx = base_tile_min_x ; btx<= base_tile_max_x ; btx++ )
            {
                int row = bty - si.min_ty_ ;
                int col = btx - si.min_tx_ ;

                if ( row < 0 || col < 0 ) continue ;

                uint64_t idx = cols * row + col ; // this is the index to tile index array

                int64_t tile_offset = si.index_[idx] ;

                bool is_sea_tile = ( tile_offset & 0x8000000000LL ) != 0 ;

                TileData data(btx, bty, si.base_zoom_, is_sea_tile) ;

                if ( !is_sea_tile ) {
                    tile_offset = tile_offset & 0x7FFFFFFFFFLL ;
                    readTileData(si, tile_offset, data) ;
                    exportTileDataOSM(data, 0, "/tmp/oo.osm") ;
                }



            }
    }

    void MapFile::readHeader()
    {
        // read the magic bytes

        char header[20] ;
        if ( !strm_.read(header, 20) || strncmp(header, "mapsforge binary OSM", 20) != 0 )
            throw ReadException(map_file_path_, "Invalid or corrupted header") ;

        // read map info

        uint32_t header_length = read_uint32() ;

        readMapInfo() ;

        readTagList(poi_tags_) ;
        readTagList(way_tags_) ;

        readSubFileInfo() ;
    }

    void MapFile::readMapInfo()
    {
        info_.version_ = read_uint32() ;
        info_.file_size_ = read_uint64() ;
        info_.date_ = read_uint64() ;

        // bounding box

        info_.min_lat_ = read_int32() / 1.0e6 ;
        info_.min_lon_ = read_int32() / 1.0e6 ;
        info_.max_lat_ = read_int32() / 1.0e6 ;
        info_.max_lon_ = read_int32() / 1.0e6 ;

        info_.tile_sz_ = read_int16() ;

        if ( info_.tile_sz_ <= 0 )
            throw ReadException(map_file_path_, str(boost::format("Invalid tile size (%d px)") % info_.tile_sz_)) ;

        info_.projection_ = read_utf8() ;
        info_.flags_ = read_uint8() ;

        has_debug_info_ = info_.flags_ & 0x80 ;

        if ( info_.flags_ & 0x40 ) {
            info_.start_lat_ = read_int32() / 1.0e6 ;
            info_.start_lon_ = read_int32() / 1.0e6;
        }

        if ( info_.flags_ & 0x20 )
            info_.start_zoom_level_ = read_uint8() ;
        else
            info_.start_zoom_level_ = 10 ;

        if ( info_.flags_ & 0x10 )
            info_.lang_preference_ = read_utf8() ;
        else
            info_.lang_preference_ = "en" ;

        if ( info_.flags_ & 0x08 )
            info_.comment_ = read_utf8() ;

        if ( info_.flags_ & 0x04 )
            info_.created_by_ = read_utf8() ;
    }

    void MapFile::readTagList(std::vector<string> &tags)
    {
        uint16_t n_tags = read_uint16() ;

        for( uint i=0 ; i<n_tags ; i++ ) {
            tags.push_back(read_utf8()) ;
        }
    }

    void MapFile::readSubFileInfo()
    {
        uint8_t num_zoom_intervals = read_uint8() ;

        sub_files_.resize(num_zoom_intervals) ;

        info_.min_zoom_level_ = std::numeric_limits<uint8_t>::max();
        info_.max_zoom_level_ = std::numeric_limits<uint8_t>::min();

        for( uint i=0 ; i<num_zoom_intervals ; i++ ) {
            SubFileInfo &info = sub_files_[i] ;

            info.base_zoom_ = read_uint8() ;
            info.min_zoom_ = read_uint8() ;
            info.max_zoom_ = read_uint8() ;
            info.offset_ = read_uint64() ;
            info.size_ = read_uint64() ;

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

    void MapFile::readTileIndex()
    {
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
                int64_t offset = read_offset() ;
                info.index_.push_back(offset) ;
            }
        }

    }

    void MapFile::readTiles()
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

                TileData data(tx, ty, info.base_zoom_, is_sea_tile) ;

                if ( !is_sea_tile ) {
                    tile_offset = tile_offset & 0x7FFFFFFFFFLL ;
                    readTileData(info, tile_offset, data) ;
                }

            }

        }

    }

    void MapFile::readTileData(const SubFileInfo &info, int64_t offset, TileData &data) {

        strm_.seekg(info.offset_ + offset) ;

        if ( has_debug_info_ ) {
            char signature[32] ;
            strm_.read(signature, 32) ;
        }

        // read zoom table

        int nz = info.max_zoom_ - info.min_zoom_ + 1 ;

        data.pois_per_level_.resize(nz) ;
        data.ways_per_level_.resize(nz) ;

        vector<uint64_t> num_pois_per_level, num_ways_per_level ;
        uint64_t total_pois = 0, total_ways = 0 ;

        for ( uint i=0 ; i<nz ; i++ ) {
            uint64_t n_pois = read_var_uint64() ;
            uint64_t n_ways = read_var_uint64() ;

            total_pois += n_pois ;
            total_ways += n_ways ;

            num_pois_per_level.push_back(n_pois) ;
            num_ways_per_level.push_back(n_ways) ;
        }

        // get the relative offset to the first stored way in the block

        uint64_t first_way_offset = read_var_uint64() ;

        // convert to absolute position

        uint64_t first_way_pos = (uint64_t)strm_.tellg() + first_way_offset ;

        // read pois

        double min_lat, min_lon, max_lat, max_lon ;
        tms::tileLatLonBounds(data.x_, (1 << data.z_)-data.y_ - 1, data.z_, min_lat, min_lon, max_lat, max_lon) ;

        for(uint i=0 ; i<nz ; i++) {
            uint64_t n = num_pois_per_level[i] ;

            data.pois_per_level_[i].resize(n) ;

            for( uint j=0 ; j<n ; j++ )
                readPOI(data.pois_per_level_[i][j], max_lat, min_lon) ;
        }

        // read ways

        strm_.seekg(first_way_pos) ;

        for(uint i=0 ; i<nz ; i++) {
            uint64_t n = num_ways_per_level[i] ;


            for( uint j=0 ; j<n ; j++ )
            {
                vector<Way> ways ;
                readWays(ways, max_lat, min_lon) ; // we break ways split into multiple blocks into separate ways

                for(uint k=0 ; k<ways.size() ; k++ )
                    data.ways_per_level_[i].push_back(std::move(ways[k])) ;
            }

        }


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

    void MapFile::readPOI(POI &poi, float lat_orig, float lon_orig) {

        if ( has_debug_info_ ) {
            char signature[32] ;
            strm_.read(signature, 32) ;
        }

        double lat_diff = (double)read_var_int64()/1.0e6 ;
        double lon_diff = (double)read_var_int64()/1.0e6 ;
        poi.lat_ = lat_orig + lat_diff ;
        poi.lon_ = lon_orig + lon_diff ;

        uint8_t tflag = read_uint8() ;

        int8_t layer = int((tflag & 0xf0) >> 4) - 5 ;
        uint8_t ntags = tflag & 0x0f ;

        for( uint i=0 ; i<ntags ; i++ ) {
            uint64_t tag_index = read_var_uint64() ;

            string kv = poi_tags_[tag_index], tag, val ;
            decode_key_value(kv, tag, val);

            poi.tags_.add(tag, val) ;
        }

        uint8_t cflag = read_uint8() ;

        if ( cflag & 0x80 )
            poi.tags_.add("name", read_utf8()) ;

        if ( cflag & 0x40 )
            poi.tags_.add("addr:housenumber", read_utf8()) ;

        if ( cflag & 0x20 )
            poi.tags_.add("ele", std::to_string(read_var_int64())) ;

        if ( layer != 0 )
            poi.tags_.add("layer", std::to_string(layer)) ;
    }

    void MapFile::readWays(vector<Way> &ways, float lat_orig, float lon_orig) {

        if ( has_debug_info_ ) {
            char signature[32] ;
            strm_.read(signature, 32) ;
        }

        uint64_t data_sz = read_var_uint64() ;
        uint16_t tile_bitmap = read_uint16() ;

        uint8_t tflag = read_uint8() ;

        int8_t layer = int((tflag & 0xf0) >> 4) - 5 ;
        uint8_t ntags = tflag & 0x0f ;

        Dictionary tags ;

        for( uint i=0 ; i<ntags ; i++ ) {
            uint64_t tag_index = read_var_uint64() ;

            string kv = way_tags_[tag_index], tag, val ;
            decode_key_value(kv, tag, val);

            tags.add(tag, val) ;
        }

        uint8_t cflag = read_uint8() ;

        if ( cflag & 0x80 )
            tags.add("name", read_utf8()) ;

        if ( cflag & 0x40 )
            tags.add("addr:housenumber", read_utf8()) ;

        if ( cflag & 0x20 )
            tags.add("ref", read_utf8()) ;

        double label_position_lat_diff = 0, label_position_lon_diff = 0 ;

        if ( cflag & 0x10 ) {
            label_position_lat_diff = read_var_int64()/1.0e6 ;
            label_position_lon_diff = read_var_int64()/1.0e6 ;
        }

        if ( layer != 0 )
            tags.add("layer", std::to_string(layer)) ;

        uint64_t n_data_blocks = 1 ;

        if ( cflag & 0x08 )
            n_data_blocks = read_var_uint64() ;

        ways.resize(n_data_blocks) ;

        for(uint i=0 ; i<n_data_blocks ; i++) {
            Way way ;
            way.tags_ = tags ;

            uint64_t n_way_coord_blocks = read_var_uint64() ;

            way.coords_.resize(n_way_coord_blocks) ;

            for(uint j=0 ; j<n_way_coord_blocks ; j++ ) {
                uint64_t num_way_nodes = read_var_uint64() ;

                way.coords_[j].resize(num_way_nodes) ;

                if ( cflag & 0x04 ) // double delta encoding
                    readWayNodesDoubleDelta(way.coords_[j], lat_orig, lon_orig);
                else
                    readWayNodesSingleDelta(way.coords_[j], lat_orig, lon_orig);
            }


            ways[i] = std::move(way) ;
        }

    }

    void MapFile::readWayNodesDoubleDelta(std::vector<LatLon> &coord_list, double tx0, double ty0)
    {
        double lat = read_var_int64()/1.0e6 + tx0 ;
        double lon = read_var_int64()/1.0e6 + ty0 ;

        coord_list[0] = LatLon{lat, lon} ;

        float previous_delta_lat = 0, previous_delta_lon = 0 ;

        for( uint i=1 ; i<coord_list.size() ; i++ ) {

            double delta_lat = previous_delta_lat + read_var_int64()/1.0e6 ;
            double delta_lon = previous_delta_lon + read_var_int64()/1.0e6 ;

            lat += delta_lat ;
            lon += delta_lon ;

            previous_delta_lat = delta_lat ;
            previous_delta_lon = delta_lon ;

            coord_list[i] = LatLon{lat, lon} ;
        }
    }

    void MapFile::readWayNodesSingleDelta(std::vector<LatLon> &coord_list, double tx0, double ty0)
    {
        double lat = read_var_int64()/1.0e6 + tx0 ;
        double lon = read_var_int64()/1.0e6 + ty0 ;

        coord_list[0] = LatLon{lat, lon} ;

        for(uint i=1 ; i<coord_list.size() ; i++ ) {

            lat += read_var_int64()/1.0e6 ;
            lon += read_var_int64()/1.0e6 ;

            coord_list[i] = LatLon{lat, lon} ;
        }
    }

    void MapFile::exportTileDataOSM(const MapFile::TileData &data, uint8_t z, const string &filename)
    {
        ofstream strm(filename.c_str()) ;

        strm << "<?xml version='1.0' encoding='UTF-8'?>\n" ;
        strm << "<osm version='0.6' generator='JOSM'>\n" ;

        int64_t count = -10000000 ;


            for(int i=0 ; i<data.pois_per_level_[z].size() ; i++ )
            {
                const POI &poi = data.pois_per_level_[z][i] ;

                strm << "<node id='" << count++ << "' visible='true' lat='" << setprecision(12) << poi.lat_ <<
                        "' lon='" << setprecision(12) << poi.lon_  ;

                if ( poi.tags_.empty() ) strm <<  "' />\n" ;
                else
                {
                    strm << "' >\n" ;

                    DictionaryIterator it(poi.tags_) ;

                    while ( it ) {
                        strm << "<tag k='" << it.key() << "' v='" << it.value() << "' />\n" ;
                        ++it ;
                    }

                    strm << "</node>\n" ;
                }
            }

            int64_t wcount = -20000000 ;

            int64_t count0 = count ;

            for(int i=0 ; i<data.ways_per_level_[z].size() ; i++ )
            {
                const Way &way = data.ways_per_level_[z][i] ;

                if ( way.coords_.size() == 1 ) {

                        const vector<LatLon> &coords = way.coords_[0] ;

                        for( uint j=0 ; j<coords.size() ; j++ ) {
                            strm << "<node id='" << count + j << "' visible='true' lat='" << setprecision(12) << coords[j].lat_ <<
                                    "' lon='" << setprecision(12) << coords[j].lon_  <<  "' />\n" ;
                        }

                        count += coords.size() ;
                }

            }

            count = count0 ;

            for(int i=0 ; i<data.ways_per_level_[z].size() ; i++ )
            {
                const Way &way = data.ways_per_level_[z][i] ;

                if ( way.coords_.size() == 1 ) {

                    const vector<LatLon> &coords = way.coords_[0] ;

                    strm << "<way id='" << -100000000 + wcount++ << "' action='modify' visible='true'>\n" ;

                    DictionaryIterator it(way.tags_) ;

                    while ( it ) {
                        strm << "<tag k='" << it.key() << "' v='" << it.value() << "' />\n" ;
                        ++it ;
                    }

                    for( uint j=0 ; j<coords.size() ; j++ ) {
                        strm << "<nd ref='" << count + j << "'/>\n" ;
                    }

                    strm << "</way>\n" ;

                    count += coords.size() ;
                }

            }




        strm << "</osm>" ;
    }

}

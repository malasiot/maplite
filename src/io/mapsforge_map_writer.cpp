#include "mapsforge_map_writer.hpp"
#include "serializer.hpp"
#include "database.hpp"
#include "tms.hpp"
#include "tile_key.hpp"
#include "progress_stream.hpp"
#include "osm_processor.hpp"

#include <string>
#include <stdexcept>
#include <iostream>
#include <ctime>

#include <spatialite.h>
#include <omp.h>

using namespace std ;



static void sort_histogram(const map<string, uint64_t> &hist, vector<string> &tags) {
    vector<uint64_t> freq ;

    for( const auto &p: hist ) {
        tags.push_back(p.first) ;
        freq.push_back(p.second) ;
    }

    std::sort(tags.begin(), tags.end(),
              [hist](const string &a, const string &b) -> bool  {
        uint64_t fa = hist.find(a)->second ;
        uint64_t fb = hist.find(b)->second ;
        return fa > fb;
    }) ;
}

static bool get_poi_tags(OSMProcessor &db, std::vector<string> &pois, map<string, uint32_t> &tag_mapping) {
    map<string, uint64_t> tag_hist ;

    db.forAllPOITags([&](const string &tag, const string &val) {
        if ( tag == "name" || tag == "addr:housenumber" || tag == "ele" ) return ;
        auto it = tag_hist.find(tag) ;
        if ( it == tag_hist.end() )
            tag_hist.insert(std::make_pair(tag + '=' + val, 1)) ;
        else
            it->second ++ ;
    }) ;

    sort_histogram(tag_hist, pois) ;

    for( uint i=0 ; i<pois.size() ; i++ )
        tag_mapping.emplace(std::make_pair(pois[i], i)) ;

    return true ;
}

static bool get_way_tags(OSMProcessor &db, std::vector<string> &ways, map<string, uint32_t> &tag_mapping) {
    map<string, uint64_t> tag_hist ;

    db.forAllWayTags([&](const string &tag, const string &val) {
        if ( tag == "name" || tag == "addr:housenumber" || tag == "ref" ) return ;
        auto it = tag_hist.find(tag) ;
        if ( it == tag_hist.end() )
            tag_hist.insert(std::make_pair(tag + '=' + val, 1)) ;
        else
            it->second ++ ;
    }) ;

    sort_histogram(tag_hist, ways) ;

    for( uint i=0 ; i<ways.size() ; i++ )
        tag_mapping.emplace(std::make_pair(ways[i], i)) ;

    return true ;
}


void MapFileWriter::setBoundingBox(const BBox &box) {
    info_.min_lat_ = box.miny_ ;
    info_.min_lon_ = box.minx_ ;
    info_.max_lat_ = box.maxy_ ;
    info_.max_lon_ = box.maxx_ ;
}



void MapFileWriter::setStartPosition(float lat, float lon) {
    info_.start_lat_ = lat ;
    info_.start_lon_ = lon ;
    info_.flags_ |= 0x40 ;
}

void MapFileWriter::setStartZoom(uint8_t zoom) {
    info_.start_zoom_level_ = zoom ;
    info_.flags_ |= 0x20 ;
}

void MapFileWriter::setPreferredLanguages(const std::string &langs) {
    info_.lang_preference_ = langs ;
    info_.flags_ |= 0x10 ;
}

void MapFileWriter::setComment(const std::string &comment) {
    info_.comment_ = comment ;
    info_.flags_ |= 0x08 ;
}

void MapFileWriter::setCreator(const std::string &creator) {
    info_.created_by_ = creator ;
    info_.flags_ |= 0x04 ;
}

void MapFileWriter::setDebug(bool debug) {
    has_debug_info_ = debug ;
    if ( debug ) info_.flags_ |= 0x80 ;
}

void MapFileWriter::create(const std::string &file_path) {

    strm_.open(file_path.c_str(),  ios::out | ios::binary) ;

    if ( !strm_ ) throw std::runtime_error("error in file creation") ;

    // initialize info

    info_.version_ = 4 ;
    info_.date_ = time(0) * 1000 ;
    info_.file_size_ = 0 ;
    info_.tile_sz_ = 256 ;
    info_.projection_ = "Mercator" ;
}

void MapFileWriter::write(OSMProcessor &db, WriteOptions &options) {

    setDebug(options.debug_);
    writeHeader(db, options) ;
    writeSubFiles(db, options) ;

    // write file size
    info_.file_size_ = strm_.tellg() ;
    strm_.seekg(28) ;

    MapFileOSerializer s(strm_) ;
    s.write_uint64(info_.file_size_) ;
}



void MapFileWriter::writeHeader(OSMProcessor &db, WriteOptions &options)
{
    MapFileOSerializer so(strm_) ;

    // write the magic bytes

    const char *header = "mapsforge binary OSM" ;
    so.write_bytes((uint8_t *)header, 20) ;
    so.write_uint32(0) ; // header length unknown so far

    writeMapInfo() ;

    // get frequency sorted POI and way tags from the database
    get_poi_tags(db, poi_tags_, poi_tag_mapping_) ;
    get_way_tags(db, way_tags_, way_tag_mapping_) ;

    writeTagList(poi_tags_) ;
    writeTagList(way_tags_) ;

    writeSubFileInfo(options) ;

    // now we finished with the header write header length
    int32_t cp = strm_.tellg() ;
    strm_.seekg(20) ;
    info_.header_size_ = cp - 20 ;
    so.write_uint32(info_.header_size_) ;
    strm_.seekg(cp) ;
}

void MapFileWriter::writeMapInfo() {
    MapFileOSerializer s(strm_) ;

    s.write_uint32(info_.version_) ;
    s.write_uint64(info_.file_size_) ;
    s.write_uint64(info_.date_) ;

    s.write_int32(round(info_.min_lat_ * 1.0e6)) ;
    s.write_int32(round(info_.min_lon_ * 1.0e6)) ;
    s.write_int32(round(info_.max_lat_ * 1.0e6)) ;
    s.write_int32(round(info_.max_lon_ * 1.0e6)) ;

    s.write_int16(info_.tile_sz_) ;
    s.write_utf8(info_.projection_) ;
    s.write_uint8(info_.flags_) ;

    has_debug_info_ = info_.flags_ & 0x80 ;

    if ( info_.flags_ & 0x40 ) {
        s.write_int32(round(info_.start_lat_ * 1.0e6) ) ;
        s.write_int32(round(info_.start_lon_ * 1.0e6) ) ;
    }

    if ( info_.flags_ & 0x20 )
        s.write_uint8(info_.start_zoom_level_) ;

    if ( info_.flags_ & 0x10 )
        s.write_utf8(info_.lang_preference_) ;

    if ( info_.flags_ & 0x08 )
        s.write_utf8(info_.comment_) ;

    if ( info_.flags_ & 0x04 )
        s.write_utf8(info_.created_by_) ;

}

void MapFileWriter::writeTagList(const vector<string> &tags)
{
    MapFileOSerializer s(strm_) ;

    s.write_uint16(tags.size()) ;

    for( const auto &tag: tags)
        s.write_utf8(tag) ;
}


void MapFileWriter::writeSubFileInfo(const WriteOptions &options)
{

    MapFileOSerializer s(strm_) ;

    // create subfile structure from configuration string

    info_.min_zoom_level_ = std::numeric_limits<uint8_t>::max();
    info_.max_zoom_level_ = std::numeric_limits<uint8_t>::min();

    const vector<uint8_t> &conf = options.zoom_interval_conf_ ;

    for( uint i=0 ; i<conf.size() ; ) {
        SubFileInfo info ;
        info.base_zoom_ = conf[i++] ;
        info.min_zoom_ = conf[i++] ;
        info.max_zoom_ = conf[i++] ;

        info_.min_zoom_level_ = std::min(info_.min_zoom_level_, info.min_zoom_) ;
        info_.max_zoom_level_ = std::max(info_.max_zoom_level_, info.max_zoom_) ;

        int32_t min_ty, max_ty ;

        tms::tilesWithinBounds(info_.min_lat_, info_.min_lon_, info_.max_lat_, info_.max_lon_, info.base_zoom_,
                               info.min_tx_, min_ty, info.max_tx_, max_ty) ;

        // comvert to Google tile coordinates
        info.max_ty_ = (1 << info.base_zoom_) - min_ty - 1 ;
        info.min_ty_ = (1 << info.base_zoom_) - max_ty - 1 ;

        sub_files_.push_back(info) ;
    }

    s.write_uint8(sub_files_.size()) ;

    for( SubFileInfo &info: sub_files_ ) {
        s.write_uint8(info.base_zoom_) ;
        s.write_uint8(info.min_zoom_) ;
        s.write_uint8(info.max_zoom_) ;

        s.write_uint64(info.offset_) ;
        s.write_uint64(info.size_) ;
    }
}

ConsoleProgressPrinter g_prog ;

static bool check_is_sea( const vector<POIData> &pois, const vector<WayDataContainer> &ways ) {
    if ( !pois.empty() ) return false ;
    if ( ways.empty() ) return false ;
    if ( ways.size() > 1 ) return false ;
    if ( ways[0].tags_.get("natural") == "sea" ) return true ;
    return false ;
}

void MapFileWriter::writeSubFiles(OSMProcessor &db, const WriteOptions &options)
{
    MapFileOSerializer s(strm_) ;

    uint64_t sf_table_pos = (int64_t)strm_.tellg() - 19 * sub_files_.size() ;

    uint count = 0 ;
    for( SubFileInfo &info: sub_files_ ) {

        info.offset_ = strm_.tellg() ;
        uint64_t extra = 0  ;

        if ( has_debug_info_ ) {
            const char *signature = "+++IndexStart+++" ;
            s.write_bytes((uint8_t *)signature, 16) ;
            extra = 16 ;
        }

        // determine tiles covered by the bounding box on the base zoom level

        uint32_t rows = info.max_ty_ - info.min_ty_ + 1 ;
        uint32_t cols = info.max_tx_ - info.min_tx_ + 1 ;
        uint64_t tile_count = rows * cols ;

        stringstream msg ;
        msg << "writing sub-file at level " << count << ": " << tile_count << " tiles" ;
        g_prog.beginTask(msg.str(), tile_count) ;

        info.index_.resize(tile_count) ;
        uint64_t sz = extra + 5 * tile_count ;

        // the tile index table contains tile_count * 5 bytes ( we will fill this later )

        strm_.seekg(tile_count * 5, ios::cur) ;

        uint64_t current_pos = sz ;

        const uint chunk_size = 512 ;
        uint chunk_left = chunk_size ;

        for( uint j=0 ; j<tile_count ; j+= chunk_size ) {

            if (j + chunk_size > tile_count ) chunk_left = tile_count - j ;

            g_prog.advance(j+1);

            vector<string> tile_data(chunk_left) ;
            vector<bool> tile_is_sea(chunk_left) ;

//#pragma omp parallel for
            for( uint k = j ; k< j + chunk_left ; k++ ) {

                int32_t row = k / cols ;
                int32_t col = k % cols ;

                int32_t tx = col + info.min_tx_ ;
                int32_t ty = row + info.min_ty_ ;
                int32_t tz = info.base_zoom_ ;

                vector<POIData> pois ;
                vector<vector<uint32_t>> pois_per_level ;
                vector<WayDataContainer> ways ;
                vector<vector<uint32_t>> ways_per_level ;

                fetchTileData(tx, ty, tz, info.min_zoom_, info.max_zoom_, db, options, pois, pois_per_level, ways, ways_per_level ) ;

                bool is_sea = check_is_sea(pois, ways) ;
                tile_is_sea[k-j] = is_sea ;

                string bytes = writeTileData(tx, ty, tz, options, pois, pois_per_level, ways, ways_per_level) ;
                tile_data[k-j] = bytes ;
            }

            // serial write to file
            for( uint k = j ; k< j + chunk_left ; k++ ) {
                bool is_sea = tile_is_sea[k-j] ;
                info.index_[k] = (is_sea) ? current_pos : ( current_pos | 0x8000000000LL ) ;
                const string &bytes = tile_data[k-j] ;
                s.write_bytes(bytes) ;
                current_pos += bytes.size() ;
                sz += bytes.size() ;

            }

        }

        info.size_ = sz ;

        // fill-in offset and size information in subfile configuration table

        strm_.seekg(sf_table_pos + 3 + 19*count++) ;
        s.write_uint64(info.offset_) ;
        s.write_uint64(info.size_) ;

        // write tile offset table
        strm_.seekg(info.offset_ + extra) ;
        for( uint j=0 ; j<tile_count ; j++ ) {
            s.write_offset(info.index_[j]) ;
        }
        // seek to end of subfile
        strm_.seekg(info.offset_ + info.size_) ;
    }
}


static string make_bbox_query(const std::string &tableName, const BBox &bbox, int min_zoom,
                              int max_zoom, bool clip, double buffer, double tol, bool centroid)
{
    stringstream sql ;

    sql.precision(16) ;

    sql << "SELECT osm_id, kv.key, kv.val, kv.zoom_min, kv.zoom_max, " ;

    if ( tol != 0.0 )
        sql << "SimplifyPreserveTopology(" ;

    if ( clip ) {
        sql << "ST_ForceLHR(ST_Intersection(geom, ST_Transform(BuildMBR(" ;
        sql << bbox.minx_-buffer << ',' << bbox.miny_-buffer << ',' << bbox.maxx_+buffer << ',' << bbox.maxy_+buffer << "," << 3857 ;
        sql << "),4326)))" ;
    }
    else sql << "geom" ;

    if ( tol != 0 )
        sql << ", " << tol << ")" ;

    sql << " AS _geom_ " ;
    if ( centroid ) sql << ", ST_Centroid(geom) " ;
    sql << " FROM " << tableName << " AS g JOIN kv USING (osm_id, osm_type) ";

    sql << " WHERE " ;
    sql << "(( g.zmin BETWEEN " << (int)min_zoom << " AND " << max_zoom << " ) OR ( g.zmax BETWEEN " << min_zoom << " AND " << max_zoom << " ) OR ( g.zmin <= " << min_zoom << " AND g.zmax >= " << max_zoom << "))" ;
    sql << "AND _geom_ NOT NULL AND ST_IsValid(_geom_) " ;
    sql << "AND g.ROWID IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='" << tableName << "' AND search_frame = ST_Transform(BuildMBR(" ;
    sql << bbox.minx_-buffer << ',' << bbox.miny_-buffer << ',' << bbox.maxx_+buffer << ',' << bbox.maxy_+buffer << "," << 3857 << "),4326)) " ;

    return sql.str() ;
}

static bool fetch_pois(OSMProcessor &proc, const BBox &bbox, uint8_t min_zoom, uint8_t max_zoom,
                       vector<POIData> &pois, vector<vector<uint32_t>> &pois_per_level) {

    return proc.forAllGeometries("geom_pois", bbox, min_zoom, max_zoom, false, 0, 0, false,
                          [&](gaiaGeomCollPtr geom, osm_id_t id, osm_feature_t ft, uint8_t minz, uint8_t maxz, double, double) {

        for( gaiaPointPtr p = geom->FirstPoint ; p != nullptr ; p = p->Next ) {
            double lon = p->X ;
            double lat = p->Y ;

            pois.emplace_back(ILatLon(lat, lon), id) ;

            // we add the poi at the lowest possible level
            int z = std::max<int>(minz, min_zoom) - (int)min_zoom;
            pois_per_level[z].push_back(pois.size()-1) ;
        }

        proc.getTags(id, ft, min_zoom, max_zoom, pois.back().tags_) ;
    }) ;

}


static bool fetch_lines(OSMProcessor &proc, const BBox &bbox, uint8_t min_zoom, uint8_t max_zoom,
                        bool clip, double buffer, double tol,
                        vector<WayDataContainer> &ways, vector<vector<uint32_t>> &ways_per_level) {


    return proc.forAllGeometries("geom_lines", bbox, min_zoom, max_zoom, clip, buffer, tol, false,
                          [&](gaiaGeomCollPtr geom, osm_id_t id, osm_feature_t ft, uint8_t minz, uint8_t maxz, double, double) {

        // each line may be broken into multiple linestrings by clipping
        WayDataContainer wc ;

        // we conveniently also get the MBR of the geometry

        tms::latlonToMeters(geom->MinY, geom->MinX, wc.box_.minx_, wc.box_.miny_) ;
        tms::latlonToMeters(geom->MaxY, geom->MaxX, wc.box_.maxx_, wc.box_.maxy_) ;

        for( gaiaLinestringPtr p = geom->FirstLinestring ; p != nullptr ; p = p->Next ) {
            double *coords = p->Coords ;
            WayDataBlock block ;
            block.coords_.resize(1) ;

            for( uint i=0 ; i<p->Points ; i++ ) {
                double lon = *coords++ ;
                double lat = *coords++ ;
                block.coords_[0].emplace_back(lat, lon) ;
            }

            wc.blocks_.emplace_back(block) ;
        }

        wc.id_ = id ;
        ways.emplace_back(wc) ;

        // we add the way at the lowest possible level
        int z = std::max<int>(minz, min_zoom) - (int)min_zoom;
        ways_per_level[z].push_back(ways.size()-1) ;

        proc.getTags(id, ft, min_zoom, max_zoom, ways.back().tags_) ;
    }) ;

}

static bool fetch_polygons(OSMProcessor &proc, const BBox &bbox, uint8_t min_zoom, uint8_t max_zoom, bool clip,
                           double buffer, double tol, bool labels, vector<WayDataContainer> &ways, vector<vector<uint32_t>> &ways_per_level) {

    return proc.forAllGeometries("geom_polygons", bbox, min_zoom, max_zoom, clip, buffer, tol, labels,
                          [&](gaiaGeomCollPtr geom, osm_id_t id, osm_feature_t ft, uint8_t minz, uint8_t maxz, double clat, double clon) {
        // each polygon may be broken into multiple polygons by clipping
        WayDataContainer wc ;

        // we conveniently also get the MBR of the geometry

        tms::latlonToMeters(geom->MinY, geom->MinX, wc.box_.minx_, wc.box_.miny_) ;
        tms::latlonToMeters(geom->MaxY, geom->MaxX, wc.box_.maxx_, wc.box_.maxy_) ;

        for( gaiaPolygonPtr p = geom->FirstPolygon ; p != nullptr ; p = p->Next ) {

            uint n_interior_rings = p->NumInteriors ;

            WayDataBlock block ;
            block.coords_.resize(1 + n_interior_rings) ;

            gaiaRingPtr ex_ring = p->Exterior ;
            double *coords = ex_ring->Coords ;

            for( uint i=0 ; i<ex_ring->Points ; i++ ) {
                double lon = *coords++ ;
                double lat = *coords++ ;
                block.coords_[0].emplace_back(lat, lon) ;
            }

            for( uint k=0 ; k< n_interior_rings ; k++ ) {
                gaiaRing &ir_ring = p->Interiors[k] ;
                double *coords = ir_ring.Coords ;

                for( uint i=0 ; i<ir_ring.Points ; i++ ) {
                    double lon = *coords++ ;
                    double lat = *coords++ ;
                    block.coords_[k+1].emplace_back(lat, lon) ;
                }
            }

            wc.blocks_.emplace_back(block) ;
        }

        wc.id_ = id ;

        if ( labels )
            wc.label_pos_ = ILatLon(clat, clon) ;

        ways.emplace_back(wc) ;

        // we add the way at the lowest possible level
        int z = std::max<int>(minz, min_zoom) - (int)min_zoom;
        ways_per_level[z].push_back(ways.size()-1) ;


        proc.getTags(id, ft, min_zoom, max_zoom, ways.back().tags_) ;
    }) ;


}

// Computes the amount of latitude degrees for a given distance in pixel at a given zoom level.
static double deltaLat(double delta, double lat, uint8_t zoom) {
    double mx, my, dlat, lon, px, py ;
    tms::latlonToMeters(lat, 0, mx, my) ;
    tms::metersToPixels(mx, my, zoom, px, py) ;

    py += delta ;
    tms::pixelsToMeters(px, py, zoom, mx, my) ;
    tms::metersToLatLon(mx, my, dlat, lon) ;

    return fabs(dlat - lat) ;
}

void MapFileWriter::fetchTileData(int32_t tx, int32_t ty, int32_t tz, uint8_t min_zoom, uint8_t max_zoom, OSMProcessor &db, const WriteOptions &options,
                                  vector<POIData> &pois,
                                  vector<vector<uint32_t>> &pois_per_level,
                                  vector<WayDataContainer> &ways,
                                  vector<vector<uint32_t>> &ways_per_level)
{
    BBox bbox ;
    TileKey bt(tx, ty, tz, true) ;
    tms::tileBounds(bt.x(), bt.y(), bt.z(), bbox.minx_, bbox.miny_, bbox.maxx_, bbox.maxy_) ;
    int nz = (int)max_zoom - (int)min_zoom + 1 ;

    pois_per_level.resize(nz) ;
    ways_per_level.resize(nz) ;

    // we compute simplification factor per subfile
    double tol = ( tz <= 12 && options.simplification_factor_ > 0 ) ? deltaLat(options.simplification_factor_, info_.max_lat_, max_zoom) : 0 ;

    fetch_pois(db, bbox, min_zoom, max_zoom, pois, pois_per_level) ;
    fetch_lines(db, bbox, min_zoom, max_zoom, options.way_clipping_, options.bbox_enlargement_, tol, ways, ways_per_level) ;
    fetch_polygons(db, bbox, min_zoom, max_zoom, options.polygon_clipping_, options.bbox_enlargement_, tol, options.label_positions_, ways, ways_per_level) ;

    for( auto &way: ways) {
        computeSubTileMask(tx, ty, tz, way, options.bbox_enlargement_) ;
    }
}

std::string MapFileWriter::writeTileData(int32_t tx, int32_t ty, int32_t tz,
                                         const WriteOptions &options,
                                         const vector<POIData> &pois,
                                         const vector<vector<uint32_t>> &pois_per_level,
                                         const vector<WayDataContainer> &ways,
                                         const vector<vector<uint32_t>> &ways_per_level
                                         ) {

    ostringstream buffer ;

    TileKey bt(tx, ty, tz, true) ;

    double min_lat, min_lon, max_lat, max_lon ;
    tms::tileLatLonBounds(bt.x(), bt.y(), bt.z(), min_lat, min_lon, max_lat, max_lon) ;

    MapFileOSerializer s(buffer) ;

    // write header
    if ( options.debug_ ) {
        stringstream sigstrm ;
        sigstrm << "###TileStart" << tx << ',' << ty << "###" ;
        int extra = 32-sigstrm.tellp() ;
        for ( int i=0 ; i<extra ; i++ ) sigstrm.put(' ') ;
        s.write_bytes((uint8_t *)sigstrm.str().c_str(), 32) ;
    }

    // write POI and way number
    for ( uint i=0 ; i<pois_per_level.size() ; i++ ) {
        s.write_var_uint64(pois_per_level[i].size()) ;
        s.write_var_uint64(ways_per_level[i].size()) ;
    }

    // we write POI and way data into a memory buffer since we need to find the offsets

    string poi_data = writePOIData(pois, pois_per_level, ILatLon(max_lat, min_lon)) ;
    string way_data = writeWayData(ways, ways_per_level, ILatLon(max_lat, min_lon)) ;

    s.write_var_uint64(poi_data.size()) ;
    s.write_bytes((uint8_t *)&poi_data[0], poi_data.size()) ;
    s.write_bytes((uint8_t *)&way_data[0], way_data.size()) ;

    return buffer.str() ;

}

static int safe_stoi(const std::string &val, uint dv=0) {
    try {
        return stoi(val) ;
    }
    catch ( std::invalid_argument & ) {
        return dv ;
    }
}

string MapFileWriter::writePOIData(const vector<POIData> &pois, const vector<vector<uint32_t> > &pois_per_level, const ILatLon &orig)
{
    ostringstream strm ;
    MapFileOSerializer buffer(strm) ;

    uint nz = pois_per_level.size() ;

    for( uint i=0 ; i<pois_per_level.size() ; i++ ) {

        for( uint j=0 ; j<pois_per_level[i].size() ; j++ ) {
            uint32_t idx = pois_per_level[i][j] ;
            const POIData &poi = pois[idx] ;

            if ( has_debug_info_ ) {
                // write header
                stringstream sigstrm ;
                sigstrm << "***POIStart" << poi.id_ << "***" ;
                int extra = 32-sigstrm.tellp() ;
                for ( int i=0 ; i<extra ; i++ ) sigstrm.put(' ') ;
                buffer.write_bytes((uint8_t *)sigstrm.str().c_str(), 32) ;
            }

            buffer.write_var_int64(poi.coords_.lat_ - orig.lat_) ;
            buffer.write_var_int64(poi.coords_.lon_ - orig.lon_) ;

            vector<uint32_t> tags ;
            uint8_t cflag = 0 ;
            int8_t layer ;

            DictionaryIterator it(poi.tags_) ;

            while (it) {
                string key = it.key() ;
                string val = it.value() ;

                if ( key == "name" ) cflag |= 0x80 ;
                else if ( key == "addr:housenumber" ) cflag |= 0x40 ;
                else if ( key == "ele" ) cflag |= 0x20 ;
                else if ( key == "layer" ) layer = safe_stoi(val) ;
                else {
                    uint32_t idx = poi_tag_mapping_[key+'='+val] ;
                    tags.push_back(idx) ;
                }
                ++it ;
            }

            uint8_t tflag = 0 ;

            uint n_tags = tags.size() ;

            tflag |= n_tags & 0x0f ;
            tflag |= ( (layer+5) << 4 ) & 0xf0;
            buffer.write_uint8(tflag) ;

            // write tag index

            for(uint32_t idx: tags)
                buffer.write_var_uint64(idx) ;

            buffer.write_uint8(cflag) ;

            if ( cflag & 0x80 )
                buffer.write_utf8(poi.tags_.get("name")) ;

            if ( cflag & 0x40 )
                buffer.write_utf8(poi.tags_.get("addr:housenumber")) ;

            if ( cflag & 0x20 )
                buffer.write_var_int64(safe_stoi(poi.tags_.get("ele"))) ;

        }
    }

    return strm.str() ;

}
static void write_single_delta(MapFileOSerializer &buffer, const vector<ILatLon> &coords, const ILatLon &origin) {

    int64_t lat = coords[0].lat_ ;
    int64_t lon = coords[0].lon_ ;

    int64_t delta_lat = lat - origin.lat_ ;
    int64_t delta_lon = lon - origin.lon_ ;

    buffer.write_var_int64(delta_lat) ;
    buffer.write_var_int64(delta_lon) ;

    int64_t previous_lat = lat, previous_lon = lon ;

    for( uint i=1 ; i<coords.size() ; i++ ) {

        lat = coords[i].lat_ ;
        lon = coords[i].lon_ ;

        int64_t delta_lat = lat - previous_lat ;
        int64_t delta_lon = lon - previous_lon ;

        buffer.write_var_int64(delta_lat) ;
        buffer.write_var_int64(delta_lon) ;

        previous_lat = lat ;
        previous_lon = lon ;
    }
}

static void write_double_delta(MapFileOSerializer &buffer, const vector<ILatLon> &coords, const ILatLon &origin) {

    int64_t lat = coords[0].lat_ ;
    int64_t lon = coords[0].lon_ ;

    int64_t delta_lat = lat - origin.lat_ ;
    int64_t delta_lon = lon - origin.lon_ ;

    buffer.write_var_int64(delta_lat) ;
    buffer.write_var_int64(delta_lon) ;

    int64_t previous_lat = lat, previous_lon = lon ;

    int64_t previous_delta_lat = 0, previous_delta_lon = 0 ;

    for( uint i=1 ; i<coords.size() ; i++ ) {

        lat = coords[i].lat_ ;
        lon = coords[i].lon_ ;

        int64_t delta_lat = lat - previous_lat ;
        int64_t delta_lon = lon - previous_lon ;

        int64_t offset_lat = delta_lat - previous_delta_lat ;
        int64_t offset_lon = delta_lon - previous_delta_lon ;

        buffer.write_var_int64(offset_lat) ;
        buffer.write_var_int64(offset_lon) ;

        previous_lat = lat ;
        previous_lon = lon ;

        previous_delta_lat = delta_lat ;
        previous_delta_lon = delta_lon ;
    }
}


static string encode_single_delta(const WayDataContainer &wc, const ILatLon &origin) {
    ostringstream strm ;
    MapFileOSerializer buffer(strm) ;

    for( const WayDataBlock &block: wc.blocks_ ) {
        buffer.write_var_uint64(block.coords_.size()) ;

        for( const auto &coords: block.coords_ ) {
            buffer.write_var_uint64(coords.size()) ;
            write_single_delta(buffer, coords, origin) ;
        }
    }

    return strm.str() ;
}

static string encode_double_delta(const WayDataContainer &wc, const ILatLon &origin) {
    ostringstream strm ;
    MapFileOSerializer buffer(strm) ;

    for( const WayDataBlock &block: wc.blocks_ ) {
        buffer.write_var_uint64(block.coords_.size()) ;

        for( const auto &coords: block.coords_ ) {
            buffer.write_var_uint64(coords.size()) ;
            write_double_delta(buffer, coords, origin) ;
        }
    }

    return strm.str() ;
}

static string encode_data(const WayDataContainer &wc, const ILatLon &origin, WayDataContainer::Encoding &enc) {

    string single_delta_encoded_data = encode_single_delta(wc, origin) ;
    string double_delta_encoded_data = encode_double_delta(wc, origin) ;

    if ( single_delta_encoded_data.size() < double_delta_encoded_data.size() ) {
        enc = WayDataContainer::SingleDelta ;
        return single_delta_encoded_data ;
    }
    else {
        enc = WayDataContainer::DoubleDelta ;
        return double_delta_encoded_data ;
    }
}

string MapFileWriter::writeWayData(const vector<WayDataContainer> &ways, const vector<vector<uint32_t> > &ways_per_level, const ILatLon &orig)
{
    ostringstream strm ;
    MapFileOSerializer buffer(strm) ;

    for( uint i=0 ; i<ways_per_level.size() ; i++ ) {

        for( uint j=0 ; j<ways_per_level[i].size() ; j++ ) {
            uint32_t idx = ways_per_level[i][j] ;
            const WayDataContainer &way = ways[idx] ;

            if ( has_debug_info_ ) {
                // write header
                stringstream sigstrm ;
                sigstrm << "---WayStart" << way.id_ << "---" ;
                int extra = 32-sigstrm.tellp() ;
                for ( int i=0 ; i<extra ; i++ ) sigstrm.put(' ') ;
                buffer.write_bytes((uint8_t *)sigstrm.str().c_str(), 32) ;
            }

            // We have to determine the bytes need to encode each way so we render it on a separate string buffer

            ostringstream wstrm ;
            MapFileOSerializer wbuffer(wstrm) ;

            wbuffer.write_uint16(way.subtile_mask_) ;

            // collect tags

            vector<uint32_t> tags ;
            int8_t layer = 0 ;
            uint8_t cflag = 0 ;

            DictionaryIterator it(way.tags_) ;

            while (it) {
                string key = it.key() ;
                string val = it.value() ;

                if ( key == "name" ) cflag |= 0x80 ;
                else if ( key == "addr:housenumber" ) cflag |= 0x40 ;
                else if ( key == "ref" ) cflag |= 0x20 ;
                else if ( key == "layer" ) layer = safe_stoi(val);
                else {
                    uint32_t idx = way_tag_mapping_[key+'='+val] ;
                    tags.push_back(idx) ;
                }
                ++it ;
            }

            uint8_t tflag = 0 ;

            tflag |= tags.size() & 0x0f ;
            tflag |= ( (layer+5) << 4 ) & 0xf0;

            wbuffer.write_uint8(tflag) ;

            // write tag index

            for( uint32_t idx: tags )
                wbuffer.write_var_uint64(idx) ;

            // encode data using either single or double delta encoding, returning the encoding leading to shortest buffer

            WayDataContainer::Encoding encoding ;
            string way_encoded_data = encode_data(way, orig, encoding) ;

            if ( way.label_pos_ ) cflag |= 0x10 ;
            if ( way.blocks_.size() > 1 ) cflag |= 0x08 ;
            if ( encoding == WayDataContainer::DoubleDelta ) cflag |= 0x04 ;

            wbuffer.write_uint8(cflag) ;

            if ( cflag & 0x80 )
                wbuffer.write_utf8(way.tags_.get("name")) ;

            if ( cflag & 0x40 )
                wbuffer.write_utf8(way.tags_.get("addr:housenumber")) ;

            if ( cflag & 0x20 )
                wbuffer.write_utf8(way.tags_.get("ref")) ;

            if ( cflag & 0x10 ) {
                double label_position_lat = way.label_pos_.get().lat_ - orig.lat_;
                double label_position_lon = way.label_pos_.get().lon_ - orig.lon_ ;

                wbuffer.write_var_int64(round(label_position_lat*1.0e6)) ;
                wbuffer.write_var_int64(round(label_position_lon*1.0e6)) ;
            }

            if ( cflag & 0x08 )
                wbuffer.write_var_uint64(way.blocks_.size()) ;

            wbuffer.write_bytes((uint8_t *)&way_encoded_data[0], way_encoded_data.size()) ;

            string wbuffer_bytes = wstrm.str() ;

            // write size and way data to stream
            buffer.write_var_uint64(wbuffer_bytes.size()) ;
            buffer.write_bytes((uint8_t *)&wbuffer_bytes[0], wbuffer_bytes.size()) ;
        }
    }

    return strm.str() ;
}


void MapFileWriter::computeSubTileMask(int btx, int bty, int btz, WayDataContainer &way, float bbox_enlargement)
{
    // get the boundaries of the 16 tiles at zoom level z0+2 and compute their bounding box

    int ofx = btx * 4, ofy = bty * 4 ;
    way.subtile_mask_ = 0 ;

    for (int i = 0, k=0; i < 4; i++) {
        for (int j = 0; j < 4; j++, k++) {
            int tx = ofx + j, ty = ofy + i, tz = btz + 2 ;
            TileKey tk(tx, ty, tz, true) ;
            BBox bbox ;
            tms::tileBounds(tk.x(), tk.y(), tk.z(), bbox.minx_, bbox.miny_, bbox.maxx_, bbox.maxy_, bbox_enlargement) ;

            if ( way.box_.intersects(bbox) ) way.subtile_mask_ |= (1 << (15-k)) ;
        }
    }

}

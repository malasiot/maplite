#include "raster_request_handler.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "geometry.hpp"
#include "tms.hpp"
#include "logger.hpp"
#include "base64.hpp"
#include "png_writer.hpp"

#include <fstream>
#include <png.h>

using namespace std ;
namespace fs = boost::filesystem ;
using namespace http ;

const uint64_t raster_tile_cache_size = 20*1024*1024 ;

RasterTileCache RasterRequestHandler::tile_cache_(raster_tile_cache_size) ;

RasterRequestHandler::RasterRequestHandler(const string &id, const string &tileSet): TileRequestHandler(id, tileSet),
    provider_(&tile_cache_)
{
    if ( !provider_.open(tileset_.native()) ) {
        LOG_FATAL_STREAM("Error opening raster tileset: " << tileset_) ;
    }
}

void RasterRequestHandler::handle_request(const Request &request, Response &resp)
{
    boost::smatch m ;
    boost::regex_match(request.path_.substr(key_.length()), m, uri_pattern_) ;

    int zoom = stoi(m.str(1)) ;
    int tx = stoi(m.str(2)) ;
    int ty = stoi(m.str(3)) ;

    ty = pow(2, zoom) - 1 - ty ;

    LOG_INFO_STREAM("Recieved request for raster tile: (" << tx << '/' << ty << '/' << zoom << ")" << "of map " << key_) ;

    time_t mod_time = boost::filesystem::last_write_time(tileset_.native());

    BBox box ;
    tms::tileBounds(tx, ty, zoom, box.minx_, box.miny_, box.maxx_, box.maxy_) ;

    float scale = provider_.georef_[0] ;
    float C = provider_.georef_[4] ;
    float F = provider_.georef_[5] ;

    float x0 = (box.minx_ - C)/scale ;
    float y1 = -(box.miny_ - F)/scale ;
    float x1 = (box.maxx_ - C)/scale ;
    float y0 = -(box.maxy_ - F)/scale ;

    std::unique_ptr<uint8_t []> buffer(new uint8_t [256*256*4]) ;
    memset(buffer.get(), 0, 256*256*4) ;

    if ( provider_.read(x0, y0, x1-x0, y1-y0, 256, 256, buffer.get(), 256*4) )
    {
        vector<uint8_t> data ;
        save_png(buffer.get(), 256, 256, data) ;

        resp.encode_file_data(std::string(data.begin(), data.end()), string(), "image/png", mod_time) ;

    }
    else {
        string empty_tile = base64_decode(g_empty_transparent_png_256) ;
        resp.encode_file_data(empty_tile, string(), "image/png", mod_time) ;
    }
}

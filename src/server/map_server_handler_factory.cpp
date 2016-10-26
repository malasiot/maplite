#include "map_server_handler_factory.hpp"
#include "request.hpp"
#include "raster_request_handler.hpp"
#include "mapsforge_tile_request_handler.hpp"
#include "pugixml.hpp"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>

using namespace std ;
namespace fs = boost::filesystem ;
using namespace http ;


MapServerHandlerFactory::MapServerHandlerFactory(const string &cfg_file, const std::shared_ptr<FileSystemTileCache> &cache)
{
    // parse maps

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cfg_file.c_str());

    if ( !result ) {
        LOG_FATAL_STREAM("XML [" << cfg_file << "] parsed with errors: " << result.description() ) ;
        return ;
    }

    pugi::xml_node root = doc.child("config") ;
    if ( !root ) {
        LOG_FATAL_STREAM("XML [" << cfg_file << "] error: no <config> element found" ) ;
        return ;
    }

    for( pugi::xml_node p: root.children("assets") ) {
        string key = p.attribute("key").as_string() ;
        string src = p.attribute("src").as_string() ;

        if ( key.empty() ) {
            LOG_FATAL_STREAM("XML [" << cfg_file << "] error: key attribute missing from assets element" ) ;
            return ;
        }

        if ( src.empty() ) {
            LOG_FATAL_STREAM("XML [" << cfg_file << "] error: src attribute missing from assets element" ) ;
            return ;
        }

        try {
            fs::path rp(cfg_file) ;
            fs::path src_path  = fs::canonical(rp.parent_path() / src) ;

            asset_request_handlers_[key] = make_shared<AssetRequestHandler>(key, src_path.native()) ;
        }
        catch ( fs::filesystem_error &e ) {
            LOG_WARN(e.what()) ;
            return ;
        }
    }

    for( pugi::xml_node p: root.children("tiles") ) {
        string key = p.attribute("key").as_string() ;
        string src = p.attribute("src").as_string() ;
        string type = p.attribute("type").as_string() ;
        string theme = p.attribute("theme").as_string() ;
        string layer = p.attribute("layer").as_string() ;
        bool debug_flag = p.attribute("debug").as_bool(false) ;

        if ( key.empty() ) {
            LOG_WARN_STREAM("XML [" << cfg_file << "] error: key attribute missing from tiles element" ) ;
            return ;
        }

        if ( src.empty() && type != "debug" ) {
            LOG_WARN_STREAM("XML [" << cfg_file << "] error: src attribute missing from tiles element" ) ;
            return ;
        }

        if ( type.empty() ) {
            LOG_WARN_STREAM("XML [" << cfg_file << "] error: tiles attribute missing from tiles element" ) ;
            return ;
        }

        try {
            fs::path rp(cfg_file) ;
            fs::path src_path  = fs::canonical(rp.parent_path() / src) ;
            src = src_path.native() ;

            if ( !theme.empty() ) {
                fs::path theme_path = fs::canonical(rp.parent_path() / theme ) ;
                theme = theme_path.native() ;
            }
        }
        catch ( fs::filesystem_error &e ) {
            LOG_WARN(e.what()) ;
            return ;
        }

        if ( type == "raster" )
            tile_request_handlers_[key] = make_shared<RasterRequestHandler>(key, src) ;
        else if ( type == "mapsforge" )
            tile_request_handlers_[key] = make_shared<MapsforgeTileRequestHandler>(key, src, theme, layer, cache, debug_flag) ;
    }



}

std::shared_ptr<RequestHandler> MapServerHandlerFactory::create(const Request &req) {

    for( auto handler: tile_request_handlers_ ) {
        if ( handler.second->matches(req.path_) ) return handler.second ;
    }
    for( auto handler: asset_request_handlers_ ) {
        if ( handler.second->matches(req.path_) ) return handler.second ;
    }

    return nullptr ;
}

#include "map_manager.hpp"
#include "pugixml.hpp"
#include "logger.hpp"
#include "map_file_tile_provider.hpp"
#include "xyz_tile_provider.hpp"

#include <boost/filesystem.hpp>

using namespace std ;
namespace fs = boost::filesystem ;

static string get_absolute_path(const fs::path &folder, const string orig_path) {

    if ( orig_path.empty() ) return orig_path ;

    fs::path full_path ;

    if ( fs::path(orig_path).is_relative() ) {
        full_path = fs::canonical(orig_path, folder) ;
    }
    else full_path = orig_path ;

    if ( fs::exists(full_path) ) return full_path.native() ;
    else return string() ;
}


bool MapManager::parseConfig(const std::string &cfg_path) {

    fs::path rp(cfg_path) ;

    if ( !fs::exists(rp) ) return false ;

    string cfg_file = rp.native() ;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cfg_file.c_str());

    if ( !result ) {
        LOG_WARN_STREAM("XML [" << cfg_file << "] parsed with errors: " << result.description() ) ;
        return false ;
    }

    pugi::xml_node root = doc.child("config") ;

    if ( !root ) return false ;

    for( pugi::xml_node p: root.children("themes") ) {

        for( pugi::xml_node q: p.children("theme") ) {

            string id = q.attribute("id").as_string() ;
            string src = q.attribute("src").as_string() ;
            string name = q.attribute("name").as_string() ;
            bool is_default = q.attribute("default").as_bool() ;
            string resource_dir = q.attribute("resources").as_string() ;
            string attribution, description ;

            if ( id.empty() || themes_.count(id) ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: id attribute missing or non-unique in theme element" ) ;
                continue ;
            }

            if ( name.empty() ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: name attribute missing in theme element" ) ;
                continue ;
            }

            pugi::xml_node e = q.child("description") ;
            if ( e ) description = e.text().as_string() ;

            e = q.child("attribution") ;
            if ( e ) attribution = e.text().as_string() ;

            src = get_absolute_path(rp.parent_path(), src) ;
            resource_dir = get_absolute_path(rp.parent_path(), resource_dir) ;

            if ( src.empty() ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: src attribute missing or path not exists while parsing theme element" ) ;
                continue ;
            }

            std::shared_ptr<RenderTheme> theme(new RenderTheme()) ;

            if ( !theme->read(src, resource_dir) ) continue ;

            ThemeInfo info ;
            info.theme_ = theme ;
            info.attribution_ = attribution ;
            info.description_ = description ;
            info.name_ = name ;
            info.src_ = src ;

            if ( default_theme_id_.empty() || is_default ) default_theme_id_ = id ;

            themes_.emplace(id, info) ;
        }
    }

    for( pugi::xml_node p: root.children("maps") ) {

        for( pugi::xml_node q: p.children("offline") ) {

            string id = q.attribute("id").as_string() ;
            string src = q.attribute("src").as_string() ;
            string name = q.attribute("name").as_string() ;
            bool is_default = q.attribute("default").as_bool() ;
            string attribution, description ;
            int start_zoom = -1 ;
            LatLon start_position ;
            bool has_start_position = false ;

            if ( id.empty() || maps_.count(id) ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: id attribute missing or non-unique in map element" ) ;
                continue ;
            }

            if ( name.empty() ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: name attribute missing in map element" ) ;
                continue ;
            }

            src = get_absolute_path(rp.parent_path(), src) ;

            if ( src.empty() ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: src attribute missing or path not exists while parsing map element" ) ;
                continue ;
            }

            pugi::xml_node e = q.child("description") ;
            if ( e ) description = e.text().as_string() ;

            e = q.child("attribution") ;
            if ( e ) attribution = e.text().as_string() ;

            e = q.child("start_zoom") ;
            if ( e ) start_zoom = e.text().as_int(-1) ;

            e = q.child("start_position") ;
            if ( e ) {
                string txt = e.text().as_string() ;
                istringstream strm(txt) ;
                float lat, lon ;
                strm >> lat >> lon ;
                if ( strm ) {
                    start_position = LatLon(lat, lon) ;
                    has_start_position = true ;
                }
            }

            std::shared_ptr<MapFileReader> reader(new MapFileReader()) ;

            try {
                reader->open(src) ;

                std::shared_ptr<MapFileTileProvider> provider(new MapFileTileProvider(QString::fromUtf8(name.c_str()), reader)) ;

                if ( !description.empty() ) provider->setDescription(QString::fromUtf8(description.c_str()));
                if ( !attribution.empty() ) provider->setAttribution(QString::fromUtf8(attribution.c_str()));
                if ( has_start_position ) provider->setStartPosition(start_position) ;
                if ( start_zoom >= 0 ) provider->setStartZoom(start_zoom) ;

                if ( default_map_id_.empty() || is_default ) default_map_id_ = id ;

                maps_.emplace(id, provider) ;
            }
            catch ( std::runtime_error &e ) {
                cerr << e.what() << endl ;
                continue ;
            }
        }

        for( pugi::xml_node q: p.children("online") ) {

            string id = q.attribute("id").as_string() ;
            string url = q.attribute("url").as_string() ;
            string name = q.attribute("name").as_string() ;
            bool is_default = q.attribute("default").as_bool() ;
            string image_format = q.attribute("format").as_string("png") ;

            string attribution, description ;
            int start_zoom = -1, minz, maxz ;
            LatLon start_position ;
            bool has_start_position = false ;

            if ( id.empty() || maps_.count(id) ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: id attribute missing or non-unique in map element" ) ;
                continue ;
            }

            if ( name.empty() ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: name attribute missing in map element" ) ;
                continue ;
            }

            if ( url.empty() ) {
                LOG_WARN_STREAM("XML [" << cfg_file << "] error: url attribute missing or path not exists while parsing map element" ) ;
                continue ;
            }

            pugi::xml_node e = q.child("description") ;
            if ( e ) description = e.text().as_string() ;

            e = q.child("attribution") ;
            if ( e ) attribution = e.text().as_string() ;

            e = q.child("start_zoom") ;
            if ( e ) start_zoom = e.text().as_int(-1) ;

            e = q.child("minz") ;
            if ( e ) minz = e.text().as_int(0) ;

            e = q.child("maxz") ;
            if ( e ) maxz = e.text().as_int(20) ;

            e = q.child("start_position") ;
            if ( e ) {
                string txt = e.text().as_string() ;
                istringstream strm(txt) ;
                float lat, lon ;
                strm >> lat >> lon ;
                if ( strm ) {
                    start_position = LatLon(lat, lon) ;
                    has_start_position = true ;
                }
            }

            std::shared_ptr<XYZTileProvider> provider(new XYZTileProvider(QString::fromUtf8(name.c_str()), QString::fromUtf8(url.c_str()))) ;

            if ( !description.empty() ) provider->setDescription(QString::fromUtf8(description.c_str()));
            if ( !attribution.empty() ) provider->setAttribution(QString::fromUtf8(attribution.c_str()));

            provider->setZoomRange(minz, maxz) ;
            provider->setTileFormat(image_format.c_str()) ;

            if ( default_map_id_.empty() || is_default ) default_map_id_ = id ;

            maps_.emplace(id, provider) ;

        }
    }

    return true ;
}


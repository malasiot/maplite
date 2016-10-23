#include "map_manager.hpp"
#include "pugixml.hpp"
#include "logger.hpp"
#include "map_file_tile_provider.hpp"
#include "xyz_tile_provider.hpp"

#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

using namespace std ;

static QString get_absolute_path(const QDir &folder, const QString orig_path) {

    if ( orig_path.isEmpty() ) return orig_path ;

    QString full_path ;

    if ( QFileInfo(orig_path).isRelative() ) {
        full_path = folder.absoluteFilePath(orig_path) ;
    }
    else full_path = orig_path ;

    if ( QFileInfo(full_path).exists() ) return full_path ;
    else return QString() ;
}


bool MapManager::parseConfig(const QString &cfg_path) {

    if ( !QFileInfo(cfg_path).exists() ) return false ;

    QDomDocument doc ;
    QFile file(cfg_path);

    if (!file.open(QIODevice::ReadOnly)) return false ;
    QString err_msg ;
    if (!doc.setContent(&file, false, &err_msg)) {
        file.close();
        return false ;
    }
    file.close();

    QDir dir(QFileInfo(cfg_path).absoluteDir()) ;

    QDomElement root = doc.documentElement() ;
    if ( root.tagName() != "config" ) return false ;

    for( QDomElement p = root.firstChildElement("themes") ; !p.isNull() ; p = p.nextSiblingElement("themes") ) {

        for( QDomElement q = p.firstChildElement("theme") ; !q.isNull() ; q = q.nextSiblingElement("theme") ) {

            QByteArray id = q.attribute("id").toAscii() ;
            QString src = q.attribute("src") ;
            QString name = q.attribute("name") ;
            bool is_default = q.attribute("default") == "true" ;
            QString resource_dir = q.attribute("resources") ;
            QString attribution, description ;

            if ( id.isEmpty() || themes_.count(id) ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: id attribute missing or non-unique in theme element" ) ;
                continue ;
            }

            if ( name.isEmpty() ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: name attribute missing in theme element" ) ;
                continue ;
            }

            QDomElement e = q.firstChildElement("description") ;
            if ( !e.isNull() ) description = e.text() ;

            e = q.firstChildElement("attribution") ;
            if ( !e.isNull() ) attribution = e.text() ;

            src = get_absolute_path(dir, src) ;
            resource_dir = get_absolute_path(dir, resource_dir) ;

            if ( src.isEmpty() ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: src attribute missing or path not exists while parsing theme element" ) ;
                continue ;
            }

            std::shared_ptr<RenderTheme> theme(new RenderTheme()) ;

            if ( !theme->read((const char *)src.toUtf8(), (const char *)resource_dir.toUtf8()) ) continue ;

            ThemeInfo info ;
            info.theme_ = theme ;
            info.attribution_ = attribution ;
            info.description_ = description ;
            info.name_ = name ;
            info.src_ = src ;

            if ( default_theme_id_.isEmpty() || is_default ) default_theme_id_ = id ;

            themes_.emplace(id, info) ;
        }
    }
    for( QDomElement p = root.firstChildElement("maps") ; !p.isNull() ; p = p.nextSiblingElement("maps") ) {

        for( QDomElement q = p.firstChildElement("offline") ; !q.isNull() ; q = q.nextSiblingElement("offline") ) {


            QByteArray id = q.attribute("id").toAscii() ;
            QString src = q.attribute("src") ;
            QString name = q.attribute("name") ;
            bool is_default = q.attribute("default") == "true" ;
            QString attribution, description ;
            int start_zoom = -1 ;
            LatLon start_position ;
            bool has_start_position = false ;

            if ( id.isEmpty() || maps_.count(id) ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: id attribute missing or non-unique in map element" ) ;
                continue ;
            }

            if ( name.isEmpty() ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: name attribute missing in map element" ) ;
                continue ;
            }

            src = get_absolute_path(dir, src) ;

            if ( src.isEmpty() ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: src attribute missing or path not exists while parsing map element" ) ;
                continue ;
            }

            QDomElement e = q.firstChildElement("description") ;
            if ( !e.isNull() ) description = e.text() ;

            e = q.firstChildElement("attribution") ;
            if ( !e.isNull() ) attribution = e.text() ;

            bool ok ;
            e = q.firstChildElement("start_zoom") ;
            if ( !e.isNull() ) start_zoom = e.text().toInt(&ok) ;
            if ( !ok ) start_zoom = -1 ;

            e = q.firstChildElement("start_position") ;

            if ( !e.isNull() ) {
                QString txt = e.text() ;
                QTextStream strm(&txt) ;

                float lat, lon ;
                strm >> lat >> lon ;
                if ( strm.status() == 0 ) {
                    start_position = LatLon(lat, lon) ;
                    has_start_position = true ;
                }
            }

            std::shared_ptr<MapFileReader> reader(new MapFileReader()) ;

            try {
                reader->open((const char *)src.toUtf8()) ;



                std::shared_ptr<MapFileTileProvider> provider(new MapFileTileProvider(id, reader)) ;

                provider->setName(name) ;
                if ( !description.isEmpty() ) provider->setDescription(description);
                if ( !attribution.isEmpty() ) provider->setAttribution(attribution);
                if ( has_start_position ) provider->setStartPosition(start_position) ;
                if ( start_zoom >= 0 ) provider->setStartZoom(start_zoom) ;
                provider->setCreationTime(QFileInfo(src).created().toTime_t()) ;

                if ( default_map_id_.isEmpty() || is_default ) default_map_id_ = id ;

                maps_.emplace(id, provider) ;
            }
            catch ( std::runtime_error &e ) {
                cerr << e.what() << endl ;
                continue ;
            }
        }

        for( QDomElement q = p.firstChildElement("online") ; !q.isNull() ; q = q.nextSiblingElement("online") ) {


            QByteArray id = q.attribute("id").toAscii() ;
            QString url = q.attribute("url") ;
            QString name = q.attribute("name") ;
            bool is_default = q.attribute("default") == "true" ;
            QByteArray image_format = q.attribute("format", "png").toAscii() ;

            QString attribution, description ;
            int start_zoom = -1, minz = 0, maxz = 20 ;
            LatLon start_position ;
            bool has_start_position = false ;

            if ( id.isEmpty() || maps_.count(id) ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: id attribute missing or non-unique in map element" ) ;
                continue ;
            }

            if ( name.isEmpty() ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: name attribute missing in map element" ) ;
                continue ;
            }

            if ( url.isEmpty() ) {
                LOG_WARN_STREAM("XML [" << (const char *)cfg_path.toUtf8() << "] error: url attribute missing or path not exists while parsing map element" ) ;
                continue ;
            }

            QDomElement e = q.firstChildElement("description") ;
            if ( !e.isNull() ) description = e.text() ;

            e = q.firstChildElement("attribution") ;
            if ( !e.isNull() ) attribution = e.text() ;

            bool ok ;
            e = q.firstChildElement("start_zoom") ;
            if ( !e.isNull() ) start_zoom = e.text().toInt(&ok) ;
            if ( !ok ) start_zoom = -1 ;

            e = q.firstChildElement("start_position") ;

            if ( !e.isNull() ) {
                QString txt = e.text() ;
                QTextStream strm(&txt) ;

                float lat, lon ;
                strm >> lat >> lon ;
                if ( strm.status() == 0 ) {
                    start_position = LatLon(lat, lon) ;
                    has_start_position = true ;
                }
            }

            e = q.firstChildElement("minz") ;
            if ( !e.isNull() ) minz = e.text().toInt(&ok) ;
            if ( !ok ) minz = 0 ;

            e = q.firstChildElement("maxz") ;
            if ( !e.isNull() ) maxz = e.text().toInt(&ok) ;
            if ( !ok ) maxz = 20 ;


            std::shared_ptr<XYZTileProvider> provider(new XYZTileProvider(id, url)) ;

            provider->setName(name) ;
            if ( !description.isEmpty() ) provider->setDescription(description);
            if ( !attribution.isEmpty() ) provider->setAttribution(attribution);

            provider->setZoomRange(minz, maxz) ;
            provider->setTileFormat(image_format) ;

            if ( default_map_id_.isEmpty() || is_default ) default_map_id_ = id ;

            maps_.emplace(id, provider) ;

        }


    }

    return true ;
}


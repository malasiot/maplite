#ifndef __MAP_MANAGER_HPP__
#define __MAP_MANAGER_HPP__

#include <map>
#include "tile_provider.hpp"
#include "theme.hpp"

#include <QString>
#include <QSettings>

class MapManager {

public:

    MapManager() = default ;

    bool parseConfig(const QString &path) ;

    bool hasMap(const QByteArray &id) const { return maps_.count(id) ; }

    std::shared_ptr<TileProvider> getMap(const QByteArray &id) const {
        auto it = maps_.find(id) ;
        if ( it != maps_.end() ) return it->second ;
        else return nullptr ;
    }

    std::shared_ptr<RenderTheme> getTheme(const QByteArray &id) const {
        auto it = themes_.find(id) ;
        if ( it != themes_.end() ) return it->second.theme_ ;
        else return nullptr ;
    }

    QByteArray getDefaultMap() const {
        return default_map_id_ ;
    }

    QByteArray getDefaultTheme() const {
        return default_theme_id_ ;
    }

    struct ThemeInfo {
        QString description_ ;
        QString attribution_ ;
        QString name_ ;
        QString src_ ;
        std::shared_ptr<RenderTheme> theme_ ;
    };

    struct ThemeBinding {

        void read(QSettings &sts) ;
        void write(QSettings &sts) ;

        QByteArray theme_id_ ;
        QByteArray style_id_ ;
        QList<QByteArray> layers_ ;
    };

    std::map<QByteArray, std::shared_ptr<TileProvider> > maps_ ;
    std::map<QByteArray, ThemeInfo> themes_ ;
    std::map<QByteArray, ThemeBinding> bindings_ ;

    QByteArray default_map_id_, default_theme_id_ ;
};






#endif

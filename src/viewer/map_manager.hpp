#ifndef __MAP_MANAGER_HPP__
#define __MAP_MANAGER_HPP__

#include <map>
#include "tile_provider.hpp"
#include "theme.hpp"

#include <QString>

class MapManager {

public:

    MapManager() = default ;

    bool parseConfig(const std::string &path) ;

    bool hasMap(const std::string id) const { return maps_.count(id) ; }

    std::shared_ptr<TileProvider> getMap(const std::string &id) const {
        auto it = maps_.find(id) ;
        if ( it != maps_.end() ) return it->second ;
        else return nullptr ;
    }

    std::shared_ptr<RenderTheme> getTheme(const std::string &id) const {
        auto it = themes_.find(id) ;
        if ( it != themes_.end() ) return it->second.theme_ ;
        else return nullptr ;
    }

    std::string getDefaultMap() const {
        return default_map_id_ ;
    }

    std::string getDefaultTheme() const {
        return default_theme_id_ ;
    }

    struct ThemeInfo {
        std::string description_ ;
        std::string attribution_ ;
        std::string name_ ;
        std::string src_ ;
        std::shared_ptr<RenderTheme> theme_ ;
    };

    std::map<std::string, std::shared_ptr<TileProvider> > maps_ ;
    std::map<std::string, ThemeInfo> themes_ ;
    std::string default_map_id_, default_theme_id_ ;
};






#endif

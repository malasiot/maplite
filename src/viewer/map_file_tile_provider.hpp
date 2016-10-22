#ifndef __MAP_FILE_TILE_PROVIDER_H__
#define __MAP_FILE_TILE_PROVIDER_H__

#include "tile_provider.hpp"

#include <QSharedPointer>
#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "renderer.hpp"

#include <string>

class MapFileTileProvider: public TileProvider
{
public:

    MapFileTileProvider(const QByteArray &id, const std::shared_ptr<MapFileReader> &reader) ;

    void setTheme(const std::shared_ptr<RenderTheme> &theme) {
        theme_ = theme ;
        renderer_->setTheme(theme) ;
    }

    void setThemeId(const QByteArray &theme_id) {
        theme_id_ = theme_id ;
    }

    void setStyle(const QByteArray &style) {
        style_ = style ;
    }

    QImage getTile(int x, int y, int z) ;
    QString name() const ;

    QByteArray key() const { return id_ + "_" + theme_id_ + " " + style_ ; }

private:

    std::shared_ptr<MapFileReader> reader_ ;
    std::shared_ptr<Renderer> renderer_ ;
    std::shared_ptr<RenderTheme> theme_ ;
    QByteArray style_, theme_id_ ;

};


#endif

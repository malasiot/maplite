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

    MapFileTileProvider(const QString &name, const std::shared_ptr<MapFileReader> &reader) ;

    void setTheme(const std::shared_ptr<RenderTheme> &theme) {
        theme_ = theme ;
        renderer_->setTheme(theme) ;
    }

    void setLayer(const std::string &layer) {
        layer_ = layer ;
    }

    QImage getTile(int x, int y, int z) ;
    QString name() const ;


private:

    std::shared_ptr<MapFileReader> reader_ ;
    std::shared_ptr<Renderer> renderer_ ;
    std::shared_ptr<RenderTheme> theme_ ;
    std::string layer_ ;

};


#endif

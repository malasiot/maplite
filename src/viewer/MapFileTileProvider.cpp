#include "MapFileTileProvider.h"

#include "renderer.hpp"

using namespace std ;

MapFileTileProvider::MapFileTileProvider(const QString &name, const std::shared_ptr<MapFileReader> &reader):
    TileProvider(name, 256), reader_(reader)
{
    const MapFileInfo &info = reader_->getMapFileInfo() ;
    if ( info.flags_ & 0x40 ) setStartPosition(LatLon(info.start_lat_, info.start_lon_)) ;
    if ( info.flags_ & 0x20 ) setStartZoom(info.start_zoom_level_) ;
    setZoomRange(info.min_zoom_level_, info.max_zoom_level_) ;

    renderer_ = std::shared_ptr<Renderer>(new Renderer(info.lang_preference_)) ;
}


QImage MapFileTileProvider::getTile(int x, int y, int z)
{
    TileKey key(x, y, z, true) ;


    VectorTile tile = reader_->readTile(key, 1);

    ImageBuffer buf(256, 256) ;

    renderer_->render(key, buf, tile, layer_ ) ;

    string data ;
    buf.saveToPNGBuffer(data);

    QImage img ;
    img.loadFromData((const uchar *)data.data(), data.size()) ;

    return img ;
}

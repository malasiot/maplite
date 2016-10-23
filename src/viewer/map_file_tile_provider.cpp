#include "map_file_tile_provider.hpp"

using namespace std ;

MapFileTileProvider::MapFileTileProvider(const QByteArray &id, const std::shared_ptr<MapFileReader> &reader):
    TileProvider(id, 256), reader_(reader), file_time_(0)
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

    renderer_->render(key, buf, tile, (const char *)style_ ) ;

    string data ;
    buf.saveToPNGBuffer(data);

    QImage img ;
    img.loadFromData((const uchar *)data.data(), data.size()) ;

    return img ;
}

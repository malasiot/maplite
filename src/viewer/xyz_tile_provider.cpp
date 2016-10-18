#include "xyz_tile_provider.hpp"

QString XYZTileProvider::makeRequestUrl(int x, int y, int z) {
    QString url = url_ ;
    url.replace("${x}", QString("%1").arg(x)) ;
    url.replace("${y}", QString("%1").arg(y)) ;
    url.replace("${z}", QString("%1").arg(z)) ;

    return url ;
}

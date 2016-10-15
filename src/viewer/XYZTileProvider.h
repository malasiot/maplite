#ifndef __XYZ_TILE_PROVIDER_H__
#define __XYZ_TILE_PROVIDER_H__

#include "NetworkTileProvider.h"

class XYZTileProvider: public NetworkTileProvider
{
public:
    XYZTileProvider(const QString &name, const QString &url):
        NetworkTileProvider(name, url) {}

    QString makeRequestUrl(int x, int y, int z) {
        return url_.arg(z).arg(x).arg(y) ;
    }
} ;

#endif

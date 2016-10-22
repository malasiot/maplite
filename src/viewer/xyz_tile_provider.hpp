#ifndef __XYZ_TILE_PROVIDER_H__
#define __XYZ_TILE_PROVIDER_H__

#include "network_tile_provider.hpp"

class XYZTileProvider: public NetworkTileProvider
{
public:
    XYZTileProvider(const QByteArray &id, const QString &url):
        NetworkTileProvider(id, url) {}

    QString makeRequestUrl(int x, int y, int z) ;

} ;

#endif

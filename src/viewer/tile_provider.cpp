#include "tile_provider.hpp"

#include <QDebug>

QByteArray TileProvider::tileKey(int x, int y, int z) const
{
    QByteArray ba ;

    QTextStream strm(&ba, QIODevice::WriteOnly) ;
    strm << key() << "/" << x << "/" << y << "/" << z ;
    strm.flush() ;

    return ba ;
}

void TileProvider::coordsFromTileKey(const QByteArray &key, int &x, int &y, int &z)
{
    QList<QByteArray> tokens = key.split('/') ;

    x = tokens.at(1).toInt() ;
    y = tokens.at(2).toInt() ;
    z = tokens.at(3).toInt() ;
}


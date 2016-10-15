#include "TileProvider.h"

#include <QDebug>

QString TileProvider::getKey(int x, int y, int z)
{
    return name_ + QString("/%1/%2/%3").arg(x).arg(y).arg(z) ;
}

void TileProvider::coordsFromKey(const QString &key, int &x, int &y, int &z)
{
    QStringList tokens = key.split('/') ;

    x = tokens.at(1).toInt() ;
    y = tokens.at(2).toInt() ;
    z = tokens.at(3).toInt() ;
}


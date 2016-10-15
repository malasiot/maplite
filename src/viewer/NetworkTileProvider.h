#ifndef __NETWORK_TILE_PROVIDER_H__
#define __NETWORK_TILE_PROVIDER_H__

#include "TileProvider.h"

#include <QNetworkAccessManager>
#include <QEventLoop>

class NetworkTileProvider: public TileProvider
{
    Q_OBJECT
public:
    NetworkTileProvider(const QString &name, const QString &url_template_) ;

    QImage getTile(int x, int y, int z) ;

    QByteArray getTileFormat() const { return format_.isEmpty() ? "PNG" : format_ ; }
    void setTileFormat(const QByteArray &format) { format_ = format ; }

    virtual QString makeRequestUrl(int x, int y, int z) = 0 ;

private Q_SLOTS:

    void finished() ;

protected:

    QNetworkAccessManager *manager_ ;
    QString url_ ;
    QByteArray format_ ;
};


#endif

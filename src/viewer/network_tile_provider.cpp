#include "network_tile_provider.hpp"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImageReader>
#include <QDebug>
#include <QPainter>
#include <QCoreApplication>
#include <QDebug>

NetworkTileProvider::NetworkTileProvider(const QByteArray &id, const QString &url):
    TileProvider(id, 256), url_(url)
{
    async_ = true ;
    manager_ = new QNetworkAccessManager(this) ;
}


const QByteArray UserAgentString = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:30.0) Gecko/20100101 Firefox/30.0" ;

QImage NetworkTileProvider::getTile(int x, int y, int z)
{

    QString urlStr = makeRequestUrl(x, y, z) ;

    QUrl url(urlStr) ;
    QNetworkRequest req(url) ;

    req.setRawHeader( "User-Agent" , UserAgentString );
    req.setRawHeader( "Host" , url.host().toAscii() );

    QNetworkReply *reply = manager_->get(req) ;
    reply->setProperty("key", tileKey(x, y, z)) ;
    reply->setProperty("url", url) ;

    connect(reply, SIGNAL(finished()), this, SLOT(finished()));

    return QImage() ;
}

void NetworkTileProvider::finished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray key = reply->property("key").toByteArray();
    QUrl orig_url = reply->property("url").toUrl() ;

    QUrl possibleRedirectUrl =
                 reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl() ;

    if( !possibleRedirectUrl.isEmpty() && possibleRedirectUrl != orig_url ) {

        QNetworkRequest req(possibleRedirectUrl) ;
        req.setRawHeader( "User-Agent" , UserAgentString );
        req.setRawHeader( "Host" , possibleRedirectUrl.host().toAscii() );

        QNetworkReply *redirect = manager_->get(req);
        redirect->setProperty("url", possibleRedirectUrl) ;
        redirect->setProperty("key", key) ;
        connect(redirect, SIGNAL(finished()), this, SLOT(finished()));
    }
    else if ( reply->error() == QNetworkReply::NoError )
    {
        QImageReader imageReader(reply);
        imageReader.setAutoDetectImageFormat(false);
        imageReader.setFormat(getTileFormat());
        QImage pic = imageReader.read();

        emit tileReady(key, pic) ;
    }
    else
    {
        emit tileReady(key, QImage()) ;
    }

    reply->deleteLater();
}

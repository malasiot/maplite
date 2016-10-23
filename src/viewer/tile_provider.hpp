#ifndef __TILE_PROVIDER_H__
#define __TILE_PROVIDER_H__

#include <QPixmap>
#include <QPixmapCache>
#include "geometry.hpp"

class TileProvider: public QObject
{
    Q_OBJECT
public:
    TileProvider(const QByteArray &id, unsigned int tileSize):
        tile_size_(tileSize), id_(id), async_(false), start_zoom_(-1), has_start_position_(false) {}

    virtual QImage getTile(int x, int y, int z) = 0 ;

    QByteArray id() const {  return id_ ; }

    void setName(const QString &name) {
        name_ = name ;
    }

    QString name() const { return name_ ; }
    QString attribution() const { return attribution_ ; }
    QString description() const { return description_ ; }
    unsigned int tileSize() const { return tile_size_ ; }
    bool isAsync() const { return async_ ; }

    void coordsFromTileKey(const QByteArray &key, int &x, int &y, int &z) ;

    void setDescription(const QString &desc) { description_ = desc ; }
    void setAttribution(const QString &attr) { attribution_ = attr ; }
    void setZoomRange(int minZ, int maxZ) {  min_zoom_ = minZ ;  max_zoom_ = maxZ ;  }

    int getStartZoom() const { return start_zoom_ ; }
    QPointF getStartPosition() const { return start_position_ ; }
    bool hasStartPosition() const { return has_start_position_ ; }

    void setStartZoom(int z) { start_zoom_ = z ; }
    void setStartPosition(const LatLon &center) {
        start_position_ = QPointF(center.lat_, center.lon_) ;
        has_start_position_ = true ;
    }

    virtual QByteArray key() const { return id_ ; }
    virtual QByteArray tileKey(int x, int y, int z) const ;
    virtual time_t creationTime() const { return 0 ; }

protected:

    unsigned int tile_size_, min_zoom_, max_zoom_ ;
    QString name_ ;
    QByteArray id_ ;
    QString attribution_, description_ ;
    bool async_ ;
    QPointF start_position_ ;
    int start_zoom_ ;
    bool has_start_position_ ;

Q_SIGNALS:

    void tileReady(QByteArray, QImage) ;

} ;


#endif

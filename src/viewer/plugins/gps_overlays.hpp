#include "map_overlay.hpp"

class GPSTrackOverlay: public PolygonOverlay {
public:
public:

    GPSTrackOverlay(const QString &name): PolygonOverlay(name) {}

    void draw(QPainter &p, MapWidget *w) ;

    MapOverlayGeometryType geomType() const { return PolygonGeometry ; }

    MapOverlayPtr clone() const ;

    void serialize(QDataStream &ds) const {
        PolygonOverlay::serialize(ds) ;
        ds << elevation_ ;

    }

    void deserialize(QDataStream &ds) {
        PolygonOverlay::deserialize(ds) ;
        ds >> elevation_ ;
    }

    virtual QByteArray type() const {
        return "gps_track" ;
    }

    virtual int cost() const {
        PolygonOverlay::cost() + sizeof(double) * elevation_.size() ;
    }

    void addElevation(double ele) {
        elevation_.append(ele) ;
    }

protected:

    QVector<double> elevation_ ;

private:
    void drawArrows(QPainter &painter, MapWidget *parent);

};

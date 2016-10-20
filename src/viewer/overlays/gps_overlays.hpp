#ifndef __GPS_OVERLAYS_HPP__
#define __GPS_OVERLAYS_HPP__

#include "map_overlay.hpp"

class GPSTrackOverlay: public PolygonOverlay {
public:
public:

    GPSTrackOverlay(const QString &name): PolygonOverlay(name) {}

    virtual void draw(QPainter &p, MapWidget *w) ;

    virtual MapOverlayGeometryType geomType() const { return PolygonGeometry ; }

    virtual MapOverlayPtr clone() const ;

    virtual void serialize(QDataStream &ds) const {
        PolygonOverlay::serialize(ds) ;
        ds << elevation_ ;

    }

    virtual void deserialize(QDataStream &ds) {
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

class GPSOverlaysFactory: public MapOverlayFactory {
public:

    GPSOverlaysFactory() {
        MapOverlayFactoryManager::instance().registerFactory(&instance_) ;
    }

    virtual MapOverlayPtr create(const std::string &type_name, const QString &name) override {
        if ( type_name == "gps_track" ) return MapOverlayPtr(new GPSTrackOverlay(name)) ;
        else return MapOverlayPtr() ;
    }

    static GPSOverlaysFactory instance_ ;

};


#endif

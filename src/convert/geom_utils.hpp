#ifndef __GEOM_UTILS_HPP__
#define __GEOM_UTILS_HPP__

#include "geometry.hpp"
#include "database.hpp"
#include "osm_document.hpp"

#include <sqlite3.h>
#include <spatialite.h>

struct gaiaGeomCollDeleter {
    void operator()(gaiaGeomCollPtr p) { if ( p ) gaiaFreeGeomColl(p); }
};

typedef std::unique_ptr<gaiaGeomColl, gaiaGeomCollDeleter> gaiaGeomCollAutoPtr ;

gaiaGeomCollAutoPtr makeBoxGeometry(const BBox &box, double buffer = 0, int srid = 4326) ;
gaiaGeomCollAutoPtr makePoint(const OSM::Node &node, int srid = 4326) ;
gaiaGeomCollAutoPtr makeLineString(const OSM::Way &way, OSM::DocumentReader &reader, int srid = 4326) ;
gaiaGeomCollAutoPtr makeMultiLineString(const std::vector<OSM::Way> &ways, OSM::DocumentReader &reader, int srid = 4326) ;
gaiaGeomCollAutoPtr makeSimplePolygon(const OSM::Ring &ring, OSM::DocumentReader &reader, int srid = 4326) ;
gaiaGeomCollAutoPtr makeMultiPolygon(const OSM::Polygon &poly, OSM::DocumentReader &reader, int srid = 4326) ;

struct WKBBuffer {
    WKBBuffer(const gaiaGeomCollAutoPtr &geom) {
        gaiaToSpatiaLiteBlobWkb(geom.get(), &data_, &sz_) ;
    }
    ~WKBBuffer() {
        gaiaFree(data_) ;
    }

    SQLite::Blob blob() const { return SQLite::Blob( (const char *)data_, sz_ ) ; }

    unsigned char *data_ ;
    int sz_ ;
};

gaiaGeomCollAutoPtr readWKB(const SQLite::Blob &blob) ;




#endif

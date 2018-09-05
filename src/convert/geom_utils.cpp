#include "geom_utils.hpp"

using namespace std ;

gaiaGeomCollAutoPtr makeBoxGeometry(const BBox &bbox, double buffer, int srid)
{
    gaiaGeomCollPtr box = gaiaAllocGeomColl();
    box->Srid = srid;
    box->DeclaredType = GAIA_POLYGON ;

    gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl (box, 5, 0) ;
    gaiaRingPtr er = g_poly->Exterior ;

    gaiaSetPoint (er->Coords, 0, bbox.minx_-buffer, bbox.miny_-buffer);
    gaiaSetPoint (er->Coords, 1, bbox.maxx_+buffer, bbox.miny_-buffer);
    gaiaSetPoint (er->Coords, 2, bbox.maxx_+buffer, bbox.maxy_+buffer);
    gaiaSetPoint (er->Coords, 3, bbox.minx_-buffer, bbox.maxy_+buffer);
    gaiaSetPoint (er->Coords, 4, bbox.minx_-buffer, bbox.miny_-buffer);

    return gaiaGeomCollAutoPtr(box) ;
}

gaiaGeomCollAutoPtr makeLineString(const OSM::Way &way, OSM::Storage &reader, int srid) {

    gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
    geo_line->Srid = srid;
    geo_line->DeclaredType = GAIA_LINESTRING ;

    gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

    size_t j = 0 ;
    reader.forAllWayCoords(way.id_, [&](osm_id_t id, double lat, double lon) {
        gaiaSetPoint (ls->Coords, j, lon, lat); ++j ;
    }) ;

    if ( j != way.nodes_.size() ) return nullptr ;
    else return gaiaGeomCollAutoPtr(geo_line) ;
}

gaiaGeomCollAutoPtr makeMultiLineString(const vector<OSM::Way> &ways, OSM::Storage &reader, int srid) {

    gaiaGeomCollPtr geo_mline = gaiaAllocGeomColl();
    geo_mline->Srid = srid ;
    geo_mline->DeclaredType = GAIA_MULTILINESTRING ;

    for( auto &way: ways ) {

        if ( way.nodes_.size() < 2 ) continue ;

        gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_mline, way.nodes_.size());

        size_t j=0 ;
        reader.forAllNodeCoordList(way.nodes_, [&](double lat, double lon) {
            gaiaSetPoint (ls->Coords, j, lon, lat); ++j ;
        }) ;

        if ( j != way.nodes_.size() ) return nullptr ;
    }

    return gaiaGeomCollAutoPtr(geo_mline) ;
}

gaiaGeomCollAutoPtr makeSimplePolygon(const OSM::Ring &ring, OSM::Storage &reader, int srid)
{
    gaiaGeomCollPtr geom = gaiaAllocGeomColl();
    geom->Srid = srid;
    geom->DeclaredType = GAIA_POLYGON ;

    gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl(geom, ring.nodes_.size(), 0);

    int j=0 ;
    reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
        gaiaSetPoint (g_poly->Exterior->Coords, j, lon, lat); ++j ;
    }) ;

    return gaiaGeomCollAutoPtr(geom) ;
}


gaiaGeomCollAutoPtr makeMultiPolygon(const OSM::Polygon &poly, OSM::Storage &reader, int srid)
{
    gaiaGeomCollPtr geom = nullptr ;

    gaiaGeomCollPtr ls_geom = gaiaAllocGeomColl();
    ls_geom->Srid = srid;

    for( uint i=0 ; i<poly.rings_.size() ; i++ ) {
        const OSM::Ring &ring = poly.rings_[i] ;
        gaiaLinestringPtr ls =  gaiaAddLinestringToGeomColl(ls_geom, ring.nodes_.size()) ;

        int j=0 ;
        reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
            gaiaSetPoint (ls->Coords, j, lon, lat); ++j ;
        }) ;
    }

    // at the moment the functions bellow ignore invalid (self-intersecting) polygons
    // self-intersection can be handled at the level of the multi-polygon parsing function (makePolygonsFromRelation)
    gaiaGeomCollPtr ps = gaiaSanitize(ls_geom) ;

    gaiaFreeGeomColl(ls_geom) ;

    if ( ps ) {
        geom = gaiaPolygonize(ps, 1) ;
        gaiaFreeGeomColl(ps) ;
    }

    return gaiaGeomCollAutoPtr(geom) ;
}


gaiaGeomCollAutoPtr makePoint(const OSM::Node &poi, int srid)
{
    gaiaGeomCollPtr geo_pt = gaiaAllocGeomColl();
    geo_pt->DeclaredType = GAIA_POINT ;
    geo_pt->Srid = srid ;

    gaiaAddPointToGeomColl (geo_pt, poi.lon_, poi.lat_);

    return gaiaGeomCollAutoPtr(geo_pt) ;
}


gaiaGeomCollAutoPtr readWKB(const SQLite::Blob &blob)
{
    return gaiaGeomCollAutoPtr(gaiaFromSpatiaLiteBlobWkb((unsigned char *)blob.data(), blob.size())) ;
}

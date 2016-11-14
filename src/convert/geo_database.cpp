#include "geo_database.hpp"

#include <geos/geom/Point.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/LineString.h>
#include <geos/io/WKBWriter.h>
#include <memory>

using namespace std ;
using namespace geos::geom ;
using namespace geos::io ;

static void write_v(ostream &strm, uint64_t uv) {
    while ( uv > 0x7F ) {
        strm.put((static_cast<uint8_t>(uv) & 0x7F) | 0x80);
        uv >>= 7 ;
    }

    strm.put(static_cast<uint8_t>(uv) & 0x7F) ;
}

static void write_v(ostream &strm, int64_t uv)
{
    write_v(strm, (uint64_t)((uv << 1) ^ (uv >> 63))) ;
}


static uint64_t read_v_u64(istream &strm)
{
    uint64_t uv ;

    uv = UINT64_C(0);
    unsigned int bits = 0;
    uint32_t b ;

    while ( 1 )
    {
        char c ;
        strm.get(c) ;
        b = c ;

        uv |= ( b & 0x7F ) << bits;

        if ( !(b & 0x80) ) break ;

        bits += 7 ;

        if ( bits > 63 ) throw runtime_error("Variable length integer is too long") ;
    }

    return uv ;

}

static int64_t read_v_s64(istream &strm)
{
    int64_t uv ;
    uint64_t v = read_v_u64(strm) ;
    uv = (v >> 1) ^ -static_cast<int64_t>(v & 1) ;
    return uv ;

}


static void write_uint32(ostream &strm, uint32_t val) {
    strm.write((const char *)&val, 4) ;
}

static void write_uint16(ostream &strm, uint16_t val) {
    strm.write((const char *)&val, 2) ;
}

static void write_uint8(ostream &strm, uint8_t val) {
    strm.write((const char *)&val, 1) ;
}

static void write_string(ostream &strm, const string &val) {
    write_v(strm, val.length()) ;
    strm.write((const char *)&val[0], val.length()) ;
}

void serialize_tags(ostream &strm, const Dictionary &tags) {
    write_v(strm, (uint64_t)tags.count()) ;

    DictionaryIterator it(tags) ;

    while ( it ) {
        write_string(strm, it.key()) ;
        write_string(strm, it.value()) ;
        ++it ;
    }
}

bool GeoDatabase::addPointGeometry(const OSM::Node &poi, uint8_t zmin, uint8_t zmax)
{
    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    std::unique_ptr<Point> p(factory.createPoint(Coordinate(poi.lon_, poi.lat_))) ;
    std::unique_ptr<Geometry> e(p->getEnvelope()) ;

    ostringstream buffer ;

    WKBWriter writer ;
    writer.write(*p, buffer);

    string bytes = buffer.str() ;

    data_.push_back({poi.id_, 0, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    strm_.write(bytes.data(), bytes.size()) ;

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;

    return true ;
}

bool GeoDatabase::addPolygonGeometry(OSM::DocumentReader &reader, const OSM::Polygon &poly, int64_t id, int ft, uint8_t zmin, uint8_t zmax)
{
    if ( poly.rings_.size() == 1 ) { // simple polygon case

        const OSM::Ring &ring = poly.rings_[0] ;


        gaiaPolygonPtr g_poly = gaiaAddPolygonToGeomColl(geom, ring.nodes_.size(), 0);

        int j=0 ;
        reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
            gaiaSetPoint (g_poly->Exterior->Coords, j, lon, lat); ++j ;
        }) ;

    }
    else { // multipolygon, use gaiPolygonize to find outer and inner rings
        gaiaGeomCollPtr ls_geom = gaiaAllocGeomColl();
        ls_geom->Srid = 4326;

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
    }



}

bool GeoDatabase::addLineGeometry(OSM::DocumentReader &reader, const OSM::Way &way, uint8_t zmin, uint8_t zmax)
{
    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    std::unique_ptr<CoordinateArraySequence> cl(new CoordinateArraySequence());

    int j = 0 ;
    reader.forAllWayCoords(way.id_, [&](osm_id_t id, double lat, double lon) {
        cl->add(Coordinate(lon, lat)) ;
    }) ;

    std::unique_ptr<LineString> ls(factory.createLineString(cl.get()));
    std::unique_ptr<Geometry> e(ls->getEnvelope()) ;

    ostringstream buffer ;

    WKBWriter writer ;
    writer.write(*ls, buffer);

    string bytes = buffer.str() ;

    data_.push_back({way.id_, 1, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    strm_.write(bytes.data(), bytes.size()) ;

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;

    return true ;
}


bool GeoDatabase::addMultiLineGeometry(OSM::DocumentReader &reader, const vector<OSM::Way> &ways, int64_t id, int ft, uint8_t zmin, uint8_t zmax)
{
    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    vector<Geometry *> geometries ;
    for( const OSM::Way &way: ways) {
        std::unique_ptr<CoordinateArraySequence> cl(new CoordinateArraySequence());

        int j = 0 ;
        reader.forAllWayCoords(way.id_, [&](osm_id_t id, double lat, double lon) {
            cl->add(Coordinate(lon, lat)) ;
        }) ;

        LineString *ls = factory.createLineString(cl.get());

        geometries.push_back(ls) ;
    }

    std::unique_ptr<MultiLineString> geom(factory.createMultiLineString(&geometries)) ;
    std::unique_ptr<Geometry> e(geom->getEnvelope()) ;

    ostringstream buffer ;

    WKBWriter writer ;
    writer.write(*geom, buffer);

    string bytes = buffer.str() ;

    data_.push_back({id, ft, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    strm_.write(bytes.data(), bytes.size()) ;

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;

    return true ;
}

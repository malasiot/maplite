#include "geo_database.hpp"

#include <geos/geom/Point.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/io/WKBWriter.h>
#include <memory>

#include <boost/filesystem.hpp>

#include "geometry.hpp"

using namespace std ;
using namespace geos::geom ;
using namespace geos::io ;
using namespace geos::operation ;
#if 0
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

#endif
bool GeoDatabase::addPointGeometry(const OSM::Node &poi, uint8_t zmin, uint8_t zmax)
{
    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    std::unique_ptr<Point> p(factory.createPoint(Coordinate(poi.lon_, poi.lat_))) ;
    std::unique_ptr<Geometry> e(p->getEnvelope()) ;

    data_.push_back({poi.id_, osm_node_t, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    WKBWriter writer ;
    writer.write(*p, strm_);

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;
    envelope_.expandToInclude((const Envelope *)e.get());

    return true ;
}

bool GeoDatabase::addPolygonGeometry(OSM::DocumentReader &reader, const OSM::Polygon &poly, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax)
{
    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    if ( poly.rings_.size() == 1 ) { // simple polygon case

        const OSM::Ring &ring = poly.rings_[0] ;

        CoordinateArraySequence *cl = new CoordinateArraySequence();

        reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
            cl->add(Coordinate(lon, lat)) ;
        }) ;

        LinearRing *shell = factory.createLinearRing(cl);

        std::unique_ptr<Polygon> geom(factory.createPolygon(shell, nullptr)) ;
        std::unique_ptr<Geometry> e(geom->getEnvelope()) ;

        data_.push_back({id, ft, zmin, zmax, (uint64_t)strm_.tellg()}) ;

        WKBWriter writer ;
        writer.write(*geom, strm_);

        index_.insert((const Envelope *)e.get(), &(data_.back())) ;
        envelope_.expandToInclude((const Envelope *)e.get());
    }
    else { // multipolygon, use Polygonize to find outer and inner rings

        polygonize::Polygonizer pg ;

        for( uint i=0 ; i<poly.rings_.size() ; i++ ) {
            const OSM::Ring &ring = poly.rings_[i] ;

            CoordinateArraySequence *cl = new CoordinateArraySequence();

            reader.forAllNodeCoordList(ring.nodes_, [&](double lat, double lon) {
                cl->add(Coordinate(lon, lat)) ;
            }) ;

            LineString *ls = factory.createLineString(cl);

            pg.add((Geometry *)ls);
        }

        std::vector<Geometry *> *polygons = (vector<Geometry *> *)pg.getPolygons();

        if ( polygons ) {
            std::unique_ptr<MultiPolygon> geom(factory.createMultiPolygon(polygons)) ;
            std::unique_ptr<Geometry> e(geom->getEnvelope()) ;

            data_.push_back({id, ft, zmin, zmax, (uint64_t)strm_.tellg()}) ;

            WKBWriter writer ;
            writer.write(*geom, strm_);

            index_.insert((const Envelope *)e.get(), &(data_.back())) ;
            envelope_.expandToInclude((const Envelope *)e.get());
        }
        else {
            return false ;
        }

    }

    return true ;

}

GeoDatabase::GeoDatabase()
{
    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path() ;
    boost::filesystem::path tmp_file = boost::filesystem::unique_path("%%%%%.geodb");

    string geo_db_file = ( tmp_dir / tmp_file ).native() ;

    cout << geo_db_file << endl ;

    strm_.open(geo_db_file, ios::in | ios::out | ios::binary | ios::trunc) ;
}

bool GeoDatabase::addLineGeometry(OSM::DocumentReader &reader, const OSM::Way &way, uint8_t zmin, uint8_t zmax)
{
    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    CoordinateArraySequence *cl = new CoordinateArraySequence ;

    int j = 0 ;
    reader.forAllWayCoords(way.id_, [&](osm_id_t id, double lat, double lon) {
        cl->add(Coordinate(lon, lat)) ;
    }) ;

    std::unique_ptr<LineString> ls(factory.createLineString(cl));
    std::unique_ptr<Geometry> e(ls->getEnvelope()) ;

    data_.push_back({way.id_, osm_way_t, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    WKBWriter writer ;
    writer.write(*ls, strm_);

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;
    envelope_.expandToInclude((const Envelope *)e.get());

    return true ;
}

bool GeoDatabase::addGeometry(Geometry *geom, osm_id_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax)
{
    std::unique_ptr<Geometry> e(geom->getEnvelope()) ;

    data_.push_back({id, ft, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    WKBWriter writer ;
    writer.write(*geom, strm_);

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;
    envelope_.expandToInclude((const Envelope *)e.get());

    return true ;
}


bool GeoDatabase::addMultiLineGeometry(OSM::DocumentReader &reader, const vector<OSM::Way> &ways, int64_t id, osm_feature_t ft, uint8_t zmin, uint8_t zmax)
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

    data_.push_back({id, ft, zmin, zmax, (uint64_t)strm_.tellg()}) ;

    WKBWriter writer ;
    writer.write(*geom, strm_);

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;
    envelope_.expandToInclude((const Envelope *)e.get());

    return true ;
}

bool GeoDatabase::addBoxGeometry(const BBox &box, osm_id_t id ) {

    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    CoordinateArraySequence *cl = new CoordinateArraySequence();

    cl->add(Coordinate(box.minx_, box.miny_));
    cl->add(Coordinate(box.maxx_, box.miny_));
    cl->add(Coordinate(box.maxx_, box.maxy_));
    cl->add(Coordinate(box.minx_, box.maxy_));
    cl->add(Coordinate(box.minx_, box.miny_));

    LinearRing *shell = factory.createLinearRing(cl);

    std::unique_ptr<Polygon> geom(factory.createPolygon(shell, nullptr)) ;
    std::unique_ptr<Geometry> e(geom->getEnvelope()) ;

    data_.push_back({id, osm_way_t, 0, 255, (uint64_t)strm_.tellg()}) ;

    WKBWriter writer ;
    writer.write(*geom, strm_);

    index_.insert((const Envelope *)e.get(), &(data_.back())) ;
    envelope_.expandToInclude((const Envelope *)e.get());

    return true ;
}

bool GeoDatabase::addTags(const TagWriteList &tags, osm_id_t id, osm_feature_t ftype)
{
    for( const TagWriteAction &kv: tags.actions_)
        addTag(id, ftype, kv.key_, kv.val_, kv.zoom_min_, kv.zoom_max_) ;
    return true ;
}

bool GeoDatabase::addTag(osm_id_t id, osm_feature_t ftype, const string &key, const string &val, uint8_t zmin, uint8_t zmax)
{
    switch ( ftype ) {
    case osm_node_t:
        ntags_.emplace(std::make_pair(id, Tag{key, val, zmin, zmax})) ;
        break ;
    case osm_way_t:
        wtags_.emplace(std::make_pair(id, Tag{key, val, zmin, zmax})) ;
        break ;
    case osm_relation_t:
        rtags_.emplace(std::make_pair(id, Tag{key, val, zmin, zmax})) ;
        break ;

    }

}

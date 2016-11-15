#include "osm_processor.hpp"

#include <spatialite.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <shapefil.h>
#include <iomanip>
#include <iostream>

#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/util/IllegalArgumentException.h>

using namespace std ;
BBox OSMProcessor::getBoundingBoxFromGeometries() {
    return geodb_.box() ;
}

bool OSMProcessor::processOsmFile(const string &osm_file, TagFilter &cfg)
{
    using namespace tag_filter ;

    // read files from memory and write to spatialite database

    uint8_t zmin, zmax ;

    try {

        OSM::DocumentReader reader ;

        cout << "Parsing file: " << osm_file << endl ;

        if ( !reader.read(osm_file) ) {
            cerr << "Error reading from " << osm_file << endl ;
            return false ;
        }

        // POIs

        reader.forAllNodes([&] ( const OSM::Node &node )
        {
            if ( node.tags_.empty() ) return ;

            TagFilterContext ctx(node, &reader) ;
            if ( !cfg.match(ctx, zmin, zmax) ) return ;

            geodb_.addPointGeometry(node, zmin, zmax) ;
            geodb_.addTags(ctx.tw_, node.id_, osm_node_t) ;
        }) ;

        // relations of type route, merge ways into chunks


        reader.forAllRelations([&] ( const OSM::Relation &relation )
        {
            string rel_type = relation.tags_.get("type") ;

            if ( rel_type == "route" ) {

                TagFilterContext ctx(relation.tags_, relation.id_,  TagFilterContext::Relation) ;
                if ( !cfg.match(ctx, zmin, zmax) ) return ;

                vector<OSM::Way> chunks ;
                if ( !reader.makeWaysFromRelation(relation, chunks) ) return ;

                if ( !chunks.empty() ) {
                    geodb_.addMultiLineGeometry(reader, chunks, relation.id_, osm_relation_t, zmin, zmax) ;
                    geodb_.addTags(ctx.tw_, relation.id_, osm_relation_t) ;
                }
            }
            else if ( rel_type == "multipolygon"  ) {

                TagFilterContext ctx(relation.tags_, relation.id_,  TagFilterContext::Way) ;
                if ( !cfg.match(ctx, zmin, zmax) ) return ;

                OSM::Polygon polygon ;
                if ( !reader.makePolygonsFromRelation(relation, polygon) ) return ;

                if ( !polygon.rings_.empty() ) {
                    geodb_.addPolygonGeometry(reader, polygon, relation.id_, osm_relation_t, zmin, zmax) ;
                    geodb_.addTags(ctx.tw_, relation.id_, osm_relation_t) ;
                }

            }
        }) ;

        // ways

        reader.forAllWays([&] ( const OSM::Way &way )
        {
            if ( way.tags_.empty() ) return ;

            // match feature with filter rules

            TagFilterContext ctx(way, &reader) ;
            if ( !cfg.match(ctx, zmin, zmax) ) return ;

            // deal with closed ways, potential polygon geometries (areas) are those indicated by area tag or those other than highway, barrier and contour

            if ( ( way.nodes_.front() == way.nodes_.back() ) &&
                 ( way.tags_.get("area") == "yes" ||
                   ( !way.tags_.contains("highway") &&
                     !way.tags_.contains("barrier") &&
                     !way.tags_.contains("contour") ) ) ) {

                OSM::Polygon poly ;

                OSM::Ring ring ;
                ring.nodes_.insert(ring.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
                poly.rings_.push_back(ring) ;

                geodb_.addPolygonGeometry(reader, poly, way.id_, osm_way_t, zmin, zmax) ;
                geodb_.addTags(ctx.tw_, way.id_, osm_way_t) ;
            }
            else {
                geodb_.addLineGeometry(reader, way, zmin, zmax) ;
                geodb_.addTags(ctx.tw_, way.id_, osm_way_t) ;
            }

        }) ;

        return true ;
    }
    catch ( LuaException &e ) {
        cerr << e.what() << endl ;
        return false ;
    }
}


struct DBField {
    DBFFieldType type_ ;
    int width_, precision_ ;
    char name_[12] ;
};

namespace fs = boost::filesystem ;

bool OSMProcessor::processLandPolygon(const string &shp_file, const BBox &clip_box)
{
    using namespace geos::geom ;

    PrecisionModel pm;
    GeometryFactory factory(&pm, 4326) ;

    cout << "Processing land polygon" << endl ;

    if ( !fs::exists(shp_file) ) {
        cerr << "cannot read land polygon: " << shp_file << endl ;
        return false ;
    }

    fs::path dir = fs::path(shp_file).parent_path() ;
    string file_name_prefix = fs::path(shp_file).stem().native() ;

    // open database file

    DBFHandle db_handle = DBFOpen( ( dir / (file_name_prefix + ".dbf") ).native().c_str(), "rb" );

    if ( !db_handle ) return false ;

    uint n_fields = DBFGetFieldCount(db_handle) ;

    vector<DBField> field_info ;

    for( uint i = 0 ; i<n_fields ; i++ ) {
        DBField fr ;

        fr.type_ = DBFGetFieldInfo( db_handle, i, fr.name_, &fr.width_, &fr.precision_);
        field_info.push_back(std::move(fr)) ;
    }

    SHPHandle shp_handle = SHPOpen( ( dir / file_name_prefix ).native().c_str(), "rb");

    int shp_entities, shp_geomtype ;
    SHPGetInfo( shp_handle, &shp_entities, &shp_geomtype, 0, 0) ;

    if ( shp_geomtype != SHPT_POLYGON ) {
        cerr << "no polygons found while reading land Shapefile: " << shp_file << endl ;
        return false ;
    }

    CoordinateArraySequence *cl = new CoordinateArraySequence();

    cl->add(Coordinate(clip_box.minx_, clip_box.miny_));
    cl->add(Coordinate(clip_box.maxx_, clip_box.miny_));
    cl->add(Coordinate(clip_box.maxx_, clip_box.maxy_));
    cl->add(Coordinate(clip_box.minx_, clip_box.maxy_));
    cl->add(Coordinate(clip_box.minx_, clip_box.miny_));

    LinearRing *shell = factory.createLinearRing(cl);
    std::unique_ptr<Polygon> clip_geom(factory.createPolygon(shell, nullptr)) ;

    osm_id_t id = 10000000000LL ;

    for( uint i=0 ; i<shp_entities ; i++ ) {

        SHPObject *obj = SHPReadObject( shp_handle, i );
        double *vx = obj->padfX, *vy = obj->padfY ;

        try {
            LinearRing *outer = nullptr ;
            vector<Geometry *> *inner = new vector<Geometry *>() ;

            for( int r = 0 ; r<obj->nParts ; r++ ) {
                uint count, last ;
                if ( r+1 < obj->nParts ) last = obj->panPartStart[r+1] ;
                else last = obj->nVertices ;

                count = last - obj->panPartStart[r] ;

                CoordinateArraySequence *cl = new CoordinateArraySequence();

                for (uint j=0 ; j<count ; j++) {
                    cl->add(Coordinate(*vx++, *vy++));
                }

                LinearRing *ring = factory.createLinearRing(cl);

                if ( outer ) {
                    inner->push_back((Geometry *)ring) ;
                }
                else {
                    outer = ring ;
                }

            }

            std::unique_ptr<Geometry> poly((Geometry *)factory.createPolygon(outer, inner)) ;
            std::unique_ptr<Geometry> geom(poly->intersection(clip_geom.get())) ;

            if ( geom ) {
                geodb_.addGeometry(geom.get(), id, osm_way_t, 0, 255) ;
                geodb_.addTag(id, osm_way_t, "natural", "nosea") ;
                ++id ;
            }

            SHPDestroyObject(obj) ;
        }
        catch (geos::util::IllegalArgumentException const& e)
        {
            cerr << e.what() << endl ;

        }
    }

    geodb_.addBoxGeometry(clip_box, id) ;
    geodb_.addTag(id, osm_way_t, "natural", "sea") ;
    geodb_.addTag(id, osm_way_t, "level", "-5") ;


    return true ;
}

bool OSMProcessor::addDefaultLandPolygon(const BBox &clip_box)
{
    osm_id_t id = 10000000000LL ; // we have to take sure that somwhow this unique

    geodb_.addBoxGeometry(clip_box, id) ;
    geodb_.addTag(id, osm_way_t, "natural", "nosea") ;
}

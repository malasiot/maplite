#include "pugixml.hpp"
#include "gpx_reader.hpp"

#include <iostream>
#include <boost/optional.hpp>

using namespace std ;
using namespace geojson ;

bool gpx_load(const pugi::xml_document &doc, FeatureCollection &col) {

    pugi::xml_node root = doc.child("gpx") ;

    if ( root.empty() ) return false ;

    // we do not parse metadata at the moment since geojson does not support them

    for( pugi::xml_node wp: root.children("wpt") ) {
        float lat = wp.attribute("lat").as_float() ;
        float lon = wp.attribute("lon").as_float() ;

        Feature feature ;

        pugi::xml_node elnode = wp.child("ele") ;
        if ( !elnode.empty() ) feature.properties_.add("ele", elnode.text().as_string()) ;

        pugi::xml_node name_node = wp.child("name") ;
        if ( !name_node.empty() ) feature.properties_.add("name", name_node.text().as_string()) ;

        feature.geometry_.reset(new PointGeometry(lon, lat)) ;

        col.features_.push_back(std::move(feature)) ;
    }

    for( pugi::xml_node trk: root.children("trk") ) {

        Feature feature ;

        pugi::xml_node name_node = trk.child("name") ;
        if ( !name_node.empty() ) feature.properties_.add("name", name_node.text().as_string()) ;

        std::shared_ptr<GeometryCollection> gcol(new GeometryCollection) ;

        for( pugi::xml_node trkseg: trk.children("trkseg") ) {

            PointList pl ;
            for( pugi::xml_node &trkpt: trkseg.children("trkpt") ) {
                float lat = trkpt.attribute("lat").as_float() ;
                float lon = trkpt.attribute("lon").as_float() ;
                float ele = trkpt.attribute("ele").as_float() ;

                pl.push_back({lon, lat, ele}) ;
            }

            gcol->geometries_.push_back(std::make_shared<LineStringGeometry>(pl)) ;
        }

        feature.geometry_ = gcol ;

        col.features_.push_back(std::move(feature)) ;
    }

    return true ;
}



bool GPXReader::load_from_file(const std::string &file_name, FeatureCollection &col)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());

    if ( !result ) {
        cerr << "GPX file parsed with errors\n";
        cerr << "error parsing file (" <<
                file_name << "): " << result.description() << "\n";
        return false ;
    }
    else
        return gpx_load(doc, col) ;

}

bool GPXReader::load_from_string(const std::string &bytes, FeatureCollection &col)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(bytes.c_str());

    if ( !result ) {
        cerr << "GPX content parsed with errors\n";
        cerr << result.description() << "\n";
        return false ;
    }
    else
        return gpx_load(doc, col) ;
}

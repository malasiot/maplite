#include "feature_collection.hpp"
#include <iomanip>

using namespace std ;

namespace geojson {

string PointGeometry::toGeoJSON() const {
    stringstream strm ;
    strm << "{ \"type\":\"Point\",\"coordinates\":[" << coordinates_[0] << "," << coordinates_[1] << "," << coordinates_[2] <<  "]}" << std::setprecision(12) ;
    return strm.str() ;
}

string LineStringGeometry::toGeoJSON() const {
    stringstream strm ;
    strm << "{ \"type\":\"LineString\",\"coordinates\":[" << std::setprecision(12)  ;

    bool first = true ;
    for ( auto c: coordinates_) {
        if ( !first ) strm << ',' ;
        strm << "[" << c[0] << "," << c[1] << ',' << c[2] << "]" ;
        first = false ;
    }
    strm << "]}" ;
    return strm.str() ;
}

string PolygonGeometry::toGeoJSON() const {
    stringstream strm ;
    strm << "{ \"type\":\"Polygon\",\"coordinates\":[" << std::setprecision(12)  ;

    bool first_list = true ;
    for ( auto c_list: coordinates_) {
        if ( !first_list ) strm << ',' ;
        strm << "[" ;
        bool first = true ;
        for ( auto c: c_list) {
            if ( !first ) strm << ',' ;
            strm << "[" << c[0] << "," << c[1] << "," << c[2] << "]" ;
            first = false ;
        }
        strm << "]" ;
        first_list = false ;
    }
    strm << "]}" ;
    return strm.str() ;
}

string GeometryCollection::toGeoJSON() const {
    stringstream strm ;
    strm << "{ \"type\":\"GeometryCollection\",\"geometries\":[" ;

    bool first = true ;
    for( auto c: geometries_) {
        if ( !first ) strm << ',' ;
        strm << c->toGeoJSON() ;
        first = false ;
    }
    strm << "]}" ;

    return strm.str() ;
}


string FeatureCollection::toGeoJSON() const {
    stringstream strm ;
    strm << "{\"type\":\"FeatureCollection\",\"features\":[" ;

    bool first = true ;
    for( auto c: features_) {
        if ( !first ) strm << ',' ;
        strm << c.toGeoJSON() ;
        first = false ;
    }
    strm << "]}" ;

    return strm.str() ;
}

static std::string escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}

string Feature::toGeoJSON() const {
    stringstream strm ;
    strm << "{\"properties\":{" ;

    bool first = true ;
    for( auto c: properties_) {
        if ( !first ) strm << ',' ;
        strm << '"' << c.first << '"' << ':' << '"' << c.second << '"';
        first = false ;
    }
    strm << "},\"geometry\":" << geometry_->toGeoJSON() ;
    strm << "}" ;

    return strm.str() ;
}

}

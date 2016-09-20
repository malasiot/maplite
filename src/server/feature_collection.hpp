#ifndef __FEATURE_COLLECTION_HPP__
#define __FEATURE_COLLECTION_HPP__

#include <string>
#include <vector>
#include <array>

#include "dictionary.hpp"

namespace geojson {

struct FeatureGeometry {
    virtual std::string toGeoJSON() const = 0;
};

struct Feature {
    std::string id_ ;
    Dictionary properties_ ;
    std::shared_ptr<FeatureGeometry> geometry_ ;
    std::string toGeoJSON() const ;
};

using Point = std::array<float, 3> ;
using PointList = std::vector<Point> ;

struct PointGeometry: public FeatureGeometry {
    PointGeometry(float x, float y, float z=0): coordinates_{x, y, z} {}
    PointGeometry(const Point &p): coordinates_(p) {}

    Point coordinates_ ;
    std::string toGeoJSON() const ;
};

struct LineStringGeometry: public FeatureGeometry {
    LineStringGeometry(const PointList &p): coordinates_(p) {}
    PointList coordinates_ ;
    std::string toGeoJSON() const ;
};

struct MultiPointGeometry: public FeatureGeometry {
    PointList coordinates_ ;
    std::string toGeoJSON() const ;
};

struct PolygonGeometry: public FeatureGeometry {
    std::vector<PointList> coordinates_ ;
    std::string toGeoJSON() const ;
};

struct MyltiPolygonGeometry: public FeatureGeometry {
    std::vector< std::vector<PointList> > coordinates_ ;
    std::string toGeoJSON() const ;
};

struct GeometryCollection: public FeatureGeometry {
    std::vector< std::shared_ptr<FeatureGeometry> > geometries_ ;
    std::string toGeoJSON() const ;
};

struct FeatureCollection {

    std::vector<Feature> features_ ;
    std::string toGeoJSON() const ;
};

}

#endif

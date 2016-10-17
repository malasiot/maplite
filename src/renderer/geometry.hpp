#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cstdint>
#include <string>
#include <sstream>
#include <boost/program_options.hpp>

struct LatLon {
    LatLon() = default;
    LatLon(double lat, double lon): lat_(lat), lon_(lon) {}

    bool fromString(const std::string &s) {
        std::istringstream strm(s) ;
        strm >> lat_ >> lon_ ;
        return (bool)strm ;
    }

    double lat_, lon_ ;
};


struct Coord {
    Coord(): x_(0), y_(0) {}
    Coord(double x, double y): x_(x), y_(y) {}

    double x_, y_ ;
};

struct BBox {

    BBox() = default ;
    BBox( double min_x, double min_y, double max_x, double max_y ):
        minx_(min_x), miny_(min_y), maxx_(max_x), maxy_(max_y) {}

    bool fromString(const std::string &s) {
        std::istringstream strm(s) ;
        strm >> minx_ >> miny_ >> maxx_ >> maxy_ ;
        return (bool)strm ;
    }

    double width() const { return maxx_ - minx_ ; }
    double height() const { return maxy_ - miny_ ; }

    bool contains(double x, double y) const { return ( x >= minx_ && y >= miny_ && x < maxx_ && y < maxy_ ) ; }

    bool intersects(const BBox &other) const {
        return (minx_ < other.maxx_ && maxx_ > other.minx_ && miny_ < other.maxy_ && maxy_ > other.miny_) ;
    }

    BBox intersection(const BBox &other) const {
        return BBox(std::max(minx_, other.minx_), std::max(miny_, other.miny_), std::min(maxx_, other.maxx_), std::max(maxy_, other.maxy_)) ;
    }

    double minx_, miny_, maxx_, maxy_ ;
};


inline void validate(boost::any& v,
              std::vector<std::string> const& values,
              BBox * /* target_type */,
              int)
{
    using namespace boost::program_options;

    validators::check_first_occurrence(v);

    std::string const& s = validators::get_single_string(values);

    BBox box ;
    if ( box.fromString(s) ) v = boost::any(box);
    else throw validation_error(validation_error::invalid_option_value);
}

inline void validate(boost::any& v,
              std::vector<std::string> const& values,
              LatLon * /* target_type */,
              int)
{
    using namespace boost::program_options;

    validators::check_first_occurrence(v);

    std::string const& s = validators::get_single_string(values);

    LatLon coords ;
    if ( coords.fromString(s) ) v = boost::any(coords);
    else throw validation_error(validation_error::invalid_option_value);
}

#endif

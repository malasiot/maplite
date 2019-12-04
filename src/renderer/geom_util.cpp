#include "tms.hpp"
#include "geometry.hpp"

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cairo.h>

using namespace std ;

void latlon_to_tms(const std::vector<std::vector<LatLon>> &latlon,  std::vector<std::vector<Coord>> &coords) {

    coords.resize(latlon.size()) ;

    for( uint i=0 ; i<latlon.size() ; i++ ) {
        for( uint j=0 ; j<latlon[i].size() ; j++ ) {
            const LatLon &c = latlon[i][j] ;
            double mx, my ;
            tms::latlonToMeters(c.lat_, c.lon_, mx, my) ;
            coords[i].emplace_back(mx, my) ;
        }
    }
}

static const double sample_near_boundaries_offset = 10 ;

struct ParametricLineRing
{
    ParametricLineRing(const vector<Coord> &coords, const cairo_matrix_t &cmm) ;

    double length() const { return par_.back() ; }

    bool sample(double p, Coord &sample, double &angle ) ;

    vector<double> par_, angle_ ;
    const vector<Coord> &coords_ ;
    const cairo_matrix_t &cmm_ ;
};

// compute arc length coordinates

ParametricLineRing::ParametricLineRing(const vector<Coord> &coords, const cairo_matrix_t &cmm):
    coords_(coords),  cmm_(cmm)
{
    double x0, y0, x1, y1, x0_, y0_  ;

    double sl = 0.0, tl = 0.0 ;

    x1 = coords[0].x_ ; y1 = coords[0].y_ ;

    par_.push_back(0) ;

    for( int j = 1 ; j < coords.size() ; j++ )
    {
        x0 = x1 ; y0 = y1 ;

        x1 = coords[j].x_ ;
        y1 = coords[j].y_ ;

        double dx = x1 - x0, dy = y1 - y0 ;

        cairo_matrix_transform_distance(&cmm, &dx, &dy);

        sl = sqrt( dx * dx + dy * dy ) ;
        tl += sl ;

        par_.push_back(tl) ;
        angle_.push_back(atan2(dy, dx)) ;
    }

}

bool ParametricLineRing::sample(double pp, Coord &sample, double &angle)
{
    auto pos = std::equal_range(par_.begin(), par_.end(), pp) ;

    int i = std::distance(par_.begin(), pos.second) ;
    int pi = i-1 ;

    double ptl = par_[pi] ;
    double tl = par_[i] ;

    double x0, y0, x1, y1 ;

    x0 = coords_[pi].x_ ;
    y0 = coords_[pi].y_ ;
    x1 = coords_[i].x_ ;
    y1 = coords_[i].y_ ;

    double h = (pp - ptl)/(tl - ptl) ;
    double x = ( 1 - h ) * x0 + h * x1 ;
    double y = ( 1 - h ) * y0 + h * y1 ;
    angle = angle_[pi] ;

    sample = {x, y} ;

    return true ;

}

static void sample_polygon(const vector<Coord> &coords, const cairo_matrix_t &cmm, float gap, float initial_gap, float box_len,
                           vector<Coord> &samples, vector<double> &angles, bool fix_angle)
{
    static const double angle_threshold = M_PI/20 ;

    ParametricLineRing parp(coords, cmm) ;

    double length = parp.length() ;

    if ( gap == 0.0 )
    {
        initial_gap = length * 0.5 - initial_gap ;
        gap = length ;
        if ( initial_gap < 0) return ;
    }

    gap = std::max(gap, box_len) ;

    for( double cp = initial_gap ; cp <= length ; cp += gap )
    {
        double cp_p = cp - box_len/2 ;
        double cp_n = cp + box_len/2 ;
        if ( cp_p < sample_near_boundaries_offset || cp_n > length - sample_near_boundaries_offset ) continue ;

        Coord sample, sample_p, sample_n ;
        double angle, angle_p, angle_n ;

        parp.sample(cp, sample, angle) ;
        parp.sample(cp_p, sample_p, angle_p) ;
        parp.sample(cp_n, sample_n, angle_n) ;

        if ( fabs(angle_p - angle) > angle_threshold ) continue ;
        if ( fabs(angle_n - angle) > angle_threshold ) continue ;

        if ( fix_angle && ( angle < -M_PI/2 || angle > M_PI/2 ) )
            angle = angle + M_PI ;

        samples.push_back(sample) ;
        angles.push_back(angle) ;
    }
}

void sample_linear_geometry(
                    const vector<vector<Coord>> geom,
                    const cairo_matrix_t &cmm,
                    float gap,
                    float initial_gap,
                    float box_len,
                    bool fix_angle,
                    std::vector<Coord> &samples,
                    std::vector<double> &angles
)
{
    for( uint i=0 ; i<geom.size() ; i++ ) {

        const vector<Coord> &ring = geom[i] ;

        for( uint j=0 ; j<ring.size() ; j++ ) {
            sample_polygon(ring, cmm, gap, initial_gap, box_len, samples, angles, fix_angle) ;
        }
    }

}

#ifdef _WIN32
#define GEOS_INLINE
#endif

#include <geos/geom/LineString.h>
#include <geos/operation/buffer/OffsetCurveBuilder.h>
#include <geos/opBuffer.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/buffer/BufferBuilder.h>

void offset_geometry(const vector<vector<Coord>> &geom, double offset, vector<vector<Coord>> &res )
{
    using namespace geos::operation::buffer ;
    using namespace geos::geom ;

    PrecisionModel pm(PrecisionModel::FLOATING) ;

    GeometryFactory::unique_ptr gfactory = GeometryFactory::create(&pm) ;

    res.resize(geom.size()) ;

    for( uint j=0 ; j<geom.size() ; j++ ) {
        CoordinateSequence *cl = new CoordinateArraySequence();

        for( uint i=0 ; i<geom[j].size() ; i++ ) {
            const Coord &c = geom[j][i] ;
            cl->add(Coordinate(c.x_, c.y_)) ;
        }

        LineString *ls = gfactory->createLineString(cl);

        BufferParameters bp(8, BufferParameters::CAP_ROUND, BufferParameters::JOIN_ROUND, 4) ;
        bp.setSingleSided(true);

        BufferBuilder bufBuilder( bp ) ;
        Geometry *bgeom = ( offset > 0 ) ? bufBuilder.bufferLineSingleSided( ls, offset, true) :
                                      bufBuilder.bufferLineSingleSided( ls, -offset, false) ;

        delete cl ;

        CoordinateSequence *seq = bgeom->getCoordinates() ;

        if ( seq->size() == 0 ) return ;

        res[j].resize(seq->size(), {0, 0}) ;

        for(int i=0 ; i<seq->size() ; i++ ) {
            const geos::geom::Coordinate &c = seq->getAt(i) ;
            res[j][i] = Coord(c.x, c.y) ;
        }

        delete bgeom ;
    }

}

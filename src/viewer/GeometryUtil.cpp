#include "GeometryUtil.h"

#ifdef _WIN32
#define GEOS_INLINE
#endif

#include <geos/geom/LineString.h>
#include <geos/operation/buffer/OffsetCurveBuilder.h>
#include <geos/opBuffer.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/buffer/BufferBuilder.h>
#include <algorithm>

using namespace std;

struct ParametricLineRing
{
    ParametricLineRing(int n, double *coords, bool closed, const cairo_matrix_t &cmm) ;

    double length() const { return par_.back() ; }

    bool sample(double p, vector<double> &samples, bool flip_angles = false) ;

    vector<double> par_, angle_ ;
    bool closed_ ;
    double *coords_ ;
    int n_ ;
    const cairo_matrix_t &cmm_ ;
};

// compute arc length coordinates

ParametricLineRing::ParametricLineRing(int n, double *coords, bool closed, const cairo_matrix_t &cmm):
    closed_(closed), coords_(coords), n_(n), cmm_(cmm)
{
    double *c = coords, x0, y0, x1, y1, x0_, y0_  ;

    double sl = 0.0, tl = 0.0, ptl = 0.0 ;
    int j=1 ;

    x1 = *c++ ; y1 = *c++ ;

    if ( closed )  {
        x0_ = x1 ;
        y0_ = y1 ;
    }

    par_.push_back(0) ;

    for( int j = 1 ; j < n + closed ; j++ )
    {
        x0 = x1 ; y0 = y1 ;

        if ( closed && j == n ) {
            x1 = x0_ ; y1 = y0_ ;
        }
        else {
            x1 = *c++ ; y1 = *c++ ;
        }

        double dx = x1 - x0, dy = y1 - y0 ;

        cairo_matrix_transform_distance(&cmm, &dx, &dy);

        sl = sqrt( dx * dx + dy * dy ) ;
        tl += sl ;

        par_.push_back(tl) ;
        angle_.push_back(atan2(dy, dx)) ;
    }

}

bool ParametricLineRing::sample(double pp, vector<double> &samples, bool flip_angles)
{
    typedef vector<double>::iterator dtype ;

    pair<dtype, dtype> pos = std::equal_range(par_.begin(), par_.end(), pp) ;

    int i = std::distance(par_.begin(), pos.second) ;
    int pi = i-1 ;

    double ptl = par_[pi] ;
    double tl = par_[i] ;

    double x0, y0, x1, y1 ;

    if ( i == n_-1 && closed_ )
    {
        double ptl =0, tl =0 ;

        x0 = coords_[2*n_-2] ;
        y0 = coords_[2*n_-1] ;
        x1 = coords_[0] ;
        y1 = coords_[1] ;
    }
    else
    {
        x0 = coords_[2*pi] ;
        y0 = coords_[2*pi+1] ;
        x1 = coords_[2*i] ;
        y1 = coords_[2*i+1] ;
    }

    double h = (pp - ptl)/(tl - ptl) ;
    double x = ( 1 - h ) * x0 + h * x1 ;
    double y = ( 1 - h ) * y0 + h * y1 ;
    double angle = angle_[pi] ;

    samples.push_back(x) ;
    samples.push_back(y) ;
    if ( flip_angles )
        samples.push_back(angle + M_PI) ;
    else
        samples.push_back(angle) ;


    return true ;

}

static const double sample_near_boundaries_offset = 20 ;

static void sample_polygon(int n, double *coords, bool closed, const cairo_matrix_t &cmm, float gap_, float initialGap_, const vector<double> &offsets, vector<double> &samples, bool fix_orientation)
{
    ParametricLineRing parp(n, coords, closed, cmm) ;

    double length = parp.length() ;

    if ( gap_ == 0.0 )
    {
        initialGap_ = length * 0.5 - initialGap_ ;
        gap_ = length ;
        if ( initialGap_ < 0) return ;
    }

    // also sample with reverse direction

    vector<double> roffsets ;

    for(int i=0 ; i<offsets.size() ; i++)
        roffsets.push_back(offsets.back() - offsets[i]) ;

    for( double cp = initialGap_ ; cp <= length ; cp += gap_ )
    {
        vector<double> samples_ ;
        bool clipped = false ;

        for(int i=0 ; i<offsets.size() ; i++ )
        {
            double pp = cp + offsets[i] ;

            if ( pp < sample_near_boundaries_offset || pp > length - sample_near_boundaries_offset )
                clipped = true ;

            parp.sample(pp, samples_, false ) ;
        }

        if ( clipped ) continue ;

        // test if the majority of samples are upside down

        int count = 0 ;
        for(int j=0 ; j<samples_.size() ; j+=3)
        {
            if ( samples_[j+2] < -M_PI/2 || samples_[j+2] > M_PI/2 ) count ++ ;
        }

        // in this case use the reverse sampling to place characters with the correct direction
        if ( fix_orientation && count > offsets.size()/2 )
        {
            samples_.clear() ;

            for(int i=0 ; i<roffsets.size() ; i++ )
            {
                double pp = cp + roffsets[i] ;

                if ( pp < sample_near_boundaries_offset || pp > length - sample_near_boundaries_offset )
                    clipped = true ;

                parp.sample(pp, samples_, true) ;
            }
        }

        if ( clipped ) continue ;
        // we append the samples only if the glyphs are contained completely with the length of the segment

        if ( samples_.size() == offsets.size() * 3 )
            samples.insert(samples.end(), samples_.begin(), samples_.end()) ;

    }
}



void sampleLinearGeometry(std::vector<double> &samples,
                    const gaiaGeomCollPtr geom_,
                    const cairo_matrix_t &cmm,
                    float gap_,
                    float initialGap_,
                    const std::vector<double> &offsets,
                    bool fix_orientation)
{
    for( gaiaLinestringPtr p = geom_->FirstLinestring ; p != NULL ; p = p->Next )
    {
        int n = p->Points ;
        double *coords = p->Coords ;

        sample_polygon(p->Points, p->Coords, false, cmm, gap_, initialGap_, offsets, samples, fix_orientation) ;
    }

    for( gaiaPolygonPtr p = geom_->FirstPolygon ; p != NULL ; p = p->Next )
    {
        gaiaRingPtr r = p->Exterior ;

        sample_polygon(r->Points, r->Coords, true, cmm, gap_, initialGap_, offsets, samples, fix_orientation) ;

        for( int j=0 ; j<p->NumInteriors ; j++ )
        {
            r = &p->Interiors[j] ;

            sample_polygon(r->Points, r->Coords, true, cmm, gap_, initialGap_, offsets, samples, fix_orientation) ;
        }

    }


}

void samplePointGeometry(std::vector<double> &samples, const gaiaGeomCollPtr geom_)
{
    for( gaiaPointPtr p = geom_->FirstPoint ; p != NULL ; p = p->Next )
    {
        samples.push_back(p->X) ;
        samples.push_back(p->Y) ;
    }

    for( gaiaLinestringPtr p = geom_->FirstLinestring ; p != NULL ; p = p->Next )
     {
         int n = p->Points ;
         double *coords = p->Coords ;
         cairo_matrix_t cmm ;
         cairo_matrix_init_identity(&cmm) ;

         vector<double> samples_ ;
         sample_polygon(p->Points, p->Coords, false, cmm, 0.0, 0.0, vector<double>(1, 0), samples_, true) ;

         samples.push_back(samples_[0]) ;
         samples.push_back(samples_[1]) ;
     }
     for( gaiaPolygonPtr p = geom_->FirstPolygon ; p != NULL ; p = p->Next )
     {
         gaiaRingPtr r = p->Exterior ;

         int n = r->Points ;
         double *coords = r->Coords, *c = coords ;
         double cX = 0.0, cY = 0.0 ;

         for(int i=0 ; i<n ; i++)
         {
             cX += *c++ ;
             cY += *c++ ;
         }

         samples.push_back(cX/n) ;
         samples.push_back(cY/n) ;
     }

}

// take the first line string of each geometry and determine if they have clockwise direction when merged

bool isClockwise(gaiaGeomColl *p1, gaiaGeomColl *p2)
{
    assert(p1->FirstLinestring && p2->FirstLinestring ) ;

    vector<double> px, py ;

    double *coords = p1->FirstLinestring->Coords ;

    for( int i=0 ; i<p1->FirstLinestring->Points ; i++ )
    {
        px.push_back(*coords++) ;
        py.push_back(*coords++) ;
    }

    coords = p2->FirstLinestring->Coords ;

    for( int i=0 ; i<p2->FirstLinestring->Points ; i++ )
    {
        px.push_back(*coords++) ;
        py.push_back(*coords++) ;
    }

    int n = px.size() ;
    int count = 0 ;

    for ( int i=0; i<n; i++ )
    {
        int j = (i + 1) % n;
        int k = (i + 2) % n;
        double z  = (px[j] - px[i]) * (py[k] - py[j]);
        z -= (py[j] - py[i]) * (px[k] - px[j]);

        if (z < 0) count--;
        else if (z > 0) count++;
    }

    return count < 0 ;
}



gaiaGeomCollPtr offsetGeometry(const gaiaGeomCollPtr geom_, double offset)
{
    using namespace geos::operation::buffer ;
    using namespace geos::geom ;

    //cairo_matrix_transform_distance(&cmm, &offset, &offset);

    PrecisionModel pm(PrecisionModel::FLOATING) ;

    GeometryFactory *gfactory = new GeometryFactory(&pm) ;

    gaiaGeomCollPtr resultGeom = gaiaAllocGeomColl() ;

    for( gaiaLinestringPtr p = geom_->FirstLinestring ; p != NULL ; p = p->Next )
    {
        int n = p->Points ;
        double *coords = p->Coords ;

        CoordinateSequence *cl = new CoordinateArraySequence();

        for( int i=0 ; i<n ; i++ )
             cl->add(Coordinate(*coords++, *coords++)) ;

        LineString *ls = gfactory->createLineString(cl);

        BufferParameters bp(8, BufferParameters::CAP_ROUND, BufferParameters::JOIN_ROUND, 4) ;
        bp.setSingleSided(true);

        BufferBuilder bufBuilder( bp ) ;
        Geometry *geom = ( offset > 0 ) ? bufBuilder.bufferLineSingleSided( ls, offset, true) :
                                          bufBuilder.bufferLineSingleSided( ls, -offset, false) ;

        delete cl ;

        CoordinateSequence *seq = geom->getCoordinates() ;

        if ( seq->size() == 0 ) continue ;

        gaiaLinestringPtr cls = gaiaAddLinestringToGeomColl(resultGeom, seq->size()) ;

        coords = cls->Coords ;

        for(int i=0 ; i<seq->size() ; i++ )
        {
            gaiaLineSetPoint(cls, i, seq->getAt(i).y, seq->getAt(i).x, 0, 0) ;
        }

        delete geom ;
    }

    delete gfactory ;

    return resultGeom ;
}

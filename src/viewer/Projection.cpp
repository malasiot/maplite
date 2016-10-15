#include "Projection.h"

#include <proj_api.h>
#include <cassert>

using namespace std ;

bool Projection::init(const string &pj_params_)
{
    proj_ctx_ = pj_ctx_alloc();
    proj_ = pj_init_plus_ctx(proj_ctx_, pj_params_.c_str());

    if ( !proj_ )
    {
        if (proj_ctx_) {
            pj_ctx_free(proj_ctx_);
            proj_ctx_ = 0;
        }
        return false ;
    }

    is_geographic_ = pj_is_latlong(proj_) ;

    return true ;
}

void Projection::forward(double x, double y, double &px, double &py) const
{
    assert(proj_) ;

    projUV p;
    p.u = x * DEG_TO_RAD;
    p.v = y * DEG_TO_RAD;
    p = pj_fwd(p ,proj_);
    px = p.u;
    py = p.v;

    if ( isGeographic() )
    {
        px *= RAD_TO_DEG;
        py *= RAD_TO_DEG;
    }
}

void Projection::forward(double &x, double &y) const
{
    assert(proj_) ;

    projUV p;
    p.u = x * DEG_TO_RAD;
    p.v = y * DEG_TO_RAD;
    p = pj_fwd(p ,proj_);
    x = p.u;
    y = p.v;

    if ( isGeographic() )
    {
        x *= RAD_TO_DEG;
        y *= RAD_TO_DEG;
    }
}

void Projection::inverse(double x, double y, double & px, double & py) const
{
    assert(proj_) ;

    if ( is_geographic_ )
    {
        x *= DEG_TO_RAD;
        y *= DEG_TO_RAD;
    }
    projUV p;
    p.u = x;
    p.v = y;
    p = pj_inv(p, proj_);
    px = RAD_TO_DEG * p.u;
    py = RAD_TO_DEG * p.v;
}

void Projection::inverse(double &x, double &y) const
{
    assert(proj_) ;

    if ( is_geographic_ )
    {
        x *= DEG_TO_RAD;
        y *= DEG_TO_RAD;
    }

    projUV p;
    p.u = x;
    p.v = y;
    p = pj_inv(p, proj_);
    x = RAD_TO_DEG * p.u;
    y = RAD_TO_DEG * p.v;
}


Projection::~Projection()
{
    if (proj_) pj_free(proj_);
    if (proj_ctx_) pj_ctx_free(proj_ctx_);
}

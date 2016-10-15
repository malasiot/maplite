#include "Renderer.h"

#include <string>
#include <boost/algorithm/string.hpp>

#include <cairo-ft.h>
#include <fontconfig/fcfreetype.h>

#include "svg/Rendering.h"
#include "ResourceCache.h"
#include "GeometryUtil.h"
#include "CollisionChecker.h"

using namespace sld ;
using namespace std ;


void Renderer::drawWellKnownShape(cairo_t *cr, const std::string &shapeName, double size)
{
    if ( shapeName == "square" )
        cairo_rectangle(cr, 0, 0, size, size) ;
    else if ( shapeName == "circle" )
        cairo_arc (cr, size/2.0, size/2.0, size/2.0, 0.0, 2*M_PI) ;
    else if ( shapeName == "triangle" )
    {
        cairo_move_to(cr, size/2.0, 0.0) ;
        cairo_line_to(cr, size, size) ;
        cairo_line_to(cr, 0, size) ;
        cairo_close_path(cr) ;
    }
    else if ( shapeName == "star" )
    {
        const double R = sqrt((25 - 11*sqrt(5))/10) ;
        const double rho = sqrt((5 - sqrt(5))/10) ;

        const double sc = sin(M_PI/5) ;
        const double cc =  cos(M_PI/5) ;
        const double sc2 = sin(M_PI/10) ;
        const double cc2 =  cos(M_PI/10) ;

        const double cosa = 0.30901699437494742410229341718282 ;
        const double sina = 0.95105651629515357211643933337938 ;
        const double cosb = 0.80901699437494742410229341718282 ;
        const double sinb = 0.58778525229247312916870595463907 ;

        double r1 = cosa/cosb ;

        cairo_save(cr) ;
        cairo_scale(cr, size/2.0, size/2.0) ;
        cairo_translate(cr, 1, 1) ;

        cairo_move_to(cr, 0, -1) ;
        cairo_line_to(cr, r1*sinb, -r1*cosb) ;
        cairo_line_to(cr, sina, -cosa) ;
        cairo_line_to(cr, r1*sina, r1*cosa) ;
        cairo_line_to(cr, sinb, cosb) ;
        cairo_line_to(cr, 0, r1) ;
        cairo_line_to(cr, -sinb, cosb) ;
        cairo_line_to(cr, -r1*sina, r1*cosa) ;
        cairo_line_to(cr, -sina, -cosa) ;
        cairo_line_to(cr, -r1*sinb, -r1*cosb) ;
        cairo_close_path(cr) ;
        cairo_restore(cr) ;
    }
    else if ( shapeName == "cross" )
    {
        cairo_save(cr) ;
        cairo_scale(cr, size/2.0, size/2.0) ;
        cairo_translate(cr, 1, 1) ;

        cairo_move_to(cr, 0, -1) ;
        cairo_line_to(cr, 0, 1) ;
        cairo_move_to(cr, -1, 0) ;
        cairo_line_to(cr, 1, 0) ;
        cairo_restore(cr) ;
    }
    else if ( shapeName == "x" )
    {
        cairo_save(cr) ;
        cairo_scale(cr, size/2.0, size/2.0) ;
        cairo_translate(cr, 1, 1) ;
        cairo_move_to(cr, -1, -1) ;
        cairo_line_to(cr, 1, 1) ;
        cairo_move_to(cr, 1, -1) ;
        cairo_line_to(cr, -1, 1) ;
        cairo_restore(cr) ;
    }
    else if ( shapeName == "hline" )
    {
        cairo_save(cr) ;
        cairo_scale(cr, size/2.0, size/2.0) ;
        cairo_translate(cr, 1, 1) ;
        cairo_move_to(cr, -1, 0) ;
        cairo_line_to(cr, 1, 0) ;
        cairo_restore(cr) ;
    }


}

struct read_png_context {

    read_png_context(const string &str): str_(str), rl_(0) {}

    const string &str_ ;
    size_t rl_ ;
};

cairo_status_t read_png_string(void *closure, unsigned char *data, unsigned int length)
{
    read_png_context *ctx = (read_png_context *)closure ;

    const char *p = ctx->str_.c_str() + ctx->rl_ ;
    ctx->rl_ += length ;

    strncpy((char *)data, p, length) ;

    return CAIRO_STATUS_SUCCESS  ;
}

cairo_surface_t *Renderer::renderGraphic(cairo_t *cr, const Feature &f, const Graphic &g, double scale, cairo_rectangle_t &rect)
{
    double size = 0.0 ;

    g.size.eval(f).toNumber(size) ;

    if ( g.mark ) {

        if ( size == 0 ) size = 16 ;

        const Mark &mark = *g.mark ;

        double sw = 0.0 ;
        if ( mark.stroke ) mark.stroke->strokeWidth.eval(f).toNumber(sw) ;

        const double extra = 2 ;
        rect.x = 0 ;
        rect.y = 0 ;
        rect.width = size + sw + sw + extra + extra ;
        rect.height = size + sw + sw + extra + extra ;

        cairo_surface_t *rs = cairo_recording_surface_create( CAIRO_CONTENT_COLOR_ALPHA, &rect) ;

        cairo_t *ctx = cairo_create(rs) ;

        cairo_save(ctx) ;
        cairo_translate(ctx, sw + extra, sw + extra) ;
        drawWellKnownShape(ctx, mark.wellKnownName, size) ;
        cairo_restore(ctx) ;

        if ( mark.fill )
        {
            applySimpleFill(ctx, f, *mark.fill, 1.0, 1.0) ;
            cairo_fill_preserve(ctx) ;
        }

        if ( mark.stroke )
        {
            applySimpleStroke(ctx, f, *mark.stroke, 1.0, 1.0) ;
            cairo_stroke(ctx) ;
        }

        cairo_surface_flush(rs) ;
        cairo_destroy(ctx) ;

        return rs ;
    }
    else if ( g.externalGraphic )
    {
        string format = g.externalGraphic->format ;

        string href = g.externalGraphic->href ;

        string data = mf.readResourceFile("styles/" + href) ;

        if ( data.empty() ) return 0 ;

        if ( format == "image/png" )
        {
            cairo_surface_t *is = 0 ;

            ResourceCache::Data cachedData ;

            if ( cache->find(href, cachedData) )
            {
                is = cachedData.image_surf_ ;
                cachedData.image_surf_ = 0 ;

            }
            else {
                read_png_context ctx(data) ;
                is = cairo_image_surface_create_from_png_stream(read_png_string, &ctx) ;
                if ( cairo_surface_status(is) != CAIRO_STATUS_SUCCESS ) return 0 ;
                cachedData.image_surf_ = is ;
                cache->save(href, cachedData) ;
            }

            if ( is )
            {
                double orig_width = cairo_image_surface_get_width(is) ;
                double orig_height = cairo_image_surface_get_height(is) ;

                double scale = ( size > 0 ) ? size/orig_height : 1.0 ;

                rect.x = 0 ;
                rect.y = 0 ;
                rect.width = orig_width * scale ;
                rect.height = orig_height * scale ;

                cairo_surface_t *rs = cairo_recording_surface_create( CAIRO_CONTENT_COLOR_ALPHA, &rect) ;

                cairo_t *ctx = cairo_create(rs) ;

                cairo_save(ctx) ;
                cairo_scale(ctx, scale, scale) ;

                cairo_set_source_surface(ctx, is, 0, 0) ;

                cairo_paint(ctx) ;
                cairo_restore(ctx) ;

                cairo_destroy(ctx) ;

                return rs ;
            }
            else return 0 ;

        }
        else if ( format == "image/svg+xml" )
        {
            if ( size == 0 ) size = 16 ;

            svg::DocumentInstance *doc = 0 ;
            ResourceCache::Data cachedData ;

            if ( cache->find(href, cachedData) )
                doc = cachedData.svg_doc_ ;
            else {
                doc = new  svg::DocumentInstance ;

                istringstream strm(data) ;
                if ( !doc->load(strm) ) {
                    delete doc ;
                    return 0;
                }
                cachedData.svg_doc_ = doc ;
                cache->save(href, cachedData) ;
            }

            rect.x = 0 ;
            rect.y = 0 ;
            rect.width  = size ;
            rect.height = size ;

            cairo_surface_t *rs = cairo_recording_surface_create( CAIRO_CONTENT_COLOR_ALPHA, &rect) ;

            cairo_t *ctx = cairo_create(rs) ;

            doc->renderToTarget(ctx, 0, 0, size, 96) ;

            cairo_destroy(ctx) ;

            return rs ;

        }
    }

    return 0 ;

}

extern double clip_percent (double percent) ;

void Renderer::renderPointsGraphic(RenderingContext *ctx, const FeatureCollection &col, const sld::Graphic &graphic)
{
    cairo_t *cr = ctx->cr ;

    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;

        cairo_rectangle_t extents ;
        cairo_surface_t *surface_ = renderGraphic(cr, f, graphic, ctx->scale, extents) ;

        if ( !surface_ ) continue ;

        //cairo_surface_write_to_png(surface_, "/tmp/surf.png") ;

        vector<double> pts ;
        samplePointGeometry(pts, f.geom);

        double anchorPointX = 0.5 ;
        double anchorPointY = 0.5 ;
        double displacementX = 0 ;
        double displacementY = 0 ;
        double rotation = 0.0 ;
        double opacity = 1;

        graphic.anchorPointX.eval(f).toNumber(anchorPointX) ;
        graphic.anchorPointY.eval(f).toNumber(anchorPointY) ;
        graphic.displacementX.eval(f).toNumber(displacementX) ;
        graphic.displacementY.eval(f).toNumber(displacementY) ;
        graphic.rotation.eval(f).toNumber(rotation) ;
        graphic.opacity.eval(f).toNumber(opacity) ;

        double ofx =  extents.width * anchorPointX ;
        double ofy =  extents.height * anchorPointY ;

        opacity = clip_percent(opacity) ;

        for( int j=0 ; j<pts.size() ; j+=2 )
        {
            double px = pts[j], py = pts[j+1] ;

            cairo_matrix_transform_point(&ctx->cmm, &px, &py) ;

            if ( ctx->colc->addLabelBox(px + displacementX, py + displacementY, rotation * M_PI/180.0, extents.width, extents.height) )
            {
                cairo_save(cr) ;

                cairo_translate(cr, displacementX, displacementY) ;
                cairo_translate(cr, px, py) ;
                cairo_rotate(cr, rotation * M_PI/180.0) ;
                cairo_translate(cr, -ofx, ofy - extents.height) ;

                cairo_set_source_surface (cr, surface_, 0, 0);
                if ( opacity == 1.0 ) cairo_paint(cr) ;
                else cairo_paint_with_alpha(cr, opacity) ;

                cairo_restore(cr) ;
/*
            cairo_rectangle(cr, px-1, py-1, 3, 3);
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_stroke(cr) ;
            */
            }
        }

        cairo_surface_destroy(surface_) ;


    }



}

#include "Renderer.h"
#include "GeometryUtil.h"
#include "CollisionChecker.h"

#include <cairo.h>

using namespace std ;

double clip_percent (double percent)
{
  if ( percent > 1.0 ) return 1.0;
  else if ( percent <= 0.) return 0.0 ;
  return percent ;
}


void cairo_path_from_geometry(cairo_t *cr, const gaiaGeomCollPtr geom_)
{
    for( gaiaLinestringPtr p = geom_->FirstLinestring ; p != NULL ; p = p->Next )
    {
        int n = p->Points ;
        double *coords = p->Coords, *c = coords, x0, y0  ;

        for(int j=0 ; j<n ; j++)
        {
            x0 = *c++ ; y0 = *c++ ;

            if ( j == 0 )
                cairo_move_to(cr, x0, y0) ;
            else
                cairo_line_to(cr, x0, y0) ;
        }
    }

    for( gaiaPolygonPtr p = geom_->FirstPolygon ; p != NULL ; p = p->Next )
    {

        gaiaRingPtr r = p->Exterior ;

        int n = r->Points ;
        double *coords = r->Coords, *c = coords, x0, y0 ;

        for(int j=0 ; j<n ; j++)
        {
            x0 = *c++ ; y0 = *c++ ;

            if ( j == 0 )
                cairo_move_to(cr, x0, y0) ;
            else
                cairo_line_to(cr, x0, y0) ;

        }

        cairo_close_path(cr) ;

        for( int j=0 ; j<p->NumInteriors ; j++ )
        {
            r = &p->Interiors[j] ;

            n = r->Points ;
            c = coords = r->Coords  ;

            for(int k=0 ; k<n ; k++)
            {
                x0 = *c++ ; y0 = *c++ ;

                if ( k == 0 )
                    cairo_move_to(cr, x0, y0) ;
                else
                    cairo_line_to(cr, x0, y0) ;
            }

            cairo_close_path(cr) ;
        }

    }
}

using namespace sld ;

void Renderer::applySimpleStroke(cairo_t *cr, const Feature &f, const Stroke &stroke_, double layerOpacity, double scale)
{
    unsigned int strokeColor = 0 ;
    double strokeWidth = 1.0 ;
    double strokeOpacity = 1.0 ;
    double strokeDashOffset = 0 ;

    vector<double> strokeDashArray ;
    string strokeLineCap = "butt";
    string strokeLineJoin = "mitre";

    stroke_.stroke.eval(f).toColor(strokeColor) ;
    stroke_.strokeWidth.eval(f).toNumber(strokeWidth) ;
    stroke_.strokeOpacity.eval(f).toNumber(strokeOpacity) ;
    stroke_.strokeDashOffset.eval(f).toNumber(strokeDashOffset) ;
    stroke_.strokeLineCap.eval(f).toString(strokeLineCap) ;
    stroke_.strokeLineJoin.eval(f).toString(strokeLineJoin) ;
    stroke_.strokeDashArray.eval(f).toNumberList(strokeDashArray) ;

    // set cairo drawing context

    cairo_set_line_width (cr, strokeWidth);

    if ( strokeLineCap == "round" )
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND) ;
    else if ( strokeLineCap == "square" )
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE) ;
    else
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT) ;

    if ( strokeLineJoin == "round" )
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND) ;
    else if ( strokeLineJoin == "bevel" )
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL) ;
    else
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);

    if ( !strokeDashArray.empty() )
    {
        int n = strokeDashArray.size() ;

        double *dashes = new double [n] ;

        for(int i=0 ; i<n ; i++ )
            dashes[i] = strokeDashArray[i] ;

        cairo_set_dash(cr, dashes, n, strokeDashOffset ) ;
        delete [] dashes ;
    }

    double r, g, b;

    r = ((strokeColor >> 16) & 0xff) / 255.0;
    g = ((strokeColor >> 8) & 0xff) / 255.0;
    b = ((strokeColor >> 0) & 0xff) / 255.0;

    strokeOpacity = clip_percent(strokeOpacity) ;

    if ( layerOpacity * strokeOpacity == 1.0 ) cairo_set_source_rgb(cr, r, g, b) ;
    else cairo_set_source_rgba(cr, r, g, b, layerOpacity * strokeOpacity) ;
}


void Renderer::applySimpleFill(cairo_t *cr, const Feature &f, const Fill &fill_, double layerOpacity, double scale)
{
    unsigned int fillColor = 0 ;
    double fillOpacity = 1.0;

    if ( !fill_.fill.eval(f).toColor(fillColor) ) return ;

    fill_.fillOpacity.eval(f).toNumber(fillOpacity) ;

    // set cairo drawing context

    double r, g, b;

    r = ((fillColor >> 16) & 0xff) / 255.0;
    g = ((fillColor >> 8) & 0xff) / 255.0;
    b = ((fillColor >> 0) & 0xff) / 255.0;

    fillOpacity = clip_percent(fillOpacity) ;

    if ( layerOpacity * fillOpacity == 1.0 ) cairo_set_source_rgb(cr, r, g, b) ;
    else cairo_set_source_rgba(cr, r, g, b, layerOpacity * fillOpacity) ;
}

void Renderer::applyFill(cairo_t *cr, const Feature &f, const Fill &fill_, double layerOpacity, double scale)
{
    if ( ! fill_.gfill )
        applySimpleFill(cr, f, fill_, layerOpacity, scale) ;
    else
    {

        cairo_rectangle_t extents ;
        cairo_surface_t *surface_ = renderGraphic(cr, f, *(fill_.gfill->graphic), scale, extents );
        if ( !surface_ ) return ;

        cairo_pattern_t *pattern_ = cairo_pattern_create_for_surface(surface_) ;

        cairo_set_source(cr, pattern_);
        cairo_pattern_set_extend(pattern_, CAIRO_EXTEND_REPEAT);
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING) ;
    }

}

void Renderer::renderLinesSimpleStroke(RenderingContext *ctx, const FeatureCollection &col, const sld::LineSymbolizer &smb)
{
    cairo_t *cr = ctx->cr ;
    const double layerOpacity = 1.0 ;

    cairo_save(cr) ;

    double offset = 0.0;



    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;
        const Stroke &stroke_ = *smb.stroke ;

        cairo_save(cr) ;

        cairo_save(cr) ;
        cairo_transform(cr, &ctx->cmm) ;

        double offset = 0 ;

        smb.perpendicularOffset.eval(f).toNumber(offset) ;

        cairo_device_to_user_distance(cr, &offset, &offset) ;

        if ( offset != 0 ) {
            gaiaGeomCollPtr offset_= offsetGeometry(f.geom, offset) ;

            cairo_path_from_geometry(cr, offset_) ;

            gaiaFreeGeomColl(offset_) ;
        }
        else
            cairo_path_from_geometry(cr, f.geom) ;

        cairo_restore(cr) ;

        applySimpleStroke(cr, f, stroke_, layerOpacity, ctx->scale) ;
        cairo_stroke(cr) ;

        cairo_restore(cr) ;

    }

    cairo_restore(cr) ;
}

void Renderer::renderLinesGraphicFill(RenderingContext *ctx, const FeatureCollection &col, const sld::LineSymbolizer &smb)
{
    cairo_t *cr = ctx->cr ;

    const double layerOpacity = 1.0 ;

    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;
        const Stroke &stroke_ = *smb.stroke ;
        const GraphicFill &gfill_ = *stroke_.gfill ;

        // This defines the width of the stroke neccessery for the pattern to become visible
        double strokeWidth = 1.0 ;

        stroke_.strokeWidth.eval(f).toNumber(strokeWidth) ;

        cairo_save(cr) ;

        cairo_rectangle_t extents ;
        cairo_surface_t *surface_ = renderGraphic(cr, f, *gfill_.graphic, ctx->scale, extents );
        if ( !surface_ ) continue ;

        cairo_set_line_width(cr, strokeWidth);

        cairo_pattern_t *pattern_ = cairo_pattern_create_for_surface(surface_) ;

        cairo_set_source(cr, pattern_);
        cairo_pattern_set_extend(pattern_, CAIRO_EXTEND_REPEAT);
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING) ;

        cairo_save(cr) ;
        cairo_transform(cr, &ctx->cmm) ;
        cairo_path_from_geometry(cr, f.geom) ;
        cairo_restore(cr) ;

        cairo_stroke(cr) ;

        cairo_restore(cr) ;

    }
}



void Renderer::renderLinesGraphicStroke(RenderingContext *ctx, const FeatureCollection &col, const LineSymbolizer &smb)
{
    using namespace sld ;

    cairo_t *cr = ctx->cr ;

    const double layerOpacity = 1.0 ;

    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;
        const Stroke &stroke_ = *smb.stroke ;
        const GraphicStroke &gstroke_ = *stroke_.gstroke ;

        for(int j=0 ; j<gstroke_.graphics.size() ; j++ )
        {
            const Graphic &graphic_ = *gstroke_.graphics[j] ;

            cairo_rectangle_t extents ;
            cairo_surface_t *surface_ = renderGraphic(cr, f, graphic_, ctx->scale, extents) ;

            if ( !surface_ ) continue ;

            double anchorPointX = 0.5 ;
            double anchorPointY = 0.5 ;
            double displacementX = 0 ;
            double displacementY = 0 ;
            double rotation = 0.0 ;
            double opacity = 1;

            graphic_.displacementX.eval(f).toNumber(displacementX) ;
            graphic_.displacementY.eval(f).toNumber(displacementY) ;
            graphic_.anchorPointX.eval(f).toNumber(anchorPointX) ;
            graphic_.anchorPointY.eval(f).toNumber(anchorPointY) ;
            graphic_.rotation.eval(f).toNumber(rotation) ;
            graphic_.opacity.eval(f).toNumber(opacity) ;

            double ofx =  extents.width * anchorPointX ;
            double ofy =  extents.height * anchorPointY ;

            opacity = clip_percent(opacity) ;

            double strokeGap = std::max(extents.width, extents.height) ;
            double strokeInitialGap = 0 ;

            gstroke_.gap.eval(f).toNumber(strokeGap) ;
            gstroke_.initialGap.eval(f).toNumber(strokeInitialGap) ;

            vector<double> pts ;

            sampleLinearGeometry(pts, f.geom, ctx->cmm, strokeGap, strokeInitialGap, vector<double>(1, 0), false) ;

            for( int j=0 ; j<pts.size() ; j+=3 )
            {
                double px = pts[j], py = pts[j+1], angle = pts[j+2] ;

                cairo_matrix_transform_point(&ctx->cmm, &px, &py) ;

                if ( ctx->colc->addLabelBox(px + displacementX, py + displacementY, rotation * M_PI/180.0, extents.width, extents.height) )
                {
                    cairo_save(cr) ;

                    cairo_translate(cr, px, py) ;
                    cairo_rotate(cr, rotation * M_PI/180.0 + angle) ;
                    cairo_translate(cr, displacementX, displacementY) ;
                    cairo_translate(cr, -ofx, ofy - extents.height) ;

                    cairo_set_source_surface (cr, surface_, 0, 0);
                    if ( opacity == 1.0 ) cairo_paint(cr) ;
                    else cairo_paint_with_alpha(cr, opacity) ;

                    cairo_restore(cr) ;
                }


            }

            cairo_surface_destroy(surface_) ;
        }
    }



}

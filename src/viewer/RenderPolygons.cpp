#include "Renderer.h"

using namespace std ;
using namespace sld ;

extern void cairo_path_from_geometry(cairo_t *cr, const gaiaGeomCollPtr geom_) ;

void Renderer::renderPolygons(RenderingContext *ctx, const FeatureCollection &col, const sld::PolygonSymbolizer &smb)
{
    cairo_t *cr = ctx->cr ;

    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;

        cairo_save(cr) ;

        cairo_save(cr) ;
        cairo_transform(cr, &ctx->cmm) ;
        cairo_path_from_geometry(cr, f.geom) ;
        cairo_restore(cr) ;

        if ( smb.fill )
        {
            applyFill(cr, f, *smb.fill, 1.0, ctx->scale) ;
            if ( smb.stroke ) cairo_fill_preserve(cr) ;
            else cairo_fill(cr) ;
        }

        if ( smb.stroke )
        {
            applySimpleStroke(cr, f, *smb.stroke, 1.0, ctx->scale) ;
            cairo_stroke(cr) ;
        }

        cairo_restore(cr) ;
    }
}

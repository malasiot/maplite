#include "Renderer.h"
#include "Style.h"

#include <fstream>
#include <spatialite/gaiageo.h>

#include "Feature.h"
#include "ResourceCache.h"
#include "CollisionChecker.h"
#include "ParseUtil.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std ;


//////////////////////////////////////////////////////////////



Renderer::Renderer(MapFile &f, const string &rs): mf(f), cache(new ResourceCache), resourceDir(rs)
{

}


bool Renderer::render(ImageBuffer &target, int target_srid, const BBox &box, const string &_layers, const string &_styles,
                      const std::string &bg_color, unsigned int query_buffer)
{

    vector<string> layers, styles ;

    boost::algorithm::split(layers, _layers, boost::is_any_of(" ,"), boost::token_compress_on) ;
    boost::algorithm::split(styles, _styles, boost::is_any_of(" ,"), boost::token_compress_on) ;

    assert(layers.size() == styles.size()) ;

    BBox target_extents, query_extents ;

    if ( !box.transform(target_srid, target_extents, *mf.pj_cache_) ) return false ;

    const double scale = target_extents.width()/target.width() ;
    const double scale_denom = scale/0.00028;

    // inflate target box with buffer to avoid rendering artifacts of labels accross tiles

    double extra_width = ((2 * query_buffer + target.width()) * scale - target_extents.width())/2 ;
    double extra_height = ((2 * query_buffer + target.height()) * scale - target_extents.height())/2 ;

    query_extents = target_extents ;
    query_extents.minx -= extra_width ;
    query_extents.miny -= extra_height ;
    query_extents.maxx += extra_width ;
    query_extents.maxy += extra_height ;

    using namespace sld ;

    cairo_t *cr = cairo_create(target.surface_) ;

    // setup coordinate transformation

    cairo_matrix_t cmm ;

    cairo_matrix_init_identity(&cmm) ;
    cairo_matrix_scale(&cmm, 1.0/scale, -1.0/scale) ;
    cairo_matrix_translate(&cmm, -target_extents.minx, -target_extents.maxy) ;

    // setup context

    RenderingContext ctx ;

    ctx.cmm = cmm ;
    ctx.cr = cr ;
    ctx.scale = scale ;
    ctx.colc = new CollisionChecker() ;
    ctx.extents = target_extents ;

    // set map background here

    cairo_save(cr) ;
    cairo_transform(cr, &cmm) ;

    cairo_rectangle(cr, target_extents.minx, target_extents.miny, target_extents.width(), target_extents.height()) ;
    cairo_restore(cr) ;

    unsigned int fillColor ;
    parse_css_color(bg_color, fillColor)  ;

    double r, g, b;

    r = ((fillColor >> 16) & 0xff) / 255.0;
    g = ((fillColor >> 8) & 0xff) / 255.0;
    b = ((fillColor >> 0) & 0xff) / 255.0;

    cairo_set_source_rgb(cr, r, g, b);
    cairo_fill_preserve(cr) ;
    cairo_clip(cr) ;

    // set global parameters

    cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL); //?

    for(int i=0 ; i<layers.size() ; i++)
    {
        const string &layerName = layers[i] ;

        // get layer information

        LayerInfo info ;
        if ( !mf.getLayerInfo(layerName, info) ) continue ;

        // load style for the layer

        string stylePath = resourceDir + "/" + styles[i] + ".sld";

        ResourceCache::Data styleData ;

        if ( cache->find(stylePath, styleData) ) ;
        else {

            if ( !sld::fromXml(mf, stylePath, styleData.style_) ) {
                cerr << "Error loading style: " << stylePath << endl ;
                continue ;
            }
            else
                cache->save(stylePath, styleData) ;
        }

        vector<sld::FeatureTypeStylePtr> &fts = styleData.style_ ;

        // precess each style

        for( int j=0 ; j<fts.size() ; j++ )
        {
            FeatureTypeStylePtr fts_ = fts[j] ;

            string elseCondition ;

            for(int k=0 ; k<fts_->rules.size() ; k++ )
            {

                RulePtr rule_ = fts_->rules[k] ;

                if ( scale_denom >= rule_->maxScaleDenom || scale_denom < rule_->minScaleDenom ) continue ;

                string condition ;

                FilterPtr filter_ = rule_->filter ;

                if ( filter_ )
                {
                    if ( filter_->type() == Filter::Expression ) {
                        FilterExpression *fex = dynamic_cast<FilterExpression *>(filter_.get()) ;

                        condition = fex->toSQL() ;


                        elseCondition += ( elseCondition.empty() ) ? condition : " AND " + condition ;
                    }
                    else
                        condition = "NOT (" + elseCondition + ")";
                }

                if ( info.type == "rasterlite" )
                {
                    RasterData raster ;

//                    if ( mf.queryRasterlite(raster, info, target_srid, scale, target_extents) )
//                        renderRaster(&ctx, raster, rule_->symbolizers) ;

          //          if ( mf.queryTiles(raster, info, target_srid, scale, target_extents) )
          //              renderRaster(&ctx, raster, rule_->symbolizers) ;

                }
          /*      else if ( 1 )
                {
                    RasterData raster ;

                    if ( mf.queryTiles(raster, info, target_srid, scale, target_extents) )
                        renderRaster(&ctx, raster, rule_->symbolizers) ;

                }
                */
                else
                {
                    FeatureCollection col ;

                    if ( mf.queryFeatures(col, info, target_srid, condition, query_extents) )
                        renderFeatures(&ctx, col, rule_->symbolizers) ;

                }

            }


        }
    }

    cairo_destroy(cr) ;

    delete ctx.colc ;

}

// dispatch to renderers

void Renderer::renderFeatures(RenderingContext *cr, const FeatureCollection &col,  const std::vector<sld::SymbolizerPtr> &symbolizers)
{
    for( int i=0 ; i<symbolizers.size() ; i++ )
    {
        sld::SymbolizerPtr p_ = symbolizers[i] ;

        switch ( p_->type() )
        {
            case sld::Symbolizer::Point:
                renderPoints(cr, col, *static_cast<const sld::PointSymbolizer *>(p_.get())) ;
                break ;
            case sld::Symbolizer::Line:
                renderLines(cr, col, *static_cast<const sld::LineSymbolizer *>(p_.get())) ;
                break ;
            case sld::Symbolizer::Polygon:
                renderPolygons(cr, col, *static_cast<const sld::PolygonSymbolizer *>(p_.get())) ;
                break ;
            case sld::Symbolizer::Text:
                renderText(cr, col, *static_cast<const sld::TextSymbolizer *>(p_.get())) ;
                break ;
        }
    }

}

void Renderer::renderLines(RenderingContext *cr, const FeatureCollection &col, const sld::LineSymbolizer &smb)
{
    if ( smb.stroke->gstroke )
        renderLinesGraphicStroke(cr, col, smb) ;
    else if ( smb.stroke->gfill )
        renderLinesGraphicFill(cr, col, smb) ;
    else
        renderLinesSimpleStroke(cr, col, smb) ;
}

void Renderer::renderText(RenderingContext *cr, const FeatureCollection &col, const sld::TextSymbolizer &smb)
{
    if ( smb.linePlacement ) renderLinesText(cr, col, smb) ;
    else if ( smb.pointPlacement ) renderPointsText(cr, col, smb) ;
}

void Renderer::renderPoints(RenderingContext *cr, const FeatureCollection &col, const sld::PointSymbolizer &smb)
{
    for( int i=0 ; i<smb.graphics.size() ; i++ )
        renderPointsGraphic(cr, col, *smb.graphics[i]) ;
}


void Renderer::renderRaster(RenderingContext *ctx, const RasterData &raster, const std::vector<sld::SymbolizerPtr> &symbolizers)
{
    cairo_matrix_t cmm ;

    cairo_matrix_init_identity(&cmm) ;

    cairo_matrix_scale(&cmm, 1/ctx->scale, -1/ctx->scale) ;
    cairo_matrix_translate(&cmm, -ctx->extents.minx, -ctx->extents.maxy) ;

    for( int i=0 ; i<symbolizers.size() ; i++ )
    {
        sld::SymbolizerPtr p_ = symbolizers[i] ;

        if ( p_->type() == sld::Symbolizer::Raster )
        {
            const sld::RasterSymbolizer *rs = static_cast<const sld::RasterSymbolizer *>(p_.get()) ;

            double opacity = 1.0 ;

            Feature f ;
            rs->opacity.eval(f).toNumber(opacity) ;

            cairo_t *cr = ctx->cr ;

            cairo_surface_t *img_surf = cairo_image_surface_create_for_data((unsigned char *)raster.image, CAIRO_FORMAT_ARGB32, raster.width, raster.height, raster.stride) ;

            cairo_status_t status = cairo_surface_status(img_surf) ;
            if ( status ==  CAIRO_STATUS_NO_MEMORY || status == CAIRO_STATUS_READ_ERROR ) continue ;

            cairo_save(cr) ;

            cairo_transform(cr, &ctx->cmm) ;
            cairo_translate(cr, raster.minx, raster.maxy) ;
            cairo_scale(cr, raster.px, -raster.py) ;

            cairo_set_source_surface(cr, img_surf, 0, 0) ;
            cairo_paint_with_alpha(cr, opacity);

            cairo_restore(cr) ;

            cairo_surface_destroy(img_surf) ;
        }
    }
}

string Renderer::resolveHRef(const string &href)
{
    string res ;

    if ( boost::starts_with(href, "file://") )
    {
        res = href.substr(7) ;

        boost::filesystem::path resource_path(res), absolute_path ;

        absolute_path = boost::filesystem::absolute(resource_path, resourceDir) ;

        res = absolute_path.string() ;
    }

    return res ;

}

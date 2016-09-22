#include "renderer.hpp"
#include "theme.hpp"

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std ;
using namespace mapsforge ;

class ResourceCache {

};

class CollisionChecker {

};

Renderer::Renderer(const mapsforge::RenderTheme &theme):
    cache_(new ResourceCache), theme_(theme)
{

}

static void get_rgb_color(uint32_t clr, double r, double g, double b) {
    r = ((clr >> 16) & 0xff) / 255.0;
    g = ((clr >> 8) & 0xff) / 255.0;
    b = ((clr >> 0) & 0xff) / 255.0;
}


bool Renderer::render(ImageBuffer &target, const VectorTile &tile, const BBox &box, uint8_t zoom, const string &layer, unsigned int query_buffer)
{
    BBox target_extents, query_extents ;

    tms::latlonToMeters(box.miny_, box.minx_, target_extents.minx_, target_extents.miny_) ;
    tms::latlonToMeters(box.maxy_, box.maxx_, target_extents.maxx_, target_extents.maxy_) ;

    const double scale = target_extents.width()/target.width() ;
    const double scale_denom = scale/0.00028;

    // inflate target box with buffer to avoid rendering artifacts of labels accross tiles

    double extra_width = ((2 * query_buffer + target.width()) * scale - target_extents.width())/2 ;
    double extra_height = ((2 * query_buffer + target.height()) * scale - target_extents.height())/2 ;

    query_extents = target_extents ;
    query_extents.minx_ -= extra_width ;
    query_extents.miny_ -= extra_height ;
    query_extents.maxx_ += extra_width ;
    query_extents.maxy_ += extra_height ;

    cairo_t *cr = cairo_create(target.surface_) ;

    // setup coordinate transformation

    cairo_matrix_t cmm ;

    cairo_matrix_init_identity(&cmm) ;
    cairo_matrix_scale(&cmm, 1.0/scale, -1.0/scale) ;
    cairo_matrix_translate(&cmm, -target_extents.minx_, -target_extents.maxy_) ;

    // setup context

    RenderingContext ctx ;

    ctx.cmm_ = cmm ;
    ctx.cr_ = cr ;
    ctx.scale_ = scale ;
    ctx.colc_.reset(new CollisionChecker()) ;
    ctx.extents_ = target_extents ;

    // set map background here

    cairo_save(cr) ;
    cairo_transform(cr, &cmm) ;

    cairo_rectangle(cr, target_extents.minx_, target_extents.miny_, target_extents.width(), target_extents.height()) ;
    cairo_restore(cr) ;


    double r, g, b;
    get_rgb_color(theme_.backgroundColor(), r, g, b) ;

    cairo_set_source_rgb(cr, r, g, b);
    cairo_fill_preserve(cr) ;
    cairo_clip(cr) ;

    // set global parameters

    cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL); //?

    for( const POI &poi: tile.pois_ ) {
        vector<RenderInstructionPtr> inst_list ;
        theme_.match(layer, poi.tags_, zoom, false, false, inst_list) ;

        double mx, my ;
        tms::latlonToMeters(poi.lat_, poi.lon_, mx, my) ;

        for( const RenderInstructionPtr &ip: inst_list ) {
            if ( ip->type() == RenderInstruction::Circle ) {
                CircleInstruction &ins = *std::dynamic_pointer_cast<CircleInstruction>(ip) ;
                drawCircle(&ctx, mx, my, ins) ;
            }
            else if ( ip->type() == RenderInstruction::Symbol ) {
                SymbolInstruction &ins = *std::dynamic_pointer_cast<SymbolInstruction>(ip) ;
                drawSymbol(&ctx, mx, my, ins) ;
            }
            else if ( ip->type() == RenderInstruction::Caption ) {
                CaptionInstruction &ins = *std::dynamic_pointer_cast<CaptionInstruction>(ip) ;
                drawCaption(&ctx, mx, my, poi.tags_.get(ins.key_, "?"), ins ) ;
            }
        }
    }


    cairo_destroy(cr) ;
}

void Renderer::drawCircle(Renderer::RenderingContext *ctx, double px, double py, const CircleInstruction &)
{

}

void Renderer::drawSymbol(Renderer::RenderingContext *ctx, double px, double py, const SymbolInstruction &)
{

}

void Renderer::drawCaption(Renderer::RenderingContext *ctx, double px, double py, const string &label, const CaptionInstruction &)
{

}


#if 0
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
#endif

#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cairo/cairo.h>

#include "geom_helpers.hpp"
#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "image_buffer.hpp"

#include <memory>

using std::string ;

class ResourceCache ;
class CollisionChecker ;

class Renderer {

public:

    Renderer(const mapsforge::RenderTheme &theme) ;

    bool render(ImageBuffer &target, const mapsforge::VectorTile &tile,
                const BBox &box,
                uint8_t zoom,
                const std::string &layer,      // comma separated list of layers
                unsigned int query_buffer = 0) ; // extra space around image (feature query) to avoid artifacts across tiles

private:

    std::shared_ptr<ResourceCache> cache_ ;
    const mapsforge::RenderTheme &theme_ ;

private:

    struct RenderingContext {
        cairo_matrix_t cmm_ ;
        double scale_ ;
        cairo_t *cr_ ;
        std::shared_ptr<CollisionChecker> colc_ ;
        BBox extents_ ;
        std::map<std::string, cairo_font_face_t *> ff_cache_ ;
    };

    void drawCircle(RenderingContext *ctx, double px, double py, const mapsforge::CircleInstruction &) ;
    void drawSymbol(RenderingContext *ctx, double px, double py, const mapsforge::SymbolInstruction &) ;
    void drawCaption(RenderingContext *ctx, double px, double py, const std::string &label, const mapsforge::CaptionInstruction &) ;

    //void renderPoints(RenderingContext *ctx, const std::vector<mapsforge::LatLon> &pts,  const sld::PointSymbolizer &smb) ;
    //void renderLines(RenderingContext *ctx, const mapsforge::VectorTile &tile,  const sld::LineSymbolizer &smb) ;
/*    void renderPolygons(RenderingContext *ctx, const mapsforge::VectorTile &tile,  const sld::PolygonSymbolizer &smb) ;
    void renderText(RenderingContext *ctx, constconst mapsforge::VectorTile &tile &col,  const sld::TextSymbolizer &smb) ;

    void renderLinesGraphicStroke(RenderingContext *ctx, const FeatureCollection &col, const sld::LineSymbolizer &smb) ;
    void renderLinesGraphicFill(RenderingContext *ctx, const FeatureCollection &col, const sld::LineSymbolizer &smb) ;
    void renderLinesSimpleStroke(RenderingContext *ctx, const FeatureCollection &col, const sld::LineSymbolizer &smb) ;
    void renderPointsGraphic(RenderingContext *ctx, const FeatureCollection &col, const sld::Graphic &graphic) ;

    void renderPointsText(RenderingContext *ctx, const FeatureCollection &col, const sld::TextSymbolizer &smb);
    void renderLinesText(RenderingContext *ctx, const FeatureCollection &col, const sld::TextSymbolizer &smb) ;

    cairo_surface_t *renderGraphic(cairo_t *cr, const Feature &f, const sld::Graphic &g, double scale, cairo_rectangle_t &extents);
    void applySimpleStroke(cairo_t *cr, const Feature &f, const sld::Stroke &stroke_, double layerOpacity, double scale);
    void applySimpleFill(cairo_t *cr, const Feature &f, const sld::Fill &fill_, double layerOpacity, double scale);
    void applyFill(cairo_t *cr, const Feature &f, const sld::Fill &fill_, double layerOpacity, double scale);
    void drawWellKnownShape(cairo_t *cr, const std::string &shapeName, double size);
    bool findSymbol(const std::string &faceName, unsigned long charCode, cairo_font_face_t *&ff, unsigned int &glyphIndex) ;

    void paintText(RenderingContext *ctx, const Feature &f, const sld::TextSymbolizer &smb);

    void renderRaster(RenderingContext *ctx, const RasterData &raster, const std::vector<sld::SymbolizerPtr> &smb) ;

    std::string resolveHRef(const std::string &href) ;
    */
};




#endif

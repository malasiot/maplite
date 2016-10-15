#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "MapFile.h"
#include "Style.h"

#include "ImageBuffer.h"

using std::string ;

class ResourceCache ;
struct RenderingContext ;
class CollisionChecker ;

class Renderer {

public:

    Renderer(MapFile &f, const string &resDir) ;

    bool render(ImageBuffer &buffer,
                int target_srid,
                const BBox &box,
                const std::string &layers,      // comma separated list of layers
                const std::string &styles,     // comma separated list of associated styles
                const std::string &bg_color,   // background color in css format
                unsigned int query_buffer = 0) ; // extra space around image (feature query) to avoid artifacts across tiles

private:

    MapFile &mf ;
    boost::shared_ptr<ResourceCache> cache ;
    string resourceDir ;

private:


    struct RenderingContext {
        cairo_matrix_t cmm ;
        double scale ;
        cairo_t *cr ;
        CollisionChecker *colc ;
        BBox extents ;
        std::map<std::string, cairo_font_face_t *> ff_cache ;
    };

    void renderFeatures(RenderingContext *ctx, const FeatureCollection &col, const std::vector<sld::SymbolizerPtr> &smb) ;

    void renderPoints(RenderingContext *ctx, const FeatureCollection &col,  const sld::PointSymbolizer &smb) ;
    void renderLines(RenderingContext *ctx, const FeatureCollection &col,  const sld::LineSymbolizer &smb) ;
    void renderPolygons(RenderingContext *ctx, const FeatureCollection &col,  const sld::PolygonSymbolizer &smb) ;
    void renderText(RenderingContext *ctx, const FeatureCollection &col,  const sld::TextSymbolizer &smb) ;

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
};




#endif

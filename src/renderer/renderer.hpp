#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cairo/cairo.h>

#include "tms.hpp"
#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "image_buffer.hpp"
#include "resource_cache.hpp"
#include "collision_checker.hpp"
#include "text_engine.hpp"

#include <memory>

using std::string ;

class ResourceCache ;
class CollisionChecker ;
class RenderingContext ;

class Renderer {

public:

    Renderer(const std::shared_ptr<RenderTheme> &theme, const std::string &pref_language, bool debug=false) ;
    Renderer(const std::string &pref_language, bool debug=false) ;

    // render a single tile
    bool render(const TileKey &key,                 // tile key
                ImageBuffer &target,                // target image surface
                const VectorTile &tile,             // vector tile data
                const std::string &layer,           // layer
                unsigned int query_buffer = 128) ;    // extra space around image (feature query) to avoid artifacts across tiles

    void setTheme(const std::shared_ptr<RenderTheme> &theme) { theme_ = theme ; }

private:

    ResourceCache cache_ ;
    std::shared_ptr<RenderTheme> theme_ ;
    TextEngine text_engine_ ;
    std::string pref_language_ ;

private:

    struct POIInstruction {

        POIInstruction(double x, double y, double angle, const RenderInstructionPtr &ri, const std::string &label = std::string(), int32_t poi_idx = -1, int32_t item_idx = 0):
            x_(x), y_(y), angle_(angle), ri_(ri), label_(label), poi_idx_(poi_idx), item_idx_(item_idx) {}

        double x_, y_, angle_ ;
        RenderInstructionPtr ri_ ;
        int32_t poi_idx_, item_idx_ ;
        std::string label_ ;
    };

    struct WayInstruction {
        WayInstruction(const std::vector<std::vector<Coord>> &coords, RenderInstructionPtr ri, uint32_t way_idx, uint32_t z_order):
            coords_(coords), ri_(ri), way_idx_(way_idx), z_order_(z_order) {}

        std::vector<std::vector<Coord>> coords_;
        RenderInstructionPtr ri_ ;
        uint32_t way_idx_, z_order_ ;
    };

    friend class POIInstructionSorter ;
    friend class WayInstructionSorter ;

    void filterWays(const string &layer, uint8_t zoom, const std::vector<Way> &ways, std::vector<WayInstruction> &winstructions);

    void filterPOIs(const string &layer, uint8_t zoom, const BBox &box, cairo_matrix_t &cmm, const std::vector<Way> &ways, const std::vector<POI> &pois,
                     std::vector<POIInstruction> &instructions, uint32_t &count) ;

    void drawCircle(RenderingContext &ctx, double px, double py, const RenderInstruction &) ;
    void drawSymbol(RenderingContext &ctx, double px, double py, double angle, const std::string &src, const RenderInstruction &, int32_t, int32_t) ;
    void drawCaption(RenderingContext &ctx, double px, double py, double angle, const std::string &label, const RenderInstruction &, int32_t, int32_t) ;

    void drawLine(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const RenderInstruction &line);
    void drawSymbolsAlongLine(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const RenderInstruction &line);
    void drawArea(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const RenderInstruction &area);
    void applySimpleStroke(cairo_t *cr, float stroke_width, uint32_t stroke_clr, const std::vector<float> &dash_array,
                           uint cap, uint join);
    void applySimpleFill(cairo_t *cr, uint32_t fill_clr);
    cairo_surface_t *renderGraphic(cairo_t *cr, const std::string &src, double width, double height, cairo_rectangle_t &rect, double scale) ;
    cairo_surface_t *renderOSMCGraphic(cairo_t *cr, const std::string &desc, double width, double height, cairo_rectangle_t &rect, double scale) ;
    void getSymbolSize(const RenderInstruction &r, double &sw, double &sh);

    std::string langString(const std::string &label);

    const double collision_extra = 10 ;

    bool debug_ ;


};


#endif

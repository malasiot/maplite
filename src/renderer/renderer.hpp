#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <cairo/cairo.h>

#include "tms.hpp"
#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "image_buffer.hpp"
#include "resource_cache.hpp"
#include "collision_checker.hpp"

#include <memory>

using std::string ;

class ResourceCache ;
class CollisionChecker ;

class Renderer {

public:

    Renderer(const mapsforge::RenderTheme &theme) ;

    bool render(ImageBuffer &target, const mapsforge::VectorTile &tile,
                const TileKey &key,
                const std::string &layer,
                unsigned int query_buffer = 0) ; // extra space around image (feature query) to avoid artifacts across tiles

private:

    std::shared_ptr<ResourceCache> cache_ ;
    const mapsforge::RenderTheme &theme_ ;

private:

    struct RenderingContext {
        cairo_matrix_t cmm_ ;
        double scale_ ;
        cairo_t *cr_ ;
        CollisionChecker colc_ ;
        BBox extents_ ;
    };

    struct POIInstruction {

        POIInstruction(double x, double y, double angle, const mapsforge::RenderInstructionPtr &ri, const std::string &label = std::string(), int32_t idx = -1):
            x_(x), y_(y), angle_(angle), ri_(ri), label_(label), poi_idx_(idx) {}

        double x_, y_, angle_ ;
        mapsforge::RenderInstructionPtr ri_ ;
        int32_t poi_idx_ ;
        std::string label_ ;
    };

    friend class POIInstructionSorter ;

    void filterWays(const std::string &layer, uint8_t zoom, const std::vector<mapsforge::Way> &ways, std::vector<std::pair<uint, mapsforge::RenderInstructionPtr>> &instructions ) ;
    void filterPOIs(const string &layer, uint8_t zoom, const BBox &box, const std::vector<mapsforge::POI> &pois,
                     std::vector<POIInstruction> &instructions) ;

    void drawCircle(RenderingContext &ctx, double px, double py, const mapsforge::RenderInstruction &) ;
    void drawSymbol(RenderingContext &ctx, double px, double py, double angle, const mapsforge::RenderInstruction &) ;
    void drawCaption(RenderingContext &ctx, double px, double py, double angle, const std::string &label, const mapsforge::RenderInstruction &) ;

    void drawLine(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const mapsforge::RenderInstruction &line);
    void drawSymbolsAlongLine(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const mapsforge::RenderInstruction &line);
    void drawArea(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const mapsforge::RenderInstruction &area);
    void applySimpleStroke(cairo_t *cr, float stroke_width, uint32_t stroke_clr, const std::vector<float> &dash_array,
                           uint cap, uint join);
    void applySimpleFill(cairo_t *cr, uint32_t fill_clr);
    cairo_surface_t *renderGraphic(cairo_t *cr, const std::string &src, double width, double height, cairo_rectangle_t &rect, double scale) ;

    const double collision_extra = 10 ;
};




#endif

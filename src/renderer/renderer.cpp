#include "renderer.hpp"
#include "geometry.hpp"

#include "theme.hpp"

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <array>

#include "svg/rendering.hpp"

using namespace std ;

struct RenderingContext {
    cairo_matrix_t cmm_ ;
    double scale_ ;
    cairo_t *cr_ ;
    CollisionChecker colc_ ;
    BBox extents_ ;
};

extern void latlon_to_tms(const std::vector<std::vector<LatLon>> &latlon,  std::vector<std::vector<Coord>> &coords) ;

extern void sample_linear_geometry(
                    const std::vector< std::vector<Coord>> geom,
                    const cairo_matrix_t &cmm,
                    float gap,
                    float initial_gap,
                    float box_len,
                    bool fix_angle,
                    std::vector<Coord> &samples,
                    std::vector<double> &angles) ;

void offset_geometry(const vector<vector<Coord>> &geom, double offset, vector<vector<Coord>> &res) ;


Renderer::Renderer(RenderTheme &theme, const std::string &lang, bool debug):
    theme_(theme), debug_(debug), text_engine_(cache_, lang)
{

}

static void get_argb_color(uint32_t clr, double &a, double &r, double &g, double &b) {
    a = ((clr >> 24) & 0xff) / 255.0;
    r = ((clr >> 16) & 0xff) / 255.0;
    g = ((clr >> 8) & 0xff) / 255.0;
    b = ((clr >> 0) & 0xff) / 255.0;
}


void get_poi_from_area(const Way &way, const vector<Coord> &coords, double &mx, double &my) {
    mx = my = 0.0 ;
    for(uint i=0 ; i<coords.size()-1 ; i++) {
        const Coord &c = coords[i] ;
        mx += c.x_ ;
        my += c.y_ ;
    }

    mx /= coords.size()-1 ;
    my /= coords.size()-1 ;
}


class WayInstructionSorter {
public:
    WayInstructionSorter(const vector<Way> &ways, const vector<Renderer::WayInstruction> &inst): ways_(ways), instructions_(inst) {}

    bool operator() (const Renderer::WayInstruction &a, const Renderer::WayInstruction &b) {

        if ( a.z_order_ == b.z_order_ )
            return ( ways_[a.way_idx_].layer_ < ways_[b.way_idx_].layer_ ) ;
        else
            return  (a.z_order_ < b.z_order_) ;
    }

    const vector<Way> &ways_ ;
    const vector<Renderer::WayInstruction> &instructions_ ;
};

class POIInstructionSorter {
public:
    POIInstructionSorter() {}

    bool operator() (const Renderer::POIInstruction &a, const Renderer::POIInstruction &b) {
        return  (a.ri_->priority_ > b.ri_->priority_) ;
    }

};

void Renderer::filterWays(const string &layer, uint8_t zoom, const BBox &query_extents, cairo_matrix_t &cmm, const std::vector<Way> &ways,
                          std::vector<WayInstruction> &winstructions, std::vector<POIInstruction> &poi_instructions, int32_t &count)
{

    for( uint32_t idx = 0 ; idx < ways.size() ; ++idx  ) {
        const Way &way = ways[idx] ;
        vector<RenderInstructionPtr> ris ;

        if ( theme_.match(layer, way.tags_, zoom, way.is_closed_, true, ris)) {

            int32_t symbol_id = -1 ;
            vector<POIInstruction> instructions ;

            vector<vector<Coord>> coords ;
            latlon_to_tms(way.coords_, coords) ;

            for( const auto &ri: ris ) {

                if ( ri->type() == RenderInstruction::Area ) {
                    winstructions.emplace_back(std::move(coords), ri, idx, ri->z_order_) ;
                }
                else if ( ri->type() == RenderInstruction::Line ) {
                    winstructions.emplace_back(std::move(coords), ri, idx, ri->z_order_) ;
                }
                else if ( ri->type() == RenderInstruction::Symbol ) {

                    double mx, my ;
                    if ( way.label_pos_ ) {
                       LatLon lp = way.label_pos_.get() ;
                       tms::latlonToMeters(lp.lat_, lp.lon_, mx, my) ;
                    }
                    else get_poi_from_area(way, coords[0], mx, my) ;

                    symbol_id = count ;

                    if ( query_extents.contains(mx, my) )
                        instructions.emplace_back(mx, my, 0.0, ri, way.tags_.get(ri->key_), count++) ;
                }
                else if ( ri->type() == RenderInstruction::Caption )
                {
                    double mx, my ;
                    if ( way.label_pos_ ) {
                       LatLon lp = way.label_pos_.get() ;
                       tms::latlonToMeters(lp.lat_, lp.lon_, mx, my) ;
                    }
                    else get_poi_from_area(way, coords[0], mx, my) ;

                    if ( query_extents.contains(mx, my) )
                        instructions.emplace_back(mx, my, 0.0, ri, way.tags_.get(ri->key_), count++) ;
                }
                else if ( ri->type() == RenderInstruction::Circle )
                {
                    double mx, my ;
                    if ( way.label_pos_ ) {
                       LatLon lp = way.label_pos_.get() ;
                       tms::latlonToMeters(lp.lat_, lp.lon_, mx, my) ;
                    }
                    else get_poi_from_area(way, coords[0], mx, my) ;

                    if ( query_extents.contains(mx, my) )
                        instructions.emplace_back(mx, my, 0.0, ri, way.tags_.get(ri->key_), count++) ;
                }
                else if ( ri->type() == RenderInstruction::LineSymbol ) {
                    RenderInstruction &line = *ri.get() ;
                    vector<Coord> pts ;
                    vector<double> angles ;

                    double gap = ( line.repeat_ ) ? line.repeat_gap_ : 0.0 ;
                    double initial_gap = ( line.repeat_ ) ? line.repeat_start_ : 0.0 ;

                    double sw, sh ;
                    getSymbolSize(line, sw, sh) ;

                    sample_linear_geometry(coords, cmm, gap, initial_gap, sw,  false, pts, angles) ;

                    for( uint i=0 ; i<pts.size() ; i++ ) {
                        const Coord &c = pts[i] ;
                        if ( query_extents.contains(c.x_, c.y_) )
                            instructions.emplace_back(c.x_, c.y_, angles[i], ri, string(), count++) ;
                    }
                }
                else if ( ri->type() == RenderInstruction::PathText ) {
                    RenderInstruction &line = *ri.get() ;
                    vector<Coord> pts ;
                    vector<double> angles ;

                    double gap = ( line.repeat_ ) ? line.repeat_gap_ : 0.0 ;
                    double initial_gap = ( line.repeat_ ) ? line.repeat_start_ : 0.0 ;
                    string label = way.tags_.get(line.key_) ;

                    sample_linear_geometry(coords, cmm, gap, initial_gap, line.font_size_ * label.length(), true, pts, angles) ;


                    for( uint i=0 ; i<pts.size() ; i++ ) {
                        const Coord &c = pts[i] ;
                        if ( query_extents.contains(c.x_, c.y_) )
                            instructions.emplace_back(c.x_, c.y_, angles[i], ri, label, count++) ;
                    }
                }
            }

            for( auto &ins: instructions ) {
                RenderInstructionPtr ri = ins.ri_ ;
                if ( ri->type_ == RenderInstruction::Caption && ri->symbol_ ) ins.poi_idx_ = symbol_id ;
            }

            std::copy( instructions.begin(), instructions.end(), std::back_inserter(poi_instructions)) ;

        }
    }

    std::sort(winstructions.begin(), winstructions.end(), WayInstructionSorter(ways, winstructions)) ;

}

void Renderer::filterPOIs(const string &layer, uint8_t zoom, const BBox &box, const std::vector<POI> &pois,
                          std::vector<POIInstruction> &instructions, int32_t &count)
{
    for( uint32_t idx = 0 ; idx < pois.size() ; ++idx  ) {
        const POI &poi = pois[idx] ;
        vector<RenderInstructionPtr> ris ;

        if ( theme_.match(layer, poi.tags_, zoom, false, false, ris)) {

            int32_t symbol_id = -1 ;
            std::vector<POIInstruction> instr ;

            for( const auto &ri: ris ) {
                double mx, my ;
                tms::latlonToMeters(poi.lat_, poi.lon_, mx, my) ;

                if ( ri->type_ == RenderInstruction::Symbol )
                    symbol_id = count ;

                if ( box.contains(mx, my))
                    instr.emplace_back(mx, my, 0, ri, poi.tags_.get(ri->key_), count++) ;
            }

            for( auto &ins: instr ) {
                RenderInstructionPtr ri = ins.ri_ ;
                if ( ri->type_ == RenderInstruction::Caption && ri->symbol_ ) ins.poi_idx_ = symbol_id ;
            }

            std::copy( instr.begin(), instr.end(), std::back_inserter(instructions)) ;
        }
    }


    std::sort(instructions.begin(), instructions.end(), POIInstructionSorter()) ;
}

bool Renderer::render(const TileKey &key, ImageBuffer &target, const VectorTile &tile, const string &layer, unsigned int query_buffer)
{
    BBox box ;
    tms::tileLatLonBounds(key.x(), key.y(), key.z(), box.miny_, box.minx_, box.maxy_, box.maxx_) ;
    uint8_t zoom = key.z();

    BBox target_extents, query_extents ;

    tms::latlonToMeters(box.miny_, box.minx_, target_extents.minx_, target_extents.miny_) ;
    tms::latlonToMeters(box.maxy_, box.maxx_, target_extents.maxx_, target_extents.maxy_) ;

    const double scale = target_extents.width()/target.width() ;

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
    ctx.extents_ = target_extents ;

    // set map background here

    cairo_save(cr) ;
    cairo_transform(cr, &cmm) ;

    cairo_rectangle(cr, target_extents.minx_, target_extents.miny_, target_extents.width(), target_extents.height()) ;
    cairo_restore(cr) ;

    double a, r, g, b;
    get_argb_color(theme_.backgroundColor(), a, r, g, b) ;

    if ( a == 1 ) cairo_set_source_rgb(cr, r, g, b);
    else cairo_set_source_rgba(cr, r, g, b, a) ;

    cairo_fill_preserve(cr) ;
    cairo_clip(cr) ;

    // initialize context

    ctx.scale_ = ( zoom > 12 ) ? pow(1.5, zoom - 12 ) : 1.0 ;

    // set global parameters

    cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL); //?

    // sort ways based on layer attribute

    vector<WayInstruction> way_instructions ;

    vector<POIInstruction> poi_instructions ;

    int32_t count = 0 ;
    filterWays(layer, zoom, query_extents, cmm, tile.ways_, way_instructions, poi_instructions, count) ;
    filterPOIs(layer, zoom, query_extents, tile.pois_, poi_instructions, count ) ;

    for( auto &ip: way_instructions ) {

        RenderInstructionPtr ri = ip.ri_ ;

        const vector<vector<Coord>> &coords = ip.coords_ ;

        if ( ri->type() == RenderInstruction::Area ) {
            drawArea(ctx, coords, *ri.get()) ;
        }
        else if ( ri->type() == RenderInstruction::Line ) {
            drawLine(ctx, coords, *ri.get()) ;
        }
    }

    for( auto &ip: poi_instructions  ) {

        RenderInstructionPtr ri = ip.ri_ ;
        double mx = ip.x_, my = ip.y_ ;
        double angle = ip.angle_ ;
        string label = ip.label_ ;

        switch ( ri->type() ) {
        case RenderInstruction::Circle:
            drawCircle(ctx, mx, my, *ri.get()) ;
            break ;
        case RenderInstruction::Symbol:
            drawSymbol(ctx, mx, my, 0.0, *ri.get(), ip.poi_idx_) ;
            break ;
        case RenderInstruction::LineSymbol:
            drawSymbol(ctx, mx, my, angle, *ri.get(), ip.poi_idx_) ;
            break ;
        case RenderInstruction::Caption:
            drawCaption(ctx, mx, my, 0.0, label, *ri.get(), ip.poi_idx_ ) ;
            break ;
        case RenderInstruction::PathText:
            drawCaption(ctx, mx, my, angle, label, *ri.get(), ip.poi_idx_ ) ;
            break ;
        }
    }

    if ( debug_ ) {

        stringstream str ;

        str << key.x() << '/' << key.y() << '/' << (int)key.z()  ;

        cairo_rectangle(cr, 0, 0, 255, 255) ;
        cairo_set_source_rgba(cr, 0, 0, 0, 0.2) ;
        cairo_fill(cr) ;

        string label = str.str() ;

        cairo_select_font_face(cr, "Arial",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_BOLD);

        cairo_set_font_size(cr, 12);

        cairo_text_extents_t extents ;
        cairo_text_extents(cr, label.c_str(), &extents);

        cairo_move_to(cr, 128 - extents.width/2, 128);

        cairo_set_source_rgba(cr, 1, 0, 0, 1);
        cairo_show_text(cr, label.c_str());
    }


    cairo_destroy(cr) ;
}


void Renderer::applySimpleStroke(cairo_t *cr, float stroke_width, uint32_t stroke_clr, const vector<float> &dash_array,
                                 uint cap, uint join)
{

    cairo_set_line_width (cr, stroke_width) ;

    if ( cap == RenderInstruction::RoundCap )
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND) ;
    else if ( cap == RenderInstruction::Square )
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE) ;
    else
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT) ;

    if ( join == RenderInstruction::RoundJoin )
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND) ;
    else if ( join == RenderInstruction::Bevel )
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL) ;
    else
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);

    if ( !dash_array.empty() )  {

        uint n = dash_array.size() ;
        vector<double> dashes(n) ;

        for(int i=0 ; i<n ; i++ )
            dashes[i] = (double)dash_array[i] ;

        cairo_set_dash(cr, dashes.data(), n, 0 ) ;
    }

    double a, r, g, b;
    get_argb_color(stroke_clr, a, r, g, b) ;

    if ( a == 1 ) cairo_set_source_rgb(cr, r, g, b) ;
    else cairo_set_source_rgba(cr, r, g, b, a) ;
}

void Renderer::applySimpleFill(cairo_t *cr, uint32_t fill_clr)
{
    double a, r, g, b;
    get_argb_color(fill_clr, a, r, g, b);

    if ( a == 1 ) cairo_set_source_rgb(cr, r, g, b) ;
    else cairo_set_source_rgba(cr, r, g, b, a) ;
}

void Renderer::drawCircle(RenderingContext &ctx, double px, double py, const RenderInstruction &circle)
{
    cairo_t *cr = ctx.cr_ ;

    double radius = circle.scale_radius_ ? circle.radius_ * ctx.scale_ : circle.radius_ ;

    cairo_matrix_transform_point(&ctx.cmm_, &px, &py) ;

    cairo_arc(cr, px, py, radius, 0, 2*M_PI) ;

    applySimpleFill(cr, circle.fill_) ;
    cairo_fill_preserve(cr) ;
    applySimpleStroke(cr, circle.stroke_width_, circle.stroke_, vector<float>(), RenderInstruction::RoundCap, RenderInstruction::RoundJoin);
    cairo_stroke(cr) ;
}



void Renderer::drawSymbol(RenderingContext &ctx, double px, double py, double angle, const RenderInstruction &symbol, int32_t poi_idx)
{
    cairo_t *cr = ctx.cr_ ;

    double sw, sh ;
    getSymbolSize(symbol, sw, sh) ;

    cairo_rectangle_t extents ;
    cairo_surface_t *surface = renderGraphic(cr, symbol.src_, sw, sh, extents, 1.0) ;

    if ( !surface ) return ;

    double anchor_x = 0.5, anchor_y = 0.5, disp_x = 0, disp_y = 0, rotation = 0 ;

    double ofx =  extents.width * anchor_x ;
    double ofy =  extents.height * anchor_y ;

    cairo_matrix_transform_point(&ctx.cmm_, &px, &py) ;

    if ( symbol.display_ == RenderInstruction::Allways ||
         symbol.display_ == RenderInstruction::IfSpace && ctx.colc_.addLabelBox(px, py, angle, extents.width + collision_extra, extents.height + collision_extra, poi_idx)  )
    {
        cairo_save(cr) ;

        cairo_translate(cr, px, py + symbol.dy_) ;
        cairo_rotate(cr, angle) ;
        cairo_translate(cr, -ofx, -ofy) ;

        cairo_set_source_surface (cr, surface, 0, 0);
        cairo_paint(cr) ;
/*
        cairo_rectangle(cr, 0, 0, extents.width, extents.height);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_stroke(cr) ;
*/
        cairo_restore(cr) ;

    }

    cairo_surface_destroy(surface) ;
}



void cairo_path_from_geometry(cairo_t *cr, const vector<vector<Coord>> &geom, bool close)
{
    for( uint i=0 ; i<geom.size() ; i++ ) {
        const vector<Coord> &ring = geom[i];

        int n = ring.size() ;

        for(int j=0 ; j<n ; j++) {

            double x0 = ring[j].x_ ;
            double y0 = ring[j].y_ ;

            if ( j == 0 ) cairo_move_to(cr, x0, y0) ;
            else cairo_line_to(cr, x0, y0) ;
        }

        if ( close ) cairo_close_path(cr) ;
    }
}

void Renderer::drawLine(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const RenderInstruction &line) {

    cairo_t *cr = ctx.cr_ ;

    cairo_save(cr) ;

    if ( line.src_.empty() ) {
        cairo_save(cr) ;
        cairo_transform(cr, &ctx.cmm_) ;

        double offset = line.dy_ ;

        cairo_device_to_user_distance(cr, &offset, &offset) ;

        if ( line.dy_ == 0 )
           cairo_path_from_geometry(cr, coords, false) ;
        else {
            vector<vector<Coord>> offset_coords ;
            offset_geometry(coords,  offset, offset_coords) ;
            cairo_path_from_geometry(cr, offset_coords, false) ;
        }

        cairo_restore(cr) ;

        float stroke_width = ( line.scale_ != RenderInstruction::None ) ? line.stroke_width_ * ctx.scale_ : line.stroke_width_ ;
        vector<float> stroke_dash_array(line.stroke_dash_array_) ;

        if ( line.scale_ == RenderInstruction::All ) {
            for( auto &v: stroke_dash_array )
                v *= ctx.scale_ ;
        }

        applySimpleStroke(cr, stroke_width, line.stroke_, stroke_dash_array, line.stroke_line_cap_, line.stroke_line_join_) ;

        cairo_stroke(cr) ;

    }
    else {

        cairo_save(cr) ;
        cairo_transform(cr, &ctx.cmm_) ;
        cairo_path_from_geometry(cr, coords, false) ;
        cairo_restore(cr) ;

        double sw, sh ;
        getSymbolSize(line, sw, sh) ;

        cairo_rectangle_t extents ;
        cairo_surface_t *surface = renderGraphic(cr, line.src_, sw, sh, extents, 1.0 );
        if ( !surface ) return ;

        float stroke_width = ( line.scale_ != RenderInstruction::None ) ? line.stroke_width_ * ctx.scale_ : line.stroke_width_ ;
        cairo_set_line_width(cr, stroke_width);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface) ;

        cairo_set_source(cr, pattern);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING) ;

        cairo_stroke(cr) ;

        cairo_surface_destroy(surface) ;


    }

    cairo_restore(cr) ;
}

cairo_surface_t *Renderer::renderGraphic(cairo_t *cr, const std::string &src, double width, double height, cairo_rectangle_t &rect, double scale)
{
    if ( boost::ends_with(src, ".png") ) {

        cairo_surface_t *is = 0 ;

        ResourcePtr cached_data ;
        if ( cache_.find(src, cached_data) )
            is = dynamic_cast<CairoSurface *>(cached_data.get())->surface_ ;
        else {
            is = cairo_image_surface_create_from_png(src.c_str()) ;
            if ( cairo_surface_status(is) != CAIRO_STATUS_SUCCESS ) return nullptr ;
            cache_.save(src, ResourcePtr(new CairoSurface(is))) ;
        }

        if ( is )
        {
            double orig_width = cairo_image_surface_get_width(is) ;
            double orig_height = cairo_image_surface_get_height(is) ;

            double scale = ( height > 0 ) ? height/orig_height : 1.0 ;

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
    else if ( boost::ends_with(src, ".svg") )
    {

        std::shared_ptr<svg::DocumentInstance> doc ;

        ResourcePtr cached_data ;

        if ( cache_.find(src, cached_data) )
            doc = dynamic_cast<SVGDocumentResource *>(cached_data.get())->instance_ ;
        else {
            ifstream strm(src.c_str()) ;
            doc.reset(new svg::DocumentInstance) ;
            if ( !doc->load(strm) ) return nullptr ;
            cache_.save(src, ResourcePtr(new SVGDocumentResource(doc))) ;
        }

        rect.x = 0 ;
        rect.y = 0 ;
        rect.width  = width ;
        rect.height = height ;

        cairo_surface_t *rs = cairo_recording_surface_create( CAIRO_CONTENT_COLOR_ALPHA, &rect) ;

        cairo_t *ctx = cairo_create(rs) ;

        doc->renderToTarget(ctx, 0, 0, width, height, 96) ;

        cairo_destroy(ctx) ;

 //     cairo_surface_write_to_png(rs, "/tmp/surf.png") ;

        return rs ;

    }

    return nullptr ;

}

void Renderer::getSymbolSize(const RenderInstruction &r, double &sw, double &sh) {

    sw = 20 ; sh = 20 ;
    if ( r.symbol_scaling_ == RenderInstruction::CustomSize ) {
        sw = r.symbol_width_ ; sh = r.symbol_height_ ;
    }
    else if ( r.symbol_scaling_ == RenderInstruction::Percent ) {
        sw *= r.symbol_percent_ ;
        sh *= r.symbol_percent_ ;
    }

}

void Renderer::drawArea(RenderingContext &ctx, const std::vector<std::vector<Coord>> &coords, const RenderInstruction &area) {

    cairo_t *cr = ctx.cr_ ;

    cairo_save(cr) ;

    cairo_save(cr) ;
    cairo_transform(cr, &ctx.cmm_) ;
    cairo_path_from_geometry(cr, coords, true) ;
    cairo_restore(cr) ;

    if ( area.src_.empty() ) {

        applySimpleFill(cr, area.fill_) ;
        cairo_fill_preserve(cr) ;

        float stroke_width = ( area.scale_ != RenderInstruction::None ) ? area.stroke_width_ * ctx.scale_ : area.stroke_width_ ;

        applySimpleStroke(cr, stroke_width, area.stroke_, vector<float>(), RenderInstruction::RoundCap, RenderInstruction::RoundJoin) ;
        cairo_stroke(cr) ;
    } else {
        double sw, sh ;
        getSymbolSize(area, sw, sh) ;

        cairo_rectangle_t extents ;
        cairo_surface_t *surface = renderGraphic(cr, area.src_, sw, sh, extents, 1.0);
        if ( !surface ) return ;

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface) ;

        cairo_set_source(cr, pattern);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING) ;

        cairo_fill(cr) ;

        cairo_surface_destroy(surface) ;
    }

    cairo_restore(cr) ;
}

// render text for point geometries

void Renderer::drawCaption(RenderingContext &ctx, double mx, double my, double angle, const std::string &label, const RenderInstruction &caption, int32_t poi_idx)
{
    cairo_t *cr = ctx.cr_ ;

    if ( label.empty() ) return ;

    string family_name ;

    if ( caption.font_family_ == RenderInstruction::Default )
        family_name = "serif" ;
    else if (caption.font_family_ == RenderInstruction::Monospace )
        family_name = "monospace" ;
    else if (caption.font_family_ == RenderInstruction::SansSerif )
        family_name = "sans-serif" ;
    else if (caption.font_family_ == RenderInstruction::Serif )
        family_name = "serif" ;

    // query font face based on provided arguments

    cairo_scaled_font_t *scaled_font = text_engine_.cairo_setup_font(family_name, caption.font_style_, caption.font_size_) ;

    if ( !scaled_font ) return ;

    cairo_set_scaled_font(cr, scaled_font) ;

    // do text shaping based on created font

    cairo_glyph_t *glyphs ;
    int num_glyphs =0 ;

    if ( ! text_engine_.shape_text(label, scaled_font, glyphs, num_glyphs) ) return ;

    // get placement attributes

    double anchor_x = 0.5 ;
    double anchor_y = 0.50 ;
    double disp_x = 0 ;
    double disp_y = caption.dy_;
    double rotation = angle ;

    // compute text placement

    cairo_text_extents_t extents ;

    cairo_glyph_extents(cr, glyphs, num_glyphs, &extents);

    double width = extents.width ;
    double height = extents.height ;

    double ofx =  width * anchor_x ;
    double ofy =  height * anchor_y ;

    cairo_save(cr) ;

    double px = mx, py = my ;

    cairo_matrix_transform_point(&ctx.cmm_, &px, &py) ;

    if ( caption.type_ == RenderInstruction::Caption && caption.symbol_ ) {
        RenderInstruction &symbol = *caption.symbol_ ;

        double sw, sh ;
        getSymbolSize(symbol, sw, sh);

        double offset_x = sw/2.0 + width/2.0 ;
        double offset_y = sh/2.0 + height/2.0 ;

        if ( caption.position_ == RenderInstruction::Above ) disp_y -= offset_y ;
        else if ( caption.position_ == RenderInstruction::Below )
            disp_y += offset_y ;
        else if ( caption.position_ == RenderInstruction::AboveLeft ) {
            disp_y -= offset_y ;
            disp_x -= offset_x ;
        }
        else if ( caption.position_ == RenderInstruction::AboveRight ) {
            disp_y -= offset_y ;
            disp_x += offset_x ;
        }
        else if ( caption.position_ == RenderInstruction::BelowLeft ) {
            disp_y += offset_y ;
            disp_x -= offset_x ;
        }
        else if ( caption.position_ == RenderInstruction::BelowRight ) {
            disp_y += offset_y ;
            disp_x += offset_x ;
        }
        else if ( caption.position_ == RenderInstruction::Left ) {
            disp_x -= offset_x ;
        }
        else if ( caption.position_ == RenderInstruction::Right ) {
            disp_x += offset_x ;
        }
        else if ( caption.position_ == RenderInstruction::Center ) ;

    }


    if ( caption.display_ == RenderInstruction::Allways ||
         caption.display_ == RenderInstruction::IfSpace && ctx.colc_.addLabelBox(px + disp_x, py + disp_y, rotation, width + collision_extra, height + collision_extra, poi_idx)  )
    {
        cairo_save(cr) ;

        cairo_translate(cr, disp_x, disp_y) ;
        cairo_translate(cr, px, py) ;
        cairo_rotate(cr, rotation) ;
        cairo_translate(cr, -ofx, ofy ) ;

        cairo_move_to(cr, 0, 0) ;

        cairo_glyph_path(cr, glyphs, num_glyphs);

        applySimpleStroke(cr, caption.stroke_width_, caption.stroke_, std::vector<float>(), RenderInstruction::Butt, RenderInstruction::Bevel) ;
        cairo_stroke_preserve(cr) ;

        applySimpleFill(cr, caption.fill_) ;
        cairo_fill(cr) ;
/*
        cairo_rectangle(cr, 0, 0, width, height) ;
        cairo_set_source_rgb(cr, 1, 0, 0) ;
        cairo_stroke(cr) ;
*/
        cairo_restore(cr) ;
    }



    cairo_scaled_font_destroy(scaled_font) ;
    cairo_glyph_free(glyphs) ;

    cairo_restore(cr);
}

#ifndef __TEXT_ENGINE_HPP__
#define __TEXT_ENGINE_HPP__

#include "resource_cache.hpp"

class TextEngine {
public:

    TextEngine(ResourceCache &cache, const std::string &lang) ;

    cairo_font_face_t *create_font_face(const std::string &family_name, uint &font_style) ;
    cairo_scaled_font_t *cairo_setup_font(const std::string &family_name, uint font_style, double font_size) ;
    bool shape_text(const std::string &text, cairo_scaled_font_t *sf, cairo_glyph_t *&cglyphs, int &num_glyphs) ;

private:

    ResourceCache &cache_ ;
    std::string lang_ ;
} ;


#endif

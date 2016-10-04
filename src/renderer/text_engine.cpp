#include "text_engine.hpp"
#include "resource_cache.hpp"
#include "theme.hpp"

#include <iostream>

#include <cairo-ft.h>
#include <fontconfig/fcfreetype.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

using namespace std ;


TextEngine::TextEngine(ResourceCache &cache, const string &lang): cache_(cache), lang_(lang)
{

}

cairo_font_face_t *TextEngine::create_font_face(const std::string &family_name, uint &font_style)
{
    cairo_font_face_t *face = 0 ;

    stringstream strm ;
    strm << family_name << '-' << font_style ;
    string key = strm.str() ;

    ResourcePtr d ;
    if ( cache_.find(key, d) ) {
        return dynamic_cast<CairoFontFace *>(d.get())->face_ ;
    }

    // not found create new

    FcPattern* pat = FcPatternCreate() ;

    FcPatternAddString(pat, FC_FAMILY, (const FcChar8*)(family_name.c_str()));

    FcPatternAddString(pat, FC_LANG, (const FcChar8*)(lang_.c_str()));

    if ( font_style == RenderInstruction::Italic ||
         font_style == RenderInstruction::BoldItalic )
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ITALIC) ;
    else
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ROMAN) ;

    if ( font_style == RenderInstruction::BoldItalic ||
         font_style == RenderInstruction::Bold )
        FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_BOLD) ;
    else
        FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_NORMAL) ;

    cairo_font_options_t *font_options =  cairo_font_options_create ();

    // more recent versions of cairo support advanced text rendering options
    cairo_font_options_set_antialias(font_options, CAIRO_ANTIALIAS_DEFAULT) ;

    cairo_ft_font_options_substitute(font_options, pat) ;

    face = cairo_ft_font_face_create_for_pattern(pat) ;

    cairo_font_options_destroy(font_options) ;

    FcPatternDestroy(pat) ;

    cache_.save(key, ResourcePtr(new CairoFontFace(face))) ;

    return face ;
}

cairo_scaled_font_t *TextEngine::cairo_setup_font(const string &family_name, uint font_style, double font_size)
{
    cairo_font_face_t *face = create_font_face(family_name, font_style) ;

    // create scaled font

    cairo_matrix_t ctm, font_matrix;
    cairo_font_options_t *font_options;

    cairo_matrix_init_identity (&ctm);
    cairo_matrix_init_scale (&font_matrix, font_size, font_size);
    font_options = cairo_font_options_create ();
    cairo_font_options_set_hint_style (font_options, CAIRO_HINT_STYLE_NONE);
    cairo_font_options_set_hint_metrics (font_options, CAIRO_HINT_METRICS_OFF);

    cairo_scaled_font_t *scaled_font = cairo_scaled_font_create (face,
                                                                 &font_matrix,
                                                                 &ctm,
                                                                 font_options);

    cairo_font_options_destroy (font_options);

    if ( cairo_scaled_font_status(scaled_font) != CAIRO_STATUS_SUCCESS ) {
        cairo_font_face_destroy (face);
        return nullptr ;
    }

    return scaled_font ;
}


bool TextEngine::shape_text(const std::string &text, cairo_scaled_font_t *sf, cairo_glyph_t *&cglyphs, int &num_glyphs)
{
    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_unicode_funcs(buffer, hb_unicode_funcs_get_default());

    size_t len = text.length() ;


    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR) ;
    hb_buffer_set_script(buffer, HB_SCRIPT_COMMON);
//    hb_buffer_set_language(buffer, hb_language_from_string("el", -1)) ;
    hb_buffer_add_utf8(buffer, text.c_str(), len, 0, len);

    FT_Face face = cairo_ft_scaled_font_lock_face(sf) ;

    if ( face == 0 ) return false ;

    hb_font_t *font = hb_ft_font_create(face, NULL);

    hb_shape(font, buffer, 0 /*features*/, 0 /*num_features*/);

    hb_font_destroy(font);

    cairo_ft_scaled_font_unlock_face(sf) ;

    num_glyphs = hb_buffer_get_length(buffer);

    hb_glyph_info_t *glyphs = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *positions = hb_buffer_get_glyph_positions(buffer, NULL);

    double x = 0, y = 0 ;

    cairo_glyph_t *cairo_glyphs = cairo_glyph_allocate (num_glyphs + 1);

    for (unsigned i=0; i<num_glyphs; i++)
    {
        cairo_glyphs[i].index = glyphs[i].codepoint ;
        cairo_glyphs[i].x = x  ;
        cairo_glyphs[i].y = -positions[i].y_offset/64 + y ;

        x +=  positions[i].x_advance/64;
        y += -positions[i].y_advance/64;
    }

    cglyphs = cairo_glyphs;

    hb_buffer_destroy(buffer) ;
}

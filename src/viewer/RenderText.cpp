#include "Renderer.h"
#include "ResourceCache.h"
#include "CollisionChecker.h"
#include "GeometryUtil.h"

#include <cairo-ft.h>
#include <fontconfig/fcfreetype.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

using namespace std ;

static cairo_font_face_t *create_font_face(ResourceCache &cache, const std::string &familyName, const std::string &fontStyle, const std::string &fontWeight)
{
    cairo_font_face_t *face = 0 ;

    string key = familyName + '-' + fontStyle + '-' + fontWeight ;

    ResourceCache::Data d ;
    if ( cache.find(key, d) ) return d.font_face_ ;

    FcPattern* pat = FcPatternCreate() ;

    FcPatternAddString(pat, FC_FAMILY, (const FcChar8*)(familyName.c_str()));

    if ( fontStyle == "italic" )
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ITALIC) ;
    else if ( fontStyle == "oblique" )
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_OBLIQUE) ;
    else
        FcPatternAddInteger(pat, FC_SLANT, FC_SLANT_ROMAN) ;

    if ( fontWeight == "bold" )
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

    d.font_face_ = face ;

    cache.save(key, d) ;

    return face ;

}

cairo_scaled_font_t *cairo_setup_font(ResourceCache &cache, const string &familyName, const string &fontStyle, const string &fontWeight, double fontSize)
{

    cairo_font_face_t *face = create_font_face(cache, familyName, fontStyle, fontWeight) ;

    // create scaled font

    cairo_matrix_t ctm, font_matrix;
    cairo_font_options_t *font_options;

    cairo_matrix_init_identity (&ctm);
    cairo_matrix_init_scale (&font_matrix, fontSize, fontSize);
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
        return 0 ;
    }

    return scaled_font ;

}


bool shape_text(const std::string &text, cairo_scaled_font_t *sf, cairo_glyph_t *&cglyphs, int &num_glyphs)
{
    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_unicode_funcs(buffer, hb_unicode_funcs_get_default());

    size_t len = text.length() ;


    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR) ;
    hb_buffer_set_script(buffer, HB_SCRIPT_COMMON);
    hb_buffer_set_language(buffer, hb_language_from_string("el", -1)) ;
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


extern double clip_percent (double percent) ;

static void collect_label_positions(const gaiaGeomCollPtr &geom, const cairo_matrix_t &cmm, vector<double> &positions )
{
    for( gaiaPointPtr p = geom->FirstPoint ; p != NULL ; p = p->Next )
    {
        double px = p->X, py = p->Y ;
        positions.push_back(px) ;
        positions.push_back(py) ;
    }

    sampleLinearGeometry(positions, geom, cmm) ;

}


void Renderer::paintText(RenderingContext *ctx, const Feature &f, const sld::TextSymbolizer &smb)
{
    cairo_t *cr = ctx->cr ;

    // We do not do complex halo fill
    // Halo is simulated by stroking text with line width equal to radius

    if ( smb.halo )
    {
        double haloRadius ;
        if ( smb.halo->radius.eval(f).toNumber(haloRadius) )
        {
            unsigned int haloColor = 0;
            double haloOpacity = 1.0 ;

            if ( smb.halo->fill )
            {
                smb.halo->fill->fill.eval(f).toColor(haloColor) ;
                smb.halo->fill->fillOpacity.eval(f).toNumber(haloOpacity) ;
            }

            double r, g, b;

            r = ((haloColor >> 16) & 0xff) / 255.0;
            g = ((haloColor >> 8) & 0xff) / 255.0;
            b = ((haloColor >> 0) & 0xff) / 255.0;

            haloOpacity = clip_percent(haloOpacity) ;

            if ( haloOpacity == 1.0 ) cairo_set_source_rgb(cr, r, g, b) ;
            else cairo_set_source_rgba(cr, r, g, b, haloOpacity) ;

            cairo_set_line_width(cr, haloRadius) ;

            cairo_stroke_preserve(cr) ;
        }
    }

    if ( smb.fill ) {
        applyFill(cr, f, *smb.fill, 1.0, ctx->scale) ;
        cairo_fill(cr) ;
    }
    else
    {
        cairo_set_source_rgb(cr, 0, 0, 0) ;
        cairo_fill(cr) ;
    }
}

// render text for point geometries

void Renderer::renderPointsText(RenderingContext *ctx, const FeatureCollection &col, const sld::TextSymbolizer &smb)
{
    cairo_t *cr = ctx->cr ;

    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;

        string label ;

        smb.label.eval(f).toString(label) ;

        if ( label.empty() ) continue ;

        // get text attributes

        string familyName = "Times";
        string fontStyle =  "normal";
        string fontWeight = "normal";
        double fontSize = 10;

        smb.fontFamily.eval(f).toString(familyName) ;
        smb.fontSize.eval(f).toNumber(fontSize) ;
        smb.fontStyle.eval(f).toString(fontStyle) ;
        smb.fontWeight.eval(f).toString(fontWeight) ;

        // query font face based on provided arguments

        cairo_scaled_font_t *scaled_font = cairo_setup_font(*cache.get(), familyName, fontStyle, fontWeight, fontSize) ;

        if ( !scaled_font ) return ;

        cairo_set_scaled_font(cr, scaled_font) ;

        // do text shaping based on created font

        cairo_glyph_t *glyphs ;
        int num_glyphs =0 ;

        shape_text(label, scaled_font, glyphs, num_glyphs) ;

        // get placement attributes

        double anchorPointX = 0.0 ;
        double anchorPointY = 0.0 ;
        double displacementX = 0 ;
        double displacementY = 0 ;
        double rotation = 0 ;

        if ( smb.pointPlacement )
        {
            smb.pointPlacement->anchorPointX.eval(f).toNumber(anchorPointX) ;
            smb.pointPlacement->anchorPointY.eval(f).toNumber(anchorPointY) ;
            smb.pointPlacement->displacementX.eval(f).toNumber(displacementX) ;
            smb.pointPlacement->displacementY.eval(f).toNumber(displacementY) ;
            smb.pointPlacement->rotation.eval(f).toNumber(rotation) ;
        }

        // compute text placement

        cairo_text_extents_t extents ;

        cairo_glyph_extents(cr, glyphs, num_glyphs, &extents);

        double width = extents.width ;
        double height = extents.height ;

        double ofx =  width * anchorPointX ;
        double ofy =  height * anchorPointY ;

        cairo_save(cr) ;

        vector<double> pts ;
        collect_label_positions(f.geom, ctx->cmm, pts);

        for( int j=0 ; j<pts.size() ; j+=2 )
        {
            double px = pts[i], py = pts[i+1] ;

            cairo_matrix_transform_point(&ctx->cmm, &px, &py) ;

            if ( ctx->colc->addLabelBox(px + displacementX, py + displacementY, rotation * M_PI/180.0, width, height)  )
            {
                cairo_save(cr) ;

                cairo_translate(cr, displacementX, displacementY) ;
                cairo_translate(cr, px, py) ;
                cairo_rotate(cr, rotation * M_PI/180.0) ;
                cairo_translate(cr, -ofx, ofy ) ;

                cairo_move_to(cr, 0, 0) ;

                cairo_glyph_path(cr, glyphs, num_glyphs);

                cairo_restore(cr) ;
            }
        }


        paintText(ctx, f, smb) ;

        cairo_scaled_font_destroy(scaled_font) ;
        cairo_glyph_free(glyphs) ;

        cairo_restore(cr);
    }
}

extern bool isClockwise(gaiaGeomColl *p1, gaiaGeomColl *p2) ;

// render text along lines

void Renderer::renderLinesText(RenderingContext *ctx, const FeatureCollection &col, const sld::TextSymbolizer &smb)
{
    cairo_t *cr = ctx->cr ;

    for(int i=0 ; i<col.features.size() ; i++ )
    {
        const Feature &f = *col.features[i] ;

        string label ;

        smb.label.eval(f).toString(label) ;

        if ( label.empty() ) continue ;

        string familyName = "Times";
        string fontStyle =  "normal";
        string fontWeight = "normal";
        double fontSize = 10;

        // get texture attributes

        smb.fontFamily.eval(f).toString(familyName) ;
        smb.fontSize.eval(f).toNumber(fontSize) ;
        smb.fontStyle.eval(f).toString(fontStyle) ;
        smb.fontWeight.eval(f).toString(fontWeight) ;

        // query font face based on requested font

        cairo_scaled_font_t *scaled_font = cairo_setup_font(*cache.get(), familyName, fontStyle, fontWeight, fontSize) ;

        if ( !scaled_font ) return ;

        cairo_set_scaled_font(cr, scaled_font) ;

        // shape text

        cairo_glyph_t *glyphs ;
        int num_glyphs ;

        if ( !shape_text(label, scaled_font, glyphs, num_glyphs) ) continue ;

        // collect position and size for each glyph

        vector<double> offsets ;
        vector<cairo_text_extents_t> glyph_extents ;

        offsets.resize(num_glyphs, 0) ;
        glyph_extents.resize(num_glyphs) ;

        double offset = 0 ;
        for(int i=0 ; i<num_glyphs ; i++)
        {
            offsets[i] = glyphs[i].x ;

            cairo_text_extents_t &extents = glyph_extents[i] ;
            cairo_scaled_font_glyph_extents(scaled_font, &glyphs[i], 1, &extents);

            offset += extents.x_advance ;
        }

        // get placement attributes

        double perpendicularOffset = 0 ;
        double gap = 0 ;
        double initialGap = 0 ;
        bool isRepeated = false ;

        if ( smb.linePlacement )
        {
            smb.linePlacement->perpendicularOffset.eval(f).toNumber(perpendicularOffset) ;
            smb.linePlacement->isRepeated.eval(f).toBoolean(isRepeated) ;
            smb.linePlacement->gap.eval(f).toNumber(gap) ;
            smb.linePlacement->initialGap.eval(f).toNumber(initialGap) ;
        }

        // compute the bounding box of the shaped label

        cairo_text_extents_t extents ;
        cairo_glyph_extents(cr, glyphs, num_glyphs, &extents) ;

        double width = extents.width;
        double height = extents.height ;
        double baseline = extents.y_bearing ;

        double ofx =  width/2.0 ;
        double ofy =  (- baseline)/2.0 ;

        // this is needed to center label vertically on the line

        perpendicularOffset += ofy ;

        vector<double> positions ;

        cairo_save(cr) ;

        if ( isRepeated && gap != 0)
        {
            sampleLinearGeometry(positions, f.geom, ctx->cmm, gap, initialGap, offsets) ;
        }
        else {

            sampleLinearGeometry(positions, f.geom, ctx->cmm, 0.0, width/2, offsets) ;

            sampleLinearGeometry(positions, f.geom, ctx->cmm, 0, width/2 + 10, offsets) ;

            sampleLinearGeometry(positions, f.geom, ctx->cmm, 0, width/2 - 10, offsets) ;

        }



        int gn = num_glyphs ;
        int nsamples = positions.size() / 3 / gn ;

        for( int j=0 ; j<nsamples ; j ++ )
        {
            // collect glyph boxes to perform collision checking
            // and also measure angle between consecutive characters

            vector<OBB> boxes ;

            double pa = 0, ca ;
            bool maxAngleExceeded = false ;


            for( int k=0 ; k<gn ; k++)
            {
                int idx = (j*gn + k)*3 ;

                double px = positions[idx] ;
                double py = positions[idx+1] ;
                double angle = positions[idx+2] ;

                double dx = perpendicularOffset * sin(-angle) ;
                double dy = perpendicularOffset * cos(-angle) ;

                cairo_matrix_transform_point(&ctx->cmm, &px, &py) ;

                boxes.push_back(OBB(px + dx, py + dy + glyph_extents[k].y_bearing, angle, glyph_extents[k].width, glyph_extents[k].height)) ;

                if ( k > 0 )
                {
                    if ( fabs(angle - pa) > M_PI/6 )
                    {
                        maxAngleExceeded = true ;
                        break ;
                    }
                }

                pa = angle ;
            }

            if ( !maxAngleExceeded && ctx->colc->addLabelBox(boxes) )
            {
                for( int k=0 ; k<gn ; k++)
                {
                    int idx = (j*gn + k)*3 ;


                    double px = positions[idx] ;
                    double py = positions[idx+1] ;
                    double angle = positions[idx+2] ;

                    double dx = perpendicularOffset * sin(-angle) ;
                    double dy = perpendicularOffset * cos(-angle) ;

                    cairo_matrix_transform_point(&ctx->cmm, &px, &py) ;

                    // we do the rendering directly with cairo since each glyph should be rendered separately


                    cairo_glyph_t glyph ;
                    glyph.index = glyphs[k].index ;
                    glyph.x = 0 ;
                    glyph.y = 0 ;

                    cairo_save(cr) ;
                    cairo_set_scaled_font (cr, scaled_font);

                    cairo_translate(cr, px + dx, py + dy) ;

                //        cairo_rectangle(cr, -1, -1, 3, 3) ;

                    cairo_rotate(cr, angle ) ;

                    cairo_move_to(cr, 0, 0) ;
                    cairo_glyph_path(cr, &glyph, 1) ;
/*
                        cairo_save(cr) ;
                        cairo_set_source_rgb(cr, 0, 0, 0) ;
                        cairo_rectangle(cr, 0,glyph_extents[k].y_bearing, glyph_extents[k].width, glyph_extents[k].height) ;
                        cairo_stroke(cr) ;
                        cairo_restore(cr) ;
*/
                    cairo_restore(cr) ;
                }
            }

        }

        paintText(ctx, f, smb) ;

        cairo_restore(cr) ;

        cairo_scaled_font_destroy(scaled_font) ;
        cairo_glyph_free(glyphs) ;

    }


}

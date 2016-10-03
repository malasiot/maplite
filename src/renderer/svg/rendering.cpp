#include "rendering.hpp"
#include "document.hpp"
#include "util.hpp"

#include <map>

#include <math.h>
#include <boost/filesystem.hpp>

#include <cairo-ft.h>
#include <fontconfig/fcfreetype.h>

using namespace std ;

namespace svg {

class RenderingContext
{
public:

    enum RenderingMode { Display, BoundingBox, Cliping } ;

    RenderingContext(cairo_t *cr_): cr(cr_) {
        dpix = 92 ; dpiy = 92 ;
        renderingMode = Display ;
    }

    void pushState(const Style &) ;
    void popState() ;
    void pushTransform(const Transform &) ;
    void popTransform() ;

    void extentBoundingBox(double x1, double x2, double y1, double y2) ;

    void populateRefs(const ElementPtr &root)  ;
    ElementPtr lookupRef(const std::string &name) ;

    void setFilePath(const std::string &path) {
        filePath = path ;
    }

    cairo_t *cr ;
    std::deque<Style> state ;
    std::deque<Transform> transforms ;
    std::deque<ViewBox> viewboxes ;
    double dpix, dpiy ;
    ViewBox obbox ;
    RenderingMode renderingMode ;
    std::map<std::string, ElementPtr> refs_ ;
    Transform view2dev ;
    double ctx, cty ;
    string filePath ;
    float docWidthHint, docHeightHint ;
} ;



////////////////////////////////////////////////////////////////////////////////////

ElementPtr RenderingContext::lookupRef(const std::string &name)
{
    map<string, ElementPtr>::const_iterator it = refs_.find(name) ;

    if ( it == refs_.end() )	return nullptr ;
    else return (*it).second ;
}

void RenderingContext::populateRefs(const ElementPtr &root)
{
    if ( root->getType() == Element::DocumentElement )
    {
        Document *pElem = dynamic_cast<Document *>(root.get()) ;

        for( int i=0 ; i<pElem->children_.size() ; i++ )
        {
            ElementPtr el = pElem->children_[i] ;

            string id = el->id_ ;

            if ( !id.empty() ) refs_['#' + id] = el ;

            if ( el->getType() == Element::DocumentElement ||
                 el->getType() == Element::GroupElement ||
                 el->getType() == Element::DefsElement ) populateRefs(el) ;
        }
    }
    else if ( root->getType() == Element::GroupElement )
    {
        Group *pElem = dynamic_cast<Group *>(root.get()) ;

        for( int i=0 ; i<pElem->children_.size() ; i++ )
        {
            ElementPtr el = pElem->children_[i] ;

            string id = el->id_ ;

            if ( !id.empty() ) refs_['#' + id] = el ;

            if ( el->getType() == Element::DocumentElement ||
                 el->getType() == Element::GroupElement ||
                 el->getType() == Element::DefsElement ) populateRefs(el) ;
        }
    }
    else if ( root->getType() == Element::DefsElement )
    {
        Defs *pElem = dynamic_cast<Defs *>(root.get()) ;

        for( int i=0 ; i<pElem->children_.size() ; i++ )
        {
            ElementPtr el = pElem->children_[i] ;

            string id = el->id_ ;

            if ( !id.empty() ) refs_['#' + id] = el ;

            if ( el->getType() == Element::DocumentElement ||
                 el->getType() == Element::GroupElement ||
                 el->getType() == Element::DefsElement ) populateRefs(el) ;
        }
    }
}

void RenderingContext::extentBoundingBox(double x1, double y1, double x2, double y2)
{
    obbox.x = x1 ;
    obbox.y = y1 ;
    obbox.w = x2 - x1 ;
    obbox.h = y2 - y1 ;
}

void RenderingContext::pushState(const Style &st)
{
    if ( state.empty() ) state.push_back(Style()) ;
    else state.push_back(state.back()) ;

    Style &style = state.back() ;

    style.resetNonInheritable() ;

    for( int i=0 ; i<st.flags_.size() ; i++ )
    {
        Style::Flag flag = st.flags_[i] ;

        if ( flag == Style::FillState )
        {
            style.fill_paint_type_ = st.fill_paint_type_ ;
            if ( style.fill_paint_type_ == Style::SolidColorPaint )
              style.fill_paint_.clr_ = st.fill_paint_.clr_ ;
            else if ( style.fill_paint_type_ == Style::PaintServerPaint )
                style.fill_paint_.paint_server_id_ = strdup(st.fill_paint_.paint_server_id_) ;

        }
        else if ( flag == Style::FillOpacityState )
        {
            style.fill_opacity_ = st.fill_opacity_ ;
        }
        else if ( flag == Style::FillRuleState )
        {
            style.fill_rule_ = st.fill_rule_ ;
        }
        else if ( flag == Style::StrokeState )
        {
            style.stroke_paint_type_ = st.stroke_paint_type_ ;
            if ( style.stroke_paint_type_ == Style::SolidColorPaint )
              style.stroke_paint_.clr_ = st.stroke_paint_.clr_ ;
            else if ( style.stroke_paint_type_ == Style::PaintServerPaint )
                style.stroke_paint_.paint_server_id_ = strdup(st.stroke_paint_.paint_server_id_) ;

        }
        else if ( flag == Style::StrokeWidthState )
        {
            style.stroke_width_ = st.stroke_width_ ;
        }
        else if ( flag == Style::StrokeDashArrayState )
        {
            style.dash_array_ = st.dash_array_ ;
            style.solid_stroke_ = st.solid_stroke_ ;
        }
        else if ( flag == Style::StrokeOpacityState )
        {
            style.stroke_opacity_ = st.stroke_opacity_ ;
        }
        else if ( flag == Style::OpacityState )
        {
            style.opacity_ = st.opacity_ ;
        }
        else if ( flag == Style::StrokeDashOffsetState )
        {
            style.dash_offset_ = st.dash_offset_ ;
        }
        else if ( flag == Style::StrokeLineCapState )
        {
            style.line_cap_ = st.line_cap_ ;
        }
        else if ( flag == Style::StrokeLineJoinState )
        {
            style.line_join_ = st.line_join_ ;
        }
        else if ( flag == Style::DisplayState )
        {
            style.display_mode_ = st.display_mode_ ;
        }
        else if ( flag == Style::VisibilityState )
        {
            style.visibility_mode_ = st.visibility_mode_ ;
        }
        else if ( flag == Style::FontFamilyState )
        {
            style.font_family_ = st.font_family_ ;
        }
        else if ( flag == Style::FontSizeState )
        {
            style.font_size_ = st.font_size_ ;
        }
        else if ( flag == Style::FontStyleState )
        {
            style.font_style_ = st.font_style_ ;
        }
        else if ( flag == Style::FontWeightState )
        {
            style.font_weight_ = st.font_weight_ ;
        }
        else if ( flag == Style::TextDecorationState )
        {
            style.text_decoration_ = st.text_decoration_ ;
        }
        else if ( flag == Style::TextAnchorState )
        {
            style.text_anchor_ = st.text_anchor_ ;
        }
        else if ( flag == Style::TextRenderingState )
        {
            style.text_rendering_quality_ = st.text_rendering_quality_ ;
        }
        else if ( flag == Style::ShapeRenderingState )
        {
            style.shape_rendering_quality_ = st.shape_rendering_quality_ ;
        }
        else if ( flag == Style::ClipPathState )
        {
            style.clip_path_id_ = st.clip_path_id_ ;
        }
    }
}


void RenderingContext::popState()
{
    state.pop_back() ;
}

void RenderingContext::pushTransform(const Transform &trs)
{
    if ( this->transforms.empty() ) transforms.push_back(trs) ;
//	else transforms.push(SVGTransform::Multiply(transforms.top(), trs)) ;
    else transforms.push_back(trs) ;
}

void RenderingContext::popTransform()
{
    transforms.pop_back() ;
}


static void cairo_push_transform(cairo_t *cr, const Transform &tr)
{
    cairo_matrix_t matrix;

    cairo_matrix_init (&matrix, tr.m_[0], tr.m_[1], tr.m_[2], tr.m_[3], tr.m_[4], tr.m_[5]) ;
    cairo_transform (cr, &matrix);
}


void Document::render(RenderingContext *ctx)
{
    ctx->pushState(style_) ;

    double xx, yy, sw, sh ;

    if ( x_.unknown() ) xx = 0 ;
    else xx = x_.toPixels(ctx, Length::HorzDir) ;

    if ( y_.unknown() ) yy = 0 ;
    else yy = y_.toPixels(ctx, Length::HorzDir) ;

    if ( width_.unknown() )
    {
        if ( parent_ )
            sw = Length(Length::PercentageLengthType, 1.0, true).toPixels(ctx, Length::HorzDir) ;
        else
            sw = ctx->docWidthHint ;
    }
    else
        sw = width_.toPixels(ctx, Length::HorzDir) ;

    if ( height_.unknown() )
    {
        if ( parent_ )
            sh = Length(Length::PercentageLengthType, 1.0, true).toPixels(ctx, Length::HorzDir) ;
        else
            sh = ctx->docHeightHint ;
    }
    else
        sh = height_.toPixels(ctx, Length::HorzDir) ;

    ViewBox vbox = view_box_ ;

    if ( vbox.w == 0 ) vbox.w = sw ;
    if ( vbox.h == 0 ) vbox.h = sh ;

    ctx->viewboxes.push_back(vbox) ;

    Transform trs = preserve_aspect_ratio_.getViewBoxTransform(sw, sh, vbox.w, vbox.h, vbox.x, vbox.y) ;

    ctx->view2dev = trs ;

    cairo_t *cr = ctx->cr ;

    cairo_save(cr) ;

    cairo_push_transform(cr, trs) ;

    bool hasOverflow = style_.hasFlag(Style::OverflowState) ;

    if ( ( hasOverflow && style_.overflow_ == false ) || ( !hasOverflow && parent_ == NULL ))
    {
    //	cairo_rectangle(ctx->cr, px, py, sw, sh) ;
//		cairo_clip(ctx->cr) ;
    }

    for( int i=0 ; i<children_.size() ; i++ )
    {
        children_[i]->render(ctx) ;
    }

    cairo_restore(ctx->cr) ;

    ctx->popState() ;

    ctx->viewboxes.pop_back();
}


static void cairo_apply_clip_path(RenderingContext *ctx, ClipPath *cp)
{
    ctx->pushState(cp->style_) ;
    ctx->pushTransform(cp->trans_) ;

    ctx->renderingMode = RenderingContext::Cliping ;

    for( int i=0 ; i<cp->children_.size() ; i++ )
    {
        cp->children_[i]->render(ctx) ;
    }

    cairo_clip(ctx->cr) ;
    ctx->renderingMode = RenderingContext::Display ;

    ctx->popTransform() ;
    ctx->popState() ;
}



static void cairo_apply_linear_gradient(RenderingContext *ctx, LinearGradient *lg, float fop)
{
    lg->resolveHRef(ctx) ;

    cairo_t *cr = ctx->cr;
    cairo_pattern_t *pattern;
    cairo_matrix_t matrix;

    Length x1 = lg->x1_, y1 = lg->y1_, x2 = lg->x2_, y2 = lg->y2_ ;

    if ( x1.unknown() ) x1 = Length(Length::PercentageLengthType, 0) ;
    if ( y1.unknown() ) y1 = Length(Length::PercentageLengthType, 0) ;
    if ( x2.unknown() ) x2 = Length(Length::PercentageLengthType, 1.0) ;
    if ( y2.unknown() ) y2 = Length(Length::PercentageLengthType, 0.0) ;

    LinearGradient::GradientUnits gu = lg->gradient_units_ ;
    LinearGradient::SpreadMethod sm  = lg->spread_method_ ;

    double ix1, iy1, ix2, iy2 ;

    ix1 = ( gu == GradientElement::ObjectBoundingBox ) ?
                x1.value_in_specified_units_ : x1.toPixels(ctx, Length::HorzDir) ;

    iy1 = ( gu == GradientElement::ObjectBoundingBox ) ?
                y1.value_in_specified_units_ : y1.toPixels(ctx, Length::VertDir) ;

    iy2 = ( gu == GradientElement::ObjectBoundingBox ) ?
                y2.value_in_specified_units_ : y2.toPixels(ctx, Length::VertDir) ;

    ix2 = ( gu == GradientElement::ObjectBoundingBox ) ?
                x2.value_in_specified_units_ : x2.toPixels(ctx, Length::HorzDir) ;

    pattern = cairo_pattern_create_linear ( ix1, iy1, ix2, iy2 ) ;

    cairo_matrix_init (&matrix,
                        lg->trans_.m_[0], lg->trans_.m_[1],
                        lg->trans_.m_[2], lg->trans_.m_[3], lg->trans_.m_[4], lg->trans_.m_[5]);

    if (lg->gradient_units_ == GradientElement::ObjectBoundingBox ) {
        cairo_matrix_t bboxmatrix;
        cairo_matrix_init (&bboxmatrix, ctx->obbox.w, 0, 0, ctx->obbox.h, ctx->obbox.x, ctx->obbox.y);
        cairo_matrix_multiply (&matrix, &matrix, &bboxmatrix);
    }

    cairo_matrix_invert (&matrix);
    cairo_pattern_set_matrix (pattern, &matrix);

    if (lg->spread_method_ == LinearGradient::ReflectSpreadMethod )
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REFLECT);
    else if (lg->spread_method_ == LinearGradient::RepeatSpreadMethod)
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    else
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_PAD);

    unsigned int gopac = ctx->state.back().opacity_ ;

    for ( int i = 0; i < lg->stops_.size() ; i++ )
    {
        Stop &stop = lg->stops_[i] ;

        unsigned int rgb = stop.stop_color_ ;
        unsigned int opacity = stop.stop_opacity_ ;

        cairo_pattern_add_color_stop_rgba (pattern, stop.offset_.value_in_specified_units_,
                                           ((rgb >> 16) & 0xff) / 255.0,
                                           ((rgb >> 8) & 0xff) / 255.0,
                                           ((rgb >> 0) & 0xff) / 255.0,
                                           fop * gopac * opacity / 255.0 / 255.0 );
    }

    cairo_set_source (cr, pattern);
    cairo_pattern_destroy (pattern);
}

static void cairo_apply_radial_gradient(RenderingContext *ctx, RadialGradient *rg, float fop)
{
    rg->resolveHRef(ctx) ;

    cairo_t *cr = ctx->cr;
    cairo_pattern_t *pattern;
    cairo_matrix_t matrix;

    double ifx, ify, icx, icy, ir ;

    Length cx = rg->cx_, cy = rg->cy_, fx = rg->fx_, fy = rg->fy_, r = rg->r_ ;

    if ( cx.unknown() ) cx = Length(Length::PercentageLengthType, 0.5) ;
    if ( cy.unknown() ) cy = Length(Length::PercentageLengthType, 0.5) ;
    if ( fx.unknown() ) fx = cx ;
    if ( fy.unknown() ) fy = cy ;
    if ( r.unknown() ) r = Length(Length::PercentageLengthType, 0.5) ;

    RadialGradient::GradientUnits gu = rg->gradient_units_ ;
    RadialGradient::SpreadMethod sm  = rg->spread_method_ ;

    icx = ( gu == GradientElement::ObjectBoundingBox ) ?
                cx.value_in_specified_units_ : cx.toPixels(ctx, Length::HorzDir) ;

    icy = ( gu == GradientElement::ObjectBoundingBox ) ?
                cy.value_in_specified_units_ : cy.toPixels(ctx, Length::VertDir) ;

    ifx = ( gu == GradientElement::ObjectBoundingBox ) ?
                fx.value_in_specified_units_ : fx.toPixels(ctx, Length::HorzDir) ;

    ify = ( gu == GradientElement::ObjectBoundingBox ) ?
                fy.value_in_specified_units_ : fy.toPixels(ctx, Length::HorzDir) ;

    ir =  ( gu == GradientElement::ObjectBoundingBox ) ?
                r.value_in_specified_units_ : r.toPixels(ctx, Length::HorzDir) ;

    pattern = cairo_pattern_create_radial ( ifx, ify, 0.0, icx, icy, ir) ;

    cairo_matrix_init (&matrix,
                        rg->trans_.m_[0], rg->trans_.m_[1],
                        rg->trans_.m_[2], rg->trans_.m_[3], rg->trans_.m_[4], rg->trans_.m_[5]);

    if ( gu == GradientElement::ObjectBoundingBox ) {
        cairo_matrix_t bboxmatrix;
        cairo_matrix_init (&bboxmatrix, ctx->obbox.w, 0, 0, ctx->obbox.h, ctx->obbox.x, ctx->obbox.y);
        cairo_matrix_multiply (&matrix, &matrix, &bboxmatrix);
    }

    cairo_matrix_invert (&matrix);
    cairo_pattern_set_matrix (pattern, &matrix);

    if ( sm == LinearGradient::ReflectSpreadMethod )
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REFLECT);
    else if ( sm == LinearGradient::RepeatSpreadMethod)
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    else
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_PAD);

    unsigned int gopac = ctx->state.back().opacity_ ;

    for ( int i = 0; i < rg->stops_.size() ; i++ )
    {
        Stop &stop = rg->stops_[i] ;

        unsigned int rgb = stop.stop_color_ ;
        unsigned int opacity = stop.stop_opacity_ ;

        cairo_pattern_add_color_stop_rgba (pattern, stop.offset_.value_in_specified_units_,
                                           ((rgb >> 16) & 0xff) / 255.0,
                                           ((rgb >> 8) & 0xff) / 255.0,
                                           ((rgb >> 0) & 0xff) / 255.0,
                                           fop * gopac * opacity / 255.0 / 255.0 );
    }

    cairo_set_source (cr, pattern);
    cairo_pattern_destroy (pattern);
}

static void cairo_apply_pattern(RenderingContext *ctx, Pattern *pat, float fop)
{
    cairo_t *cr = ctx->cr, *cr_pattern;
    cairo_pattern_t *pattern;

    double ipw, iph, ipx, ipy ;

    Length px = pat->x_, py = pat->y_, pw = pat->width_, ph = pat->height_ ;

    if ( px.unknown() ) px = Length(Length::NumberLengthType, 0.0) ;
    if ( py.unknown() ) py = Length(Length::NumberLengthType, 0.0) ;
    if ( pw.unknown() ) pw = Length(Length::NumberLengthType, 0.0) ;
    if ( ph.unknown() ) ph = Length(Length::NumberLengthType, 0.0) ;

    ipw = pw.toPixels(ctx, Length::HorzDir) ;
    iph = ph.toPixels(ctx, Length::VertDir) ;
    ipx = px.toPixels(ctx, Length::HorzDir) ;
    ipy = py.toPixels(ctx, Length::VertDir) ;

    double bbwscale, bbhscale, scwscale, schscale;

    int pbw, pbh;
    cairo_matrix_t affine, caffine, taffine ;

    if (pat->pattern_units_ == Pattern::ObjectBoundingBox)
    {
        bbwscale = ctx->obbox.w;
        bbhscale = ctx->obbox.h;
    }
    else {
        bbwscale = 1.0;
        bbhscale = 1.0;
    }

    cairo_matrix_init_identity(&affine) ;
    cairo_matrix_init_identity(&caffine) ;

    Transform &trs = ctx->transforms.back() ;
    cairo_matrix_t patm;
    cairo_matrix_init(&patm, pat->trans_.m_[0], pat->trans_.m_[1], pat->trans_.m_[2], pat->trans_.m_[3],
            pat->trans_.m_[4], pat->trans_.m_[5]) ;
    //cairo_get_matrix(ctx->cr, &trsm) ;

    cairo_matrix_multiply(&taffine, &patm, &caffine) ;

    scwscale = sqrt (taffine.xx * taffine.xx + taffine.xy * taffine.xy);
    schscale = sqrt (taffine.yx * taffine.yx + taffine.yy * taffine.yy);

    pbw = ipw * bbwscale * scwscale;
    pbh = iph * bbhscale * schscale;

    scwscale = (double) pbw / (double) (ipw * bbwscale);
    schscale = (double) pbh / (double) (iph * bbhscale);

    cairo_surface_t *surface = cairo_surface_create_similar (cairo_get_target (cr),
                                                             CAIRO_CONTENT_COLOR_ALPHA, pbw, pbh);
    cr_pattern = cairo_create (surface);

    if (pat->pattern_units_ == Pattern::ObjectBoundingBox) {
        /* subtract the pattern origin */
        affine.x0 = ctx->obbox.x + ipx * ctx->obbox.w;
        affine.y0 = ctx->obbox.y + ipy * ctx->obbox.h;
    } else {
        /* subtract the pattern origin */
        affine.x0 = ipx;
        affine.y0 = ipy;
    }

    cairo_matrix_multiply(&affine, &affine, &patm) ;

    double sw = ipw * bbwscale;
    double sh = iph * bbhscale;

    Transform prs ;

    if ( pat->view_box_.w == 0 ) pat->view_box_.w = sw ;
    if ( pat->view_box_.h == 0 ) pat->view_box_.h = sh ;

    if ( pat->view_box_.w != 0.0 && pat->view_box_.h != 0.0 )
    {
        ViewBox vbox = pat->view_box_ ;

        double ofx = 0, ofy = 0 ;
        double aspScaleX = 1.0 ;
        double aspScaleY = 1.0 ;

        if ( pat->preserve_aspect_ratio_.view_box_align_ != PreserveAspectRatio::NoViewBoxAlign )
        {
            pat->preserve_aspect_ratio_.constrainViewBox(sw, sh, vbox) ;

            aspScaleX = vbox.w/pat->view_box_.w ;
            aspScaleY = vbox.h/pat->view_box_.h ;

            ofx = vbox.x;
            ofy = vbox.y ;
        }
        else {
            aspScaleX = sw/vbox.w ;
            aspScaleY = sh/vbox.h ;

        }

        prs = Transform::translation(-ipx, -ipy) ;
        prs = Transform::multiply(prs, Transform::scaling(aspScaleX, aspScaleY)) ;
        prs = Transform::multiply(prs, Transform::translation(ofx , ofy)) ;

    }

    RenderingContext pctx(cr_pattern) ;

    pctx.refs_ = ctx->refs_ ;

    pctx.pushTransform(prs) ;

    cairo_matrix_init(&affine, prs.m_[0], prs.m_[1], prs.m_[2], prs.m_[3], prs.m_[4], prs.m_[5]) ;
    //cairo_matrix_multiply(&affine, &affine, &patm) ;

    cairo_transform(cr_pattern, &affine) ;

    for( int i=0 ; i<pat->children_.size() ; i++ )
        pat->children_[i]->render(&pctx) ;

    pattern = cairo_pattern_create_for_surface (surface);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

    // cairo_matrix_invert(&patm) ;
    cairo_pattern_set_matrix (pattern, &patm);
    cairo_pattern_set_filter (pattern, CAIRO_FILTER_BEST);

    cairo_set_source (cr, pattern);

    cairo_pattern_destroy (pattern);

    cairo_destroy (cr_pattern);

    cairo_surface_destroy (surface);
}

static void
cairo_set_shape_antialias (cairo_t * cr, Style::ShapeQuality aa)
{
    if (Style::AutoShapeQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_DEFAULT);
    else if (Style::OptimizeSpeedShapeQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
    else if (Style::CrispEdgesShapeQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_DEFAULT);
    else if (Style::GeometricPrecisionShapeQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_DEFAULT);
}

static void
cairo_set_text_antialias (cairo_t * cr, Style::TextQuality aa)
{
    if (Style::AutoTextQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_DEFAULT);
    else if (Style::OptimizeSpeedTextQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
    else if (Style::OptimizeLegibilityTextQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_DEFAULT);
    else if (Style::GeometricPrecisionTextQuality == aa)
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_DEFAULT);
}


static void before_render_shape(RenderingContext *ctx, const Style &style,
                           const Transform &trans)
{
    cairo_t *cr = ctx->cr ;

    ctx->pushState(style) ;
    ctx->pushTransform(trans) ;

    Style &st = ctx->state.back() ;

    cairo_save(cr) ;
    cairo_push_transform(cr, ctx->transforms.back()) ;

    if ( ctx->renderingMode == RenderingContext::Display && !st.clip_path_id_.empty() )
    {
        ElementPtr pElem = ctx->lookupRef(st.clip_path_id_) ;
        if ( pElem && pElem->getType() == Element::ClipPathElement )
            cairo_apply_clip_path(ctx, dynamic_cast<ClipPath *>(pElem.get())) ;
    }
}


static void fill_stroke_shape(RenderingContext *ctx)
{
    if ( ctx->renderingMode == RenderingContext::Cliping ) return ;

    cairo_t *cr = ctx->cr ;

    Style &st = ctx->state.back() ;

    cairo_set_shape_antialias(cr, st.shape_rendering_quality_) ;

    double x1, y1, x2, y2 ;
    cairo_path_extents(cr, &x1, &y1, &x2, &y2) ;

    ctx->extentBoundingBox(x1, y1, x2, y2) ;

    cairo_set_line_width (cr, st.stroke_width_.toPixels(ctx, Length::HorzDir));
    cairo_set_miter_limit (cr, st.miter_limit_);
    cairo_set_line_cap (cr, (cairo_line_cap_t) st.line_cap_);
    cairo_set_line_join (cr, (cairo_line_join_t) st.line_join_);

    if ( !st.solid_stroke_ && st.stroke_paint_type_ != Style::NoPaint ) {
        int n = st.dash_array_.size() ;
        double *dashes = new double [n] ;
        for(int i=0 ; i<n ; i++ )
            dashes[i] = st.dash_array_[i].toPixels(ctx, Length::AbsoluteDir) ;
        double offset = st.dash_offset_.toPixels(ctx, Length::AbsoluteDir) ;

        cairo_set_dash(cr, dashes, n, offset) ;
        delete dashes ;
    }

    if ( st.fill_paint_type_ != Style::NoPaint )
    {

        if (st.fill_rule_ == Style::EvenOddFillRule)
            cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
        else if ( st.fill_rule_ == Style::NonZeroFillRule )
            cairo_set_fill_rule (cr, CAIRO_FILL_RULE_WINDING);

        if ( st.fill_paint_type_ == Style::SolidColorPaint )
        {
            double r, g, b;
            unsigned int rgb  = st.fill_paint_.clr_ ;

            r = ((rgb >> 16) & 0xff) / 255.0;
            g = ((rgb >> 8) & 0xff) / 255.0;
            b = ((rgb >> 0) & 0xff) / 255.0;

            if ( st.opacity_ * st.fill_opacity_ == 0xff * 0xff ) cairo_set_source_rgb(cr, r, g, b) ;
            else cairo_set_source_rgba(cr, r, g, b, st.opacity_ * st.fill_opacity_/255.0/255.0) ;
        }
        else if ( st.fill_paint_type_ == Style::PaintServerPaint )
        {
            ElementPtr elem = ctx->lookupRef(st.fill_paint_.paint_server_id_) ;
            if ( elem )
            {
                if ( elem->getType() == Element::LinearGradientElement )
                    cairo_apply_linear_gradient(ctx, dynamic_cast<LinearGradient *>(elem.get()), st.fill_opacity_/255.0) ;
                else if ( elem->getType() == Element::RadialGradientElement )
                    cairo_apply_radial_gradient(ctx, dynamic_cast<RadialGradient *>(elem.get()), st.fill_opacity_/255.0) ;
                else if ( elem->getType() == Element::PatternElement)
                    cairo_apply_pattern(ctx, dynamic_cast<Pattern *>(elem.get()), st.fill_opacity_/255.0) ;
            }
        }


        if ( st.stroke_paint_type_ != Style::NoPaint ) cairo_fill_preserve(cr) ;
        else cairo_fill (cr);
    }

    if ( st.stroke_paint_type_ != Style::NoPaint )
    {
        if ( st.stroke_paint_type_ == Style::SolidColorPaint )
        {
            double r, g, b;
            unsigned int rgb  = st.stroke_paint_.clr_ ;

            r = ((rgb >> 16) & 0xff) / 255.0;
            g = ((rgb >> 8) & 0xff) / 255.0;
            b = ((rgb >> 0) & 0xff) / 255.0;

            if ( st.opacity_ * st.stroke_opacity_ == 0xff * 0xff ) cairo_set_source_rgb(cr, r, g, b) ;
            else cairo_set_source_rgba(cr, r, g, b, st.opacity_ * st.stroke_opacity_/255.0/255.0) ;
        }
        else if ( st.stroke_paint_type_ == Style::PaintServerPaint )
        {
            ElementPtr elem = ctx->lookupRef(st.fill_paint_.paint_server_id_) ;
            if ( elem )
            {
                if ( elem->getType() == Element::LinearGradientElement )
                    cairo_apply_linear_gradient(ctx, dynamic_cast<LinearGradient *>(elem.get()), st.fill_opacity_) ;
                else if ( elem->getType() == Element::RadialGradientElement )
                    cairo_apply_radial_gradient(ctx, dynamic_cast<RadialGradient *>(elem.get()), st.fill_opacity_) ;
                else if ( elem->getType() == Element::PatternElement)
                    cairo_apply_pattern(ctx, dynamic_cast<Pattern *>(elem.get()), st.fill_opacity_) ;

            }
        }
        cairo_stroke(cr) ;
    }


}

static void after_render_shape(RenderingContext *ctx)
{
    cairo_restore(ctx->cr) ;

    ctx->popTransform() ;
    ctx->popState() ;

    ctx->obbox.x = ctx->obbox.y = ctx->obbox.w = ctx->obbox.h = 0.0 ;
}


void Path::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style_, trans_) ;

    cairo_new_path(cr) ;

    for(int i=0 ; i<data_.elements_.size() ; i++ )
    {
        PathData::Element &e = data_.elements_[i] ;

        switch( e.cmd )
        {
        case PathData::MoveToCmd:
            cairo_move_to(cr, e.args_[0], e.args_[1]) ;
            break ;
        case PathData::LineToCmd:
            cairo_line_to(cr, e.args_[0], e.args_[1]) ;
            break ;
        case PathData::CurveToCmd:
            cairo_curve_to(cr, e.args_[0], e.args_[1], e.args_[2], e.args_[3], e.args_[4], e.args_[5]) ;
            break ;
        case PathData::ClosePathCmd:;
            cairo_close_path(cr) ;

        }
    }

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;

}
#define SVG_ARC_MAGIC ((double) 0.5522847498)

static void cairo_elliptical_arc_to(cairo_t *cr, double x2, double y2)
{
    double x1, y1 ;
    double cx, cy, rx, ry ;

    cairo_get_current_point (cr, &x1, &y1);
    rx = x2 - x1 ;
    ry = y2 - y1 ;

    if ( rx > 0 && ry > 0 )
    {
        cx = x1 ;  	cy = y2 ;

        cairo_curve_to(cr,
                       cx + SVG_ARC_MAGIC * rx, cy - ry,
                       cx + rx, cy - SVG_ARC_MAGIC * ry,
                       x2, y2) ;
    }
    else if ( rx < 0 && ry > 0 )
    {
        rx = -rx ;	cx = x2 ;	cy = y1 ;

        cairo_curve_to(cr,
                       cx + rx, cy + SVG_ARC_MAGIC * ry,
                       cx + SVG_ARC_MAGIC * rx,  cy + ry,
                       x2, y2) ;
    }
    else if ( rx < 0 && ry < 0 )
    {
        rx = -rx ; ry = -ry ;	cx = x1 ;	cy = y2 ;

        cairo_curve_to(cr,
                       cx - rx*SVG_ARC_MAGIC, cy + ry,
                       cx - rx,  cy + SVG_ARC_MAGIC *ry,
                       x2, y2) ;
    }
    else {
        ry = -ry ;	cx = x2 ;	cy = y1 ;
        cairo_curve_to(cr,
                       cx - rx, cy - ry*SVG_ARC_MAGIC,
                       cx - rx*SVG_ARC_MAGIC,  cy - ry,
                       x2, y2) ;
    }

}


void Rect::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style_, trans_) ;

    double rxp = rx_.toPixels(ctx, Length::HorzDir) ;
    double ryp = ry_.toPixels(ctx, Length::VertDir) ;
    double xp = x_.toPixels(ctx, Length::HorzDir) ;
    double yp = y_.toPixels(ctx, Length::VertDir) ;
    double wp = width_.toPixels(ctx, Length::HorzDir) ;
    double hp = height_.toPixels(ctx, Length::VertDir) ;

    if (rxp > fabs (wp / 2.))
        rxp = fabs (wp / 2.);
    if (ryp > fabs (hp / 2.))
        ryp = fabs (hp / 2.);

    if (rxp == 0) rxp = ryp;
    else if (ryp == 0) ryp = rxp ;

    if ( wp != 0.0 && hp != 0.0 )
    {
        if ( rxp == 0.0 || ryp == 0.0 )
            cairo_rectangle(cr, xp, yp, wp, hp) ;
        else
        {
            cairo_move_to(cr, xp + rxp, yp) ;
            cairo_line_to(cr, xp + wp - rxp, yp) ;
            cairo_elliptical_arc_to(cr, xp + wp, yp + ryp) ;
            cairo_line_to(cr, xp + wp, yp + hp - ryp) ;
            cairo_elliptical_arc_to(cr, xp + wp -rxp, yp + hp) ;
            cairo_line_to(cr, xp + rxp, yp + hp) ;
            cairo_elliptical_arc_to(cr, xp, yp + hp - ryp) ;
            cairo_line_to(cr, xp, yp + ryp) ;
            cairo_elliptical_arc_to(cr, xp + rxp, yp) ;
        }
    }
    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}

void Ellipse::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style_, trans_) ;

    double rxp = rx_.toPixels(ctx, Length::HorzDir) ;
    double ryp = ry_.toPixels(ctx, Length::VertDir) ;
    double xp = cx_.toPixels(ctx, Length::HorzDir) ;
    double yp = cy_.toPixels(ctx, Length::VertDir) ;


    if ( rxp != 0.0 && ryp != 0.0 )
    {
        cairo_move_to(cr, xp, yp - ryp) ;
        cairo_elliptical_arc_to(cr, xp + rxp, yp) ;
        cairo_elliptical_arc_to(cr, xp, yp + ryp) ;
        cairo_elliptical_arc_to(cr, xp - rxp, yp) ;
        cairo_elliptical_arc_to(cr, xp , yp - ryp) ;
    }

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}

void Circle::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style_, trans_) ;

    double rp = r_.toPixels(ctx, Length::AbsoluteDir) ;
    double xp = cx_.toPixels(ctx, Length::HorzDir) ;
    double yp = cy_.toPixels(ctx, Length::VertDir) ;

    if ( rp != 0.0 )
        cairo_arc (cr, xp, yp, rp, 0.0, 2*M_PI) ;

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}

void Line::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style_, trans_) ;

    double x1p = x1_.toPixels(ctx, Length::HorzDir) ;
    double y1p = y1_.toPixels(ctx, Length::VertDir) ;
    double x2p = x2_.toPixels(ctx, Length::HorzDir) ;
    double y2p = y2_.toPixels(ctx, Length::VertDir) ;

    cairo_move_to(cr, x1p, y1p) ;
    cairo_line_to(cr, x2p, y2p) ;

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}


void PolyLine::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;

    before_render_shape(ctx, style_, trans_) ;

    vector<float> &pts = points_.points_ ;

    if ( pts.size() > 0 ) {

        float x1 = pts[0] ;
        float y1 = pts[1] ;

        cairo_move_to(cr, x1, y1) ;

        for( int i = 2 ; i<pts.size() ; i+=2 )
        {
            float x1 = pts[i] ;
            float y1 = pts[i+1] ;

            cairo_line_to(cr, x1, y1) ;
        }
    }

    fill_stroke_shape(ctx) ;

    after_render_shape(ctx) ;
}

void Polygon::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style_, trans_) ;

     vector<float> &pts = points_.points_ ;

    if ( pts.size() > 0 ) {

        float x1 = pts[0] ;
        float y1 = pts[1] ;

        cairo_move_to(cr, x1, y1) ;

        cairo_move_to(cr, x1, y1) ;

        for( int i = 2 ; i<pts.size() ; i+=2 )
        {
            float x1 = pts[i] ;
            float y1 = pts[i+1] ;

            cairo_line_to(cr, x1, y1) ;
        }

        cairo_close_path(cr) ;
    }

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}


static cairo_font_face_t *cairo_load_font(const string &font_family_Str,
                        Style::FontStyle style,
                        Style::FontVariant variant,
                        Style::FontWeight weight,
                        Style::FontStretch stretch,
                        double font_size_)
{
    FcInit();
    FcPattern *pattern = FcPatternCreate ();
    FcPatternAddString (pattern, FC_FAMILY, (const FcChar8 *)font_family_Str.c_str());

    switch ( style )
    {
        case Style::ObliqueFontStyle:
            FcPatternAddInteger (pattern, FC_SLANT, FC_SLANT_OBLIQUE);
            break ;
        case Style::ItalicFontStyle:
            FcPatternAddInteger (pattern, FC_SLANT, FC_SLANT_ITALIC);
            break ;
        case Style::NormalFontStyle:
            FcPatternAddInteger (pattern, FC_SLANT, FC_SLANT_ROMAN);
            break ;
    }

    switch ( weight )
    {
        case Style::NormalFontWeight:
        case Style::W100FontWeight:
            FcPatternAddInteger (pattern, FC_WEIGHT, FC_WEIGHT_NORMAL);
            break ;
        case Style::LighterFontWeight:
        case Style::W200FontWeight:
        case Style::W300FontWeight:
            FcPatternAddInteger (pattern, FC_WEIGHT, FC_WEIGHT_MEDIUM);
            break ;
        case Style::BoldFontWeight:
        case Style::W400FontWeight:
        case Style::W500FontWeight:
        case Style::W600FontWeight:
            FcPatternAddInteger (pattern, FC_WEIGHT, FC_WEIGHT_BOLD);
            break ;
        case Style::BolderFontWeight:
        case Style::W700FontWeight:
        case Style::W800FontWeight:
            FcPatternAddInteger (pattern, FC_WEIGHT, FC_WEIGHT_EXTRABOLD);
            break ;
        case Style::W900FontWeight:
            FcPatternAddInteger (pattern, FC_WEIGHT, FC_WEIGHT_BLACK);
            break ;
    }

    FcResult result;

    return  cairo_ft_font_face_create_for_pattern (pattern);

}

void SpanElement::render(RenderingContext *ctx)
{
    // not implemented
}

void Text::render(RenderingContext *ctx)
{
    ctx->pushState(style_) ;

    for(int i=0 ; i<children_.size() ; i++ )
    {
        children_[i]->render(ctx) ;
    }

    ctx->popState() ;
}

void Use::render(RenderingContext *ctx)
{
    ElementPtr pElem  = ctx->lookupRef(ref_id_) ;

    if ( !pElem ) return ;

    Transform tr = Transform::translation(x_.toPixels(ctx, Length::HorzDir),
                                          y_.toPixels(ctx, Length::VertDir)) ;

    before_render_shape(ctx, style_, Transform::multiply(trans_, tr)) ;

    Length width = width_, height = height_ ;

    if ( width.unknown() ) width = Length(Length::PercentageLengthType, 1.0) ;
    if ( height.unknown() ) height = Length(Length::PercentageLengthType, 1.0) ;

    if ( pElem->getType() == Element::SymbolElement )
    {
        Symbol *smb = dynamic_cast<Symbol *>(pElem.get()) ;

        smb->width_ = width ;
        smb->height_ = height ;

        smb->Document::render(ctx) ;
    }
    else
        pElem->render(ctx) ;

    after_render_shape(ctx) ;
}


void Group::render(RenderingContext *ctx)
{
    before_render_shape(ctx, style_, trans_) ;

    for( int i=0 ; i<children_.size() ; i++ )
    {
        children_[i]->render(ctx) ;
    }

    after_render_shape(ctx) ;
}

// We only support PNG for the moment
static cairo_surface_t *cairo_load_image(const std::string &path, std::string &docPath)
{
    // try PNG absolute path
    cairo_surface_t *psurf =
        cairo_image_surface_create_from_png(path.c_str()) ;

    cairo_status_t status = cairo_surface_status(psurf) ;

    if ( status == CAIRO_STATUS_SUCCESS ) return psurf ;

    // try PNG relative path

    boost::filesystem::path p(docPath) ;
    boost::filesystem::path q = p.parent_path() ;
    q /= path ;

    psurf = cairo_image_surface_create_from_png(q.string().c_str()) ;

    status = cairo_surface_status(psurf) ;

    if ( status == CAIRO_STATUS_SUCCESS ) return psurf ;

    return 0 ;
}

void Image::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;

    ctx->pushState(style_) ;
    ctx->pushTransform(trans_) ;

    cairo_save(cr) ;

    cairo_push_transform(cr, ctx->transforms.back()) ;

    Style &st = ctx->state.back() ;
    if ( ctx->renderingMode == RenderingContext::Display && !st.clip_path_id_.empty() )
    {
        ElementPtr pElem = ctx->lookupRef(st.clip_path_id_) ;
        if ( pElem && pElem->getType() == Element::ClipPathElement )
            cairo_apply_clip_path(ctx, dynamic_cast<ClipPath *>(pElem.get())) ;
    }

    float ix = ( x_.unknown() ) ? 0 : x_.toPixels(ctx, Length::HorzDir) ;
    float iy = ( y_.unknown() ) ? 0 : y_.toPixels(ctx, Length::VertDir) ;

    if ( width_.unknown() ) return ;
    if ( height_.unknown() ) return ;

    float iw  = width_.toPixels(ctx, Length::HorzDir) ;
    float ih = height_.toPixels(ctx, Length::VertDir) ;

    float opc = ctx->state.back().opacity_ ;

    if ( img_path_.empty() ) return ;

    cairo_surface_t *imsurf = cairo_load_image(img_path_, ctx->filePath) ;

    if ( !imsurf ) return ;

    double width = cairo_image_surface_get_width(imsurf) ;
    double height = cairo_image_surface_get_height(imsurf) ;

    Transform trs = preserve_aspect_ratio_.getViewBoxTransform(iw, ih, width, height, 0, 0 ) ;

    cairo_save(cr) ;

    cairo_translate(cr, ix, iy) ;
    cairo_push_transform(cr, trs) ;

    cairo_set_source_surface(cr, (cairo_surface_t *)imsurf, 0, 0);

    //set_object_bbox(ix, ih, iw, ih) ;

    cairo_paint_with_alpha (cr, opc);
    cairo_restore(cr) ;

    cairo_surface_destroy(imsurf) ;

    ctx->popState() ;
    ctx->popTransform() ;
}

void Pattern::render(RenderingContext *ctx)
{
}

void ClipPath::render(RenderingContext *ctx)
{
}

#define POINTS_PER_INCH (72.0)
#define CM_PER_INCH     (2.54)
#define MM_PER_INCH     (25.4)
#define PICA_PER_INCH   (6.0)

double Length::toPixels(RenderingContext *ctx, Length::Direction dir) const {

    double factor = 1.0 ;

    switch( unit_type_ )
    {
    case CMLengthType:
        factor = 1/2.54 ;
        break ;
    case MMLengthType:
        factor = 1/25.4 ;
        break ;
    case PTLengthType:
        factor = 1/72.0 ;
        break ;
    case PCLengthType:
        factor = 1/6.0 ;
        break ;
    }

    switch( unit_type_ )
    {
    case NumberLengthType:
    case PXLengthType:
        return value_in_specified_units_ ;
    case CMLengthType:
    case MMLengthType:
    case INLengthType:
    case PTLengthType:
    case PCLengthType:
        if ( dir == HorzDir  ) return  value_in_specified_units_ * factor*ctx->dpix ;
        else if ( dir == VertDir ) return  value_in_specified_units_ * factor*ctx->dpiy ;
        else if ( dir == AbsoluteDir ) return  value_in_specified_units_ * factor*sqrt(ctx->dpix * ctx->dpiy) ;
    case EMSLengthType:
    case EXSLengthType:
    {
        double font = 12.0 ;
        if ( unit_type_ == EMSLengthType ) return value_in_specified_units_ * font ;
        else return value_in_specified_units_ * font/2 ;
    }
    case PercentageLengthType:
    {
        float fx = (scale_to_viewport_) ? ctx->viewboxes.back().w : ctx->obbox.w ;
        float fy = (scale_to_viewport_) ? ctx->viewboxes.back().h : ctx->obbox.h ;

        if ( dir == HorzDir  ) return  value_in_specified_units_ * fx ;
        else if ( dir == VertDir ) return  value_in_specified_units_ * fy ;
        else if ( dir == AbsoluteDir ) return  value_in_specified_units_ * sqrt(fx*fy) ;
    }
    }

    return 0 ;
}


void GradientElement::resolveHRef(RenderingContext *ctx)
{
    if ( href_.empty() ) return ;

    ElementPtr pElem = ctx->lookupRef(href_) ;

    if ( pElem == 0 ) return ;

    if ( pElem->getType() != Element::LinearGradientElement &&
         pElem->getType() != Element::RadialGradientElement ) return ;

    GradientElement *grad = dynamic_cast<GradientElement *>(pElem.get()) ;

    grad->resolveHRef(ctx) ;

    if ( fallback_ && GradientElement::SpreadMethodDefined == 0 )
        spread_method_ = grad->spread_method_ ;

    if ( fallback_ && GradientElement::GradientUnitsDefined == 0 )
        gradient_units_ = grad->gradient_units_ ;

    if ( fallback_ && GradientElement::TransformDefined == 0 )
        trans_ = grad->trans_ ;

    if ( stops_.size() == 0 )
    {
        for(int i=0 ; i<grad->stops_.size() ; i++ )
          stops_.push_back(grad->stops_[i]) ;
    }

}

void DocumentInstance::render(cairo_t *cr, float w, float h, float dpi)
{
    assert(root_) ;

    RenderingContext ctx(cr) ;

    ctx.dpix = dpi ;
    ctx.dpiy = dpi ;
    ctx.docWidthHint = w ;
    ctx.docHeightHint = h ;

    ctx.setFilePath(file_name_) ;
    ctx.populateRefs(root_) ;

    root_->render(&ctx) ;

}

static void getDimensions(Document *doc, float dpi, float mw, float mh, float &sw, float &sh)
{
    assert(doc) ;

    RenderingContext ctx(0) ;

    ctx.dpix = dpi ;
    ctx.dpiy = dpi ;
    ctx.docWidthHint ;
    ctx.docHeightHint ;

    if ( doc->width_.unknown() ) sw = mw ;
    else sw = doc->width_.toPixels(&ctx, Length::HorzDir) ;

    if ( doc->height_.unknown() ) sh = mw ;
    else sh = doc->height_.toPixels(&ctx, Length::VertDir) ;
}

void DocumentInstance::renderToTarget(cairo_t *target, float x, float y, float sw, float sh, float dpi)
{
    assert(root_) ;

    float width, height ;

    getDimensions(root_.get(), dpi, sw, sh, width, height) ;

    // we render onto recording surface to isolate from target context

    cairo_rectangle_t r ;
    r.x = 0 ;
    r.y = 0 ;
    r.width = width ;
    r.height = height ;

    cairo_surface_t *rsurf = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &r) ;
    cairo_t *cr = cairo_create(rsurf) ;
    render(cr, sw, sh, dpi) ;
    cairo_destroy(cr) ;

  //  cairo_surface_write_to_png(rsurf, "/tmp/cairo.png") ;

    // paste on the target context

    cairo_save(target) ;

    cairo_translate(target, x, y) ;
    cairo_scale(target, (sw+1)/width, (sh+1)/height) ; // scaling introduces artifacts
    cairo_set_source_surface (target, rsurf, 0, 0);
    cairo_paint (target);
    cairo_restore(target) ;

    cairo_surface_destroy ((cairo_surface_t *)rsurf);
}



}

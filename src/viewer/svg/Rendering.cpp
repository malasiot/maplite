#include "Rendering.h"
#include "Document.h"
#include "Util.h"

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

    void populateRefs(Element *root)  ;
    Element *lookupRef(const std::string &name) ;

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
    std::map<std::string, Element *> refs ;
    Transform view2dev ;
    double ctx, cty ;
    string filePath ;
    float docWidthHint, docHeightHint ;
} ;



////////////////////////////////////////////////////////////////////////////////////

Element *RenderingContext::lookupRef(const std::string &name)
{
    map<string, Element *>::const_iterator it = refs.find(name) ;

    if ( it == refs.end() )	return 0 ;
    else return (*it).second ;
}

void RenderingContext::populateRefs(Element *root)
{
    if ( root->getType() == Element::DocumentElement )
    {
        Document *pElem = (Document *)root ;

        for( int i=0 ; i<pElem->children.size() ; i++ )
        {
            Element *el = pElem->children[i] ;

            string id = el->id ;

            if ( !id.empty() ) refs['#' + id] = el ;

            if ( el->getType() == Element::DocumentElement ||
                 el->getType() == Element::GroupElement ||
                 el->getType() == Element::DefsElement ) populateRefs(el) ;
        }
    }
    else if ( root->getType() == Element::GroupElement )
    {
        Group *pElem = (Group *)root ;

        for( int i=0 ; i<pElem->children.size() ; i++ )
        {
            Element *el = pElem->children[i] ;

            string id = el->id ;

            if ( !id.empty() ) refs['#' + id] = el ;

            if ( el->getType() == Element::DocumentElement ||
                 el->getType() == Element::GroupElement ||
                 el->getType() == Element::DefsElement ) populateRefs(el) ;
        }
    }
    else if ( root->getType() == Element::DefsElement )
    {
        Defs *pElem = (Defs *)root ;

        for( int i=0 ; i<pElem->children.size() ; i++ )
        {
            Element *el = pElem->children[i] ;

            string id = el->id ;

            if ( !id.empty() ) refs['#' + id] = el ;

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

    for( int i=0 ; i<st.flags.size() ; i++ )
    {
        Style::Flag flag = st.flags[i] ;

        if ( flag == Style::FillState )
        {
            style.fillPaintType = st.fillPaintType ;
            if ( style.fillPaintType == Style::SolidColorPaint )
              style.fillPaint.clr = st.fillPaint.clr ;
            else if ( style.fillPaintType == Style::PaintServerPaint )
                style.fillPaint.paintServerId = strdup(st.fillPaint.paintServerId) ;

        }
        else if ( flag == Style::FillOpacityState )
        {
            style.fillOpacity = st.fillOpacity ;
        }
        else if ( flag == Style::FillRuleState )
        {
            style.fillRule = st.fillRule ;
        }
        else if ( flag == Style::StrokeState )
        {
            style.strokePaintType = st.strokePaintType ;
            if ( style.strokePaintType == Style::SolidColorPaint )
              style.strokePaint.clr = st.strokePaint.clr ;
            else if ( style.strokePaintType == Style::PaintServerPaint )
                style.strokePaint.paintServerId = strdup(st.strokePaint.paintServerId) ;

        }
        else if ( flag == Style::StrokeWidthState )
        {
            style.strokeWidth = st.strokeWidth ;
        }
        else if ( flag == Style::StrokeDashArrayState )
        {
            style.dashArray = st.dashArray ;
            style.solidStroke = st.solidStroke ;
        }
        else if ( flag == Style::StrokeOpacityState )
        {
            style.strokeOpacity = st.strokeOpacity ;
        }
        else if ( flag == Style::OpacityState )
        {
            style.opacity = st.opacity ;
        }
        else if ( flag == Style::StrokeDashOffsetState )
        {
            style.dashOffset = st.dashOffset ;
        }
        else if ( flag == Style::StrokeLineCapState )
        {
            style.lineCap = st.lineCap ;
        }
        else if ( flag == Style::StrokeLineJoinState )
        {
            style.lineJoin = st.lineJoin ;
        }
        else if ( flag == Style::DisplayState )
        {
            style.displayMode = st.displayMode ;
        }
        else if ( flag == Style::VisibilityState )
        {
            style.visibilityMode = st.visibilityMode ;
        }
        else if ( flag == Style::FontFamilyState )
        {
            style.fontFamily = st.fontFamily ;
        }
        else if ( flag == Style::FontSizeState )
        {
            style.fontSize = st.fontSize ;
        }
        else if ( flag == Style::FontStyleState )
        {
            style.fontStyle = st.fontStyle ;
        }
        else if ( flag == Style::FontWeightState )
        {
            style.fontWeight = st.fontWeight ;
        }
        else if ( flag == Style::TextDecorationState )
        {
            style.textDecoration = st.textDecoration ;
        }
        else if ( flag == Style::TextAnchorState )
        {
            style.textAnchor = st.textAnchor ;
        }
        else if ( flag == Style::TextRenderingState )
        {
            style.textRenderingQuality = st.textRenderingQuality ;
        }
        else if ( flag == Style::ShapeRenderingState )
        {
            style.shapeRenderingQuality = st.shapeRenderingQuality ;
        }
        else if ( flag == Style::ClipPathState )
        {
            style.clipPathId = st.clipPathId ;
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

    cairo_matrix_init (&matrix,
                       tr.m[0], tr.m[1],
            tr.m[2], tr.m[3],
            tr.m[4], tr.m[5]) ;

    cairo_transform (cr, &matrix);
}


void Document::render(RenderingContext *ctx)
{
    ctx->pushState(style) ;

    double xx, yy, sw, sh ;

    if ( x.unknown() ) xx = 0 ;
    else xx = x.toPixels(ctx, Length::HorzDir) ;

    if ( y.unknown() ) yy = 0 ;
    else yy = y.toPixels(ctx, Length::HorzDir) ;

    if ( width.unknown() )
    {
        if ( parent )
            sw = Length(Length::PercentageLengthType, 1.0, true).toPixels(ctx, Length::HorzDir) ;
        else
            sw = ctx->docWidthHint ;
    }
    else
        sw = width.toPixels(ctx, Length::HorzDir) ;

    if ( height.unknown() )
    {
        if ( parent )
            sh = Length(Length::PercentageLengthType, 1.0, true).toPixels(ctx, Length::HorzDir) ;
        else
            sh = ctx->docHeightHint ;
    }
    else
        sh = height.toPixels(ctx, Length::HorzDir) ;

    ViewBox vbox = viewBox ;

    if ( vbox.w == 0 ) vbox.w = sw ;
    if ( vbox.h == 0 ) vbox.h = sh ;

    ctx->viewboxes.push_back(vbox) ;

    Transform trs = preserveAspectRatio.getViewBoxTransform(sw, sh, vbox.w, vbox.h, vbox.x, vbox.y) ;

    ctx->view2dev = trs ;

    cairo_t *cr = ctx->cr ;

    cairo_save(cr) ;

    cairo_push_transform(cr, trs) ;

    bool hasOverflow = style.hasFlag(Style::OverflowState) ;

    if ( ( hasOverflow && style.overflow == false ) || ( !hasOverflow && parent == NULL ))
    {
    //	cairo_rectangle(ctx->cr, px, py, sw, sh) ;
//		cairo_clip(ctx->cr) ;
    }

    for( int i=0 ; i<children.size() ; i++ )
    {
        children[i]->render(ctx) ;
    }

    cairo_restore(ctx->cr) ;

    ctx->popState() ;

    ctx->viewboxes.pop_back();
}


static void cairo_apply_clip_path(RenderingContext *ctx, ClipPath *cp)
{
    ctx->pushState(cp->style) ;
    ctx->pushTransform(cp->trans) ;

    ctx->renderingMode = RenderingContext::Cliping ;
    for( int i=0 ; i<cp->children.size() ; i++ )
    {
        cp->children[i]->render(ctx) ;
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

    double x1, y1, x2, y2 ;

    Length x1_ = lg->x1, y1_ = lg->y1, x2_ = lg->x2, y2_ = lg->y2 ;

    if ( x1_.unknown() ) x1_ = Length(Length::PercentageLengthType, 0) ;
    if ( y1_.unknown() ) y1_ = Length(Length::PercentageLengthType, 0) ;
    if ( x2_.unknown() ) x2_ = Length(Length::PercentageLengthType, 1.0) ;
    if ( y2_.unknown() ) y2_ = Length(Length::PercentageLengthType, 0.0) ;

    LinearGradient::GradientUnits gu = lg->gradientUnits ;
    LinearGradient::SpreadMethod sm  = lg->spreadMethod ;

    x1 = ( gu == GradientElement::ObjectBoundingBox ) ?
                x1_.valueInSpecifiedUnits : x1_.toPixels(ctx, Length::HorzDir) ;

    y1 = ( gu == GradientElement::ObjectBoundingBox ) ?
                y1_.valueInSpecifiedUnits : y1_.toPixels(ctx, Length::VertDir) ;

    y2 = ( gu == GradientElement::ObjectBoundingBox ) ?
                y2_.valueInSpecifiedUnits : y2_.toPixels(ctx, Length::VertDir) ;

    x2 = ( gu == GradientElement::ObjectBoundingBox ) ?
                x2_.valueInSpecifiedUnits : x2_.toPixels(ctx, Length::HorzDir) ;

    pattern = cairo_pattern_create_linear ( x1, y1, x2, y2 ) ;

    cairo_matrix_init (&matrix,
                        lg->trans.m[0], lg->trans.m[1],
                        lg->trans.m[2], lg->trans.m[3], lg->trans.m[4], lg->trans.m[5]);

    if (lg->gradientUnits == GradientElement::ObjectBoundingBox ) {
        cairo_matrix_t bboxmatrix;
        cairo_matrix_init (&bboxmatrix, ctx->obbox.w, 0, 0, ctx->obbox.h, ctx->obbox.x, ctx->obbox.y);
        cairo_matrix_multiply (&matrix, &matrix, &bboxmatrix);
    }

    cairo_matrix_invert (&matrix);
    cairo_pattern_set_matrix (pattern, &matrix);

    if (lg->spreadMethod == LinearGradient::ReflectSpreadMethod )
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REFLECT);
    else if (lg->spreadMethod == LinearGradient::RepeatSpreadMethod)
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    else
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_PAD);

    unsigned int gopac = ctx->state.back().opacity ;

    for ( int i = 0; i < lg->stops.size() ; i++ )
    {
        Stop &stop = lg->stops[i] ;

        unsigned int rgb = stop.stopColor ;
        unsigned int opacity = stop.stopOpacity ;

        cairo_pattern_add_color_stop_rgba (pattern, stop.offset.valueInSpecifiedUnits,
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

    double fx, fy, cx, cy, r ;

    Length cx_ = rg->cx, cy_ = rg->cy, fx_ = rg->fx, fy_ = rg->fy, r_ = rg->r ;

    if ( cx_.unknown() ) cx_ = Length(Length::PercentageLengthType, 0.5) ;
    if ( cy_.unknown() ) cy_ = Length(Length::PercentageLengthType, 0.5) ;
    if ( fx_.unknown() ) fx_ = cx_ ;
    if ( fy_.unknown() ) fy_ = cy_ ;
    if ( r_.unknown() ) r_ = Length(Length::PercentageLengthType, 0.5) ;

    RadialGradient::GradientUnits gu = rg->gradientUnits ;
    RadialGradient::SpreadMethod sm  = rg->spreadMethod ;

    cx = ( gu == GradientElement::ObjectBoundingBox ) ?
                cx_.valueInSpecifiedUnits : cx_.toPixels(ctx, Length::HorzDir) ;

    cy = ( gu == GradientElement::ObjectBoundingBox ) ?
                cy_.valueInSpecifiedUnits : cy_.toPixels(ctx, Length::VertDir) ;

    fx = ( gu == GradientElement::ObjectBoundingBox ) ?
                fx_.valueInSpecifiedUnits : fx_.toPixels(ctx, Length::HorzDir) ;

    fy = ( gu == GradientElement::ObjectBoundingBox ) ?
                fy_.valueInSpecifiedUnits : fy_.toPixels(ctx, Length::HorzDir) ;

    r =  ( gu == GradientElement::ObjectBoundingBox ) ?
                r_.valueInSpecifiedUnits : r_.toPixels(ctx, Length::HorzDir) ;

    pattern = cairo_pattern_create_radial ( fx,	fy, 0.0, cx, cy, r) ;

    cairo_matrix_init (&matrix,
                        rg->trans.m[0], rg->trans.m[1],
                        rg->trans.m[2], rg->trans.m[3], rg->trans.m[4], rg->trans.m[5]);

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

    unsigned int gopac = ctx->state.back().opacity ;

    for ( int i = 0; i < rg->stops.size() ; i++ )
    {
        Stop &stop = rg->stops[i] ;

        unsigned int rgb = stop.stopColor ;
        unsigned int opacity = stop.stopOpacity ;

        cairo_pattern_add_color_stop_rgba (pattern, stop.offset.valueInSpecifiedUnits,
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

    double pw, ph, px, py ;

    Length px_ = pat->x, py_ = pat->y, pw_ = pat->width, ph_ = pat->height ;

    if ( px_.unknown() ) px_ = Length(Length::NumberLengthType, 0.0) ;
    if ( py_.unknown() ) py_ = Length(Length::NumberLengthType, 0.0) ;
    if ( pw_.unknown() ) pw_ = Length(Length::NumberLengthType, 0.0) ;
    if ( ph_.unknown() ) ph_ = Length(Length::NumberLengthType, 0.0) ;

    pw = pw_.toPixels(ctx, Length::HorzDir) ;
    ph = ph_.toPixels(ctx, Length::VertDir) ;
    px = px_.toPixels(ctx, Length::HorzDir) ;
    py = py_.toPixels(ctx, Length::VertDir) ;

    double  bbwscale, bbhscale, scwscale, schscale;

    int pbw, pbh;
    cairo_matrix_t affine, caffine, taffine ;

    if (pat->patternUnits == Pattern::ObjectBoundingBox)
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
    cairo_matrix_init(&patm, pat->trans.m[0], pat->trans.m[1], pat->trans.m[2], pat->trans.m[3],
            pat->trans.m[4], pat->trans.m[5]) ;
    //cairo_get_matrix(ctx->cr, &trsm) ;

    cairo_matrix_multiply(&taffine, &patm, &caffine) ;

    scwscale = sqrt (taffine.xx * taffine.xx + taffine.xy * taffine.xy);
    schscale = sqrt (taffine.yx * taffine.yx + taffine.yy * taffine.yy);

    pbw = pw * bbwscale * scwscale;
    pbh = ph * bbhscale * schscale;

    scwscale = (double) pbw / (double) (pw * bbwscale);
    schscale = (double) pbh / (double) (ph * bbhscale);

    cairo_surface_t *surface = cairo_surface_create_similar (cairo_get_target (cr),
                                                             CAIRO_CONTENT_COLOR_ALPHA, pbw, pbh);
    cr_pattern = cairo_create (surface);

    if (pat->patternUnits == Pattern::ObjectBoundingBox) {
        /* subtract the pattern origin */
        affine.x0 = ctx->obbox.x + px * ctx->obbox.w;
        affine.y0 = ctx->obbox.y + py * ctx->obbox.h;
    } else {
        /* subtract the pattern origin */
        affine.x0 = px;
        affine.y0 = py;
    }

    cairo_matrix_multiply(&affine, &affine, &patm) ;

    double sw = pw * bbwscale;
    double sh = ph * bbhscale;

    Transform prs ;

    if ( pat->viewBox.w == 0 ) pat->viewBox.w = sw ;
    if ( pat->viewBox.h == 0 ) pat->viewBox.h = sh ;

    if ( pat->viewBox.w != 0.0 && pat->viewBox.h != 0.0 )
    {
        ViewBox vbox = pat->viewBox ;

        double ofx = 0, ofy = 0 ;
        double aspScaleX = 1.0 ;
        double aspScaleY = 1.0 ;

        if ( pat->preserveAspectRatio.viewBoxAlign != PreserveAspectRatio::NoViewBoxAlign )
        {
            pat->preserveAspectRatio.constrainViewBox(sw, sh, vbox) ;

            aspScaleX = vbox.w/pat->viewBox.w ;
            aspScaleY = vbox.h/pat->viewBox.h ;

            ofx = vbox.x;
            ofy = vbox.y ;
        }
        else {
            aspScaleX = sw/vbox.w ;
            aspScaleY = sh/vbox.h ;

        }

        prs = Transform::translation(-px, -py) ;
        prs = Transform::multiply(prs, Transform::scaling(aspScaleX, aspScaleY)) ;
        prs = Transform::multiply(prs, Transform::translation(ofx , ofy)) ;

    }

    RenderingContext pctx(cr_pattern) ;

    pctx.refs = ctx->refs ;

    pctx.pushTransform(prs) ;

    cairo_matrix_init(&affine, prs.m[0], prs.m[1], prs.m[2], prs.m[3], prs.m[4], prs.m[5]) ;
    //cairo_matrix_multiply(&affine, &affine, &patm) ;

    cairo_transform(cr_pattern, &affine) ;

    for( int i=0 ; i<pat->children.size() ; i++ )
        pat->children[i]->render(&pctx) ;

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

    if ( ctx->renderingMode == RenderingContext::Display && !st.clipPathId.empty() )
    {
        Element *pElem = ctx->lookupRef(st.clipPathId) ;
        if ( pElem && pElem->getType() == Element::ClipPathElement )
            cairo_apply_clip_path(ctx, (ClipPath *)pElem) ;
    }
}


static void fill_stroke_shape(RenderingContext *ctx)
{
    if ( ctx->renderingMode == RenderingContext::Cliping ) return ;

    cairo_t *cr = ctx->cr ;

    Style &st = ctx->state.back() ;

    cairo_set_shape_antialias(cr, st.shapeRenderingQuality) ;

    double x1, y1, x2, y2 ;
    cairo_path_extents(cr, &x1, &y1, &x2, &y2) ;

    ctx->extentBoundingBox(x1, y1, x2, y2) ;

    cairo_set_line_width (cr, st.strokeWidth.toPixels(ctx, Length::HorzDir));
    cairo_set_miter_limit (cr, st.miterLimit);
    cairo_set_line_cap (cr, (cairo_line_cap_t) st.lineCap);
    cairo_set_line_join (cr, (cairo_line_join_t) st.lineJoin);

    if ( !st.solidStroke && st.strokePaintType != Style::NoPaint ) {
        int n = st.dashArray.size() ;
        double *dashes = new double [n] ;
        for(int i=0 ; i<n ; i++ )
            dashes[i] = st.dashArray[i].toPixels(ctx, Length::AbsoluteDir) ;
        double offset = st.dashOffset.toPixels(ctx, Length::AbsoluteDir) ;

        cairo_set_dash(cr, dashes, n, offset) ;
        delete dashes ;
    }

    if ( st.fillPaintType != Style::NoPaint )
    {

        if (st.fillRule == Style::EvenOddFillRule)
            cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
        else if ( st.fillRule == Style::NonZeroFillRule )
            cairo_set_fill_rule (cr, CAIRO_FILL_RULE_WINDING);

        if ( st.fillPaintType == Style::SolidColorPaint )
        {
            double r, g, b;
            unsigned int rgb  = st.fillPaint.clr ;

            r = ((rgb >> 16) & 0xff) / 255.0;
            g = ((rgb >> 8) & 0xff) / 255.0;
            b = ((rgb >> 0) & 0xff) / 255.0;

            if ( st.opacity * st.fillOpacity == 0xff * 0xff ) cairo_set_source_rgb(cr, r, g, b) ;
            else cairo_set_source_rgba(cr, r, g, b, st.opacity * st.fillOpacity/255.0/255.0) ;
        }
        else if ( st.fillPaintType == Style::PaintServerPaint )
        {
            Element *elem = ctx->lookupRef(st.fillPaint.paintServerId) ;
            if ( elem )
            {
                if ( elem->getType() == Element::LinearGradientElement )
                    cairo_apply_linear_gradient(ctx, (LinearGradient *)elem, st.fillOpacity/255.0) ;
                else if ( elem->getType() == Element::RadialGradientElement )
                    cairo_apply_radial_gradient(ctx, (RadialGradient *)elem, st.fillOpacity/255.0) ;
                else if ( elem->getType() == Element::PatternElement)
                    cairo_apply_pattern(ctx, (Pattern *)elem, st.fillOpacity/255.0) ;
            }
        }


        if ( st.strokePaintType != Style::NoPaint ) cairo_fill_preserve(cr) ;
        else cairo_fill (cr);
    }

    if ( st.strokePaintType != Style::NoPaint )
    {
        if ( st.strokePaintType == Style::SolidColorPaint )
        {
            double r, g, b;
            unsigned int rgb  = st.strokePaint.clr ;

            r = ((rgb >> 16) & 0xff) / 255.0;
            g = ((rgb >> 8) & 0xff) / 255.0;
            b = ((rgb >> 0) & 0xff) / 255.0;

            if ( st.opacity * st.strokeOpacity == 0xff * 0xff ) cairo_set_source_rgb(cr, r, g, b) ;
            else cairo_set_source_rgba(cr, r, g, b, st.opacity * st.strokeOpacity/255.0/255.0) ;
        }
        else if ( st.strokePaintType == Style::PaintServerPaint )
        {
            Element *elem = ctx->lookupRef(st.fillPaint.paintServerId) ;
            if ( elem )
            {
                if ( elem->getType() == Element::LinearGradientElement )
                    cairo_apply_linear_gradient(ctx, (LinearGradient *)elem, st.fillOpacity) ;
                else if ( elem->getType() == Element::RadialGradientElement )
                    cairo_apply_radial_gradient(ctx, (RadialGradient *)elem, st.fillOpacity) ;
                else if ( elem->getType() == Element::PatternElement)
                    cairo_apply_pattern(ctx, (Pattern *)elem, st.fillOpacity) ;

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
    before_render_shape(ctx, style, trans) ;

    cairo_new_path(cr) ;

    for(int i=0 ; i<data.elements.size() ; i++ )
    {
        PathData::Element &e = data.elements[i] ;

        switch( e.cmd )
        {
        case PathData::MoveToCmd:
            cairo_move_to(cr, e.args[0], e.args[1]) ;
            break ;
        case PathData::LineToCmd:
            cairo_line_to(cr, e.args[0], e.args[1]) ;
            break ;
        case PathData::CurveToCmd:
            cairo_curve_to(cr, e.args[0], e.args[1], e.args[2], e.args[3], e.args[4], e.args[5]) ;
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
    before_render_shape(ctx, style, trans) ;

    double rxp = rx.toPixels(ctx, Length::HorzDir) ;
    double ryp = ry.toPixels(ctx, Length::VertDir) ;
    double xp = x.toPixels(ctx, Length::HorzDir) ;
    double yp = y.toPixels(ctx, Length::VertDir) ;
    double wp = width.toPixels(ctx, Length::HorzDir) ;
    double hp = height.toPixels(ctx, Length::VertDir) ;

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
    before_render_shape(ctx, style, trans) ;

    double rxp = rx.toPixels(ctx, Length::HorzDir) ;
    double ryp = ry.toPixels(ctx, Length::VertDir) ;
    double xp = cx.toPixels(ctx, Length::HorzDir) ;
    double yp = cy.toPixels(ctx, Length::VertDir) ;


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
    before_render_shape(ctx, style, trans) ;

    double rp = r.toPixels(ctx, Length::AbsoluteDir) ;
    double xp = cx.toPixels(ctx, Length::HorzDir) ;
    double yp = cy.toPixels(ctx, Length::VertDir) ;

    if ( rp != 0.0 )
        cairo_arc (cr, xp, yp, rp, 0.0, 2*M_PI) ;

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}

void Line::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;
    before_render_shape(ctx, style, trans) ;

    double x1p = x1.toPixels(ctx, Length::HorzDir) ;
    double y1p = y1.toPixels(ctx, Length::VertDir) ;
    double x2p = x2.toPixels(ctx, Length::HorzDir) ;
    double y2p = y2.toPixels(ctx, Length::VertDir) ;

    cairo_move_to(cr, x1p, y1p) ;
    cairo_line_to(cr, x2p, y2p) ;

    fill_stroke_shape(ctx) ;
    after_render_shape(ctx) ;
}


void PolyLine::render(RenderingContext *ctx)
{
    cairo_t *cr = ctx->cr ;

    before_render_shape(ctx, style, trans) ;

    vector<float> &pts = points.points ;

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
    before_render_shape(ctx, style, trans) ;

     vector<float> &pts = points.points ;

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


static cairo_font_face_t *cairo_load_font(const string &fontFamilyStr,
                        Style::FontStyle style,
                        Style::FontVariant variant,
                        Style::FontWeight weight,
                        Style::FontStretch stretch,
                        double fontSize)
{
    FcInit();
    FcPattern *pattern = FcPatternCreate ();
    FcPatternAddString (pattern, FC_FAMILY, (const FcChar8 *)fontFamilyStr.c_str());

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
    if ( text.empty() ) return ;

    cairo_t *cr = ctx->cr ;

    Text *parent_ = (Text *)parent ;

    float xp, yp, dxp = 0, dyp = 0;

    if ( x.unknown() ) xp = ctx->ctx ;
    else xp = x.toPixels(ctx, Length::HorzDir) ;

    if ( y.unknown() ) yp = ctx->cty ;
    else yp =  y.toPixels(ctx, Length::VertDir) ;

    if ( !dx.unknown() )
        dxp = dx.toPixels(ctx, Length::HorzDir) ;

    if ( !dy.unknown() )
        dyp = dy.toPixels(ctx, Length::VertDir) ;

    xp += dxp ;
    yp += dyp ;

    Transform &trv2d = ctx->view2dev ;

    double xpt = xp, ypt = yp;

    Style &st = ctx->state.back() ;

    cairo_font_face_t *fontFace =
        cairo_load_font(st.fontFamily, st.fontStyle, st.fontVariant, st.fontWeight, st.fontStretch, st.fontSize.toPixels(ctx, Length::HorzDir)) ;

    cairo_set_font_face(cr, fontFace) ;
    cairo_set_font_size(cr, st.fontSize.toPixels(ctx,Length::HorzDir)) ;
    cairo_set_text_antialias(cr, st.textRenderingQuality) ;
    cairo_set_line_width (cr, st.strokeWidth.toPixels(ctx, Length::HorzDir));

    cairo_text_extents_t extents ;

    cairo_text_extents(cr, text.c_str(), &extents) ;

    if ( st.textAnchor == Style::MiddleTextAnchor )
        xpt -= extents.width/2 ;
    else if ( st.textAnchor == Style::EndTextAnchor )
        xpt -= extents.width ;

    ctx->ctx += extents.width + dxp ;

    Transform prs ;

    prs = Transform::translation(-xpt, -ypt) ;
    prs = Transform::multiply(prs, parent_->trans) ;
    prs = Transform::multiply(prs, Transform::translation(xpt , ypt)) ;

    before_render_shape(ctx, style, prs) ;

    cairo_move_to(cr, xpt, ypt) ;

    cairo_text_path(cr, text.c_str() ) ;

    fill_stroke_shape(ctx) ;

    after_render_shape(ctx) ;

    cairo_font_face_destroy (fontFace);

}

void Text::render(RenderingContext *ctx)
{
    ctx->pushState(style) ;

    for(int i=0 ; i<children.size() ; i++ )
    {
        children[i]->render(ctx) ;
    }

    ctx->popState() ;
}

void Use::render(RenderingContext *ctx)
{
    Element *pElem  = ctx->lookupRef(refId) ;

    if ( !pElem ) return ;

    Transform tr = Transform::translation(x.toPixels(ctx, Length::HorzDir),
                                          y.toPixels(ctx, Length::VertDir)) ;

    before_render_shape(ctx, style, Transform::multiply(trans, tr)) ;

    Length width_ = width, height_ = height ;

    if ( width_.unknown() ) width_ = Length(Length::PercentageLengthType, 1.0) ;
    if ( height_.unknown() ) height_ = Length(Length::PercentageLengthType, 1.0) ;

    if ( pElem->getType() == Element::SymbolElement )
    {
        Symbol *smb = (Symbol *)pElem ;

        smb->width = width ;
        smb->height = height ;

        smb->Document::render(ctx) ;
    }
    else
        pElem->render(ctx) ;

    after_render_shape(ctx) ;
}


void Group::render(RenderingContext *ctx)
{
    before_render_shape(ctx, style, trans) ;

    for( int i=0 ; i<children.size() ; i++ )
    {
        children[i]->render(ctx) ;
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

    ctx->pushState(style) ;
    ctx->pushTransform(trans) ;

    cairo_save(cr) ;

    cairo_push_transform(cr, ctx->transforms.back()) ;

    Style &st = ctx->state.back() ;
    if ( ctx->renderingMode == RenderingContext::Display && !st.clipPathId.empty() )
    {
        Element *pElem = ctx->lookupRef(st.clipPathId) ;
        if ( pElem && pElem->getType() == Element::ClipPathElement )
            cairo_apply_clip_path(ctx, (ClipPath *)pElem) ;
    }

    float ix = ( x.unknown() ) ? 0 : x.toPixels(ctx, Length::HorzDir) ;
    float iy = ( y.unknown() ) ? 0 : y.toPixels(ctx, Length::VertDir) ;

    if ( width.unknown() ) return ;
    if ( height.unknown() ) return ;

    float iw  = width.toPixels(ctx, Length::HorzDir) ;
    float ih = height.toPixels(ctx, Length::VertDir) ;

    float opc = ctx->state.back().opacity ;

    if ( img_path.empty() ) return ;

    cairo_surface_t *imsurf = cairo_load_image(img_path, ctx->filePath) ;

    if ( !imsurf ) return ;

    double width = cairo_image_surface_get_width(imsurf) ;
    double height = cairo_image_surface_get_height(imsurf) ;

    Transform trs = preserveAspectRatio.getViewBoxTransform(iw, ih, width, height, 0, 0 ) ;

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

    switch( unitType )
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

    switch( unitType )
    {
    case NumberLengthType:
    case PXLengthType:
        return valueInSpecifiedUnits ;
    case CMLengthType:
    case MMLengthType:
    case INLengthType:
    case PTLengthType:
    case PCLengthType:
        if ( dir == HorzDir  ) return  valueInSpecifiedUnits * factor*ctx->dpix ;
        else if ( dir == VertDir ) return  valueInSpecifiedUnits * factor*ctx->dpiy ;
        else if ( dir == AbsoluteDir ) return  valueInSpecifiedUnits * factor*sqrt(ctx->dpix * ctx->dpiy) ;
    case EMSLengthType:
    case EXSLengthType:
    {
        double font = 12.0 ;
        if ( unitType == EMSLengthType ) return valueInSpecifiedUnits * font ;
        else return valueInSpecifiedUnits * font/2 ;
    }
    case PercentageLengthType:
    {
        float fx = (scaleToViewport) ? ctx->viewboxes.back().w : ctx->obbox.w ;
        float fy = (scaleToViewport) ? ctx->viewboxes.back().h : ctx->obbox.h ;

        if ( dir == HorzDir  ) return  valueInSpecifiedUnits * fx ;
        else if ( dir == VertDir ) return  valueInSpecifiedUnits * fy ;
        else if ( dir == AbsoluteDir ) return  valueInSpecifiedUnits * sqrt(fx*fy) ;
    }
    }

    return 0 ;
}


void GradientElement::resolveHRef(RenderingContext *ctx)
{
    if ( href.empty() ) return ;

    Element *pElem = ctx->lookupRef(href) ;

    if ( pElem == 0 ) return ;

    if ( pElem->getType() != Element::LinearGradientElement &&
         pElem->getType() != Element::RadialGradientElement ) return ;

    GradientElement *grad = (GradientElement *)pElem ;

    grad->resolveHRef(ctx) ;

    if ( fallback && GradientElement::SpreadMethodDefined == 0 )
        spreadMethod = grad->spreadMethod ;

    if ( fallback && GradientElement::GradientUnitsDefined == 0 )
        gradientUnits = grad->gradientUnits ;

    if ( fallback && GradientElement::TransformDefined == 0 )
        trans = grad->trans ;

    if ( stops.size() == 0 )
    {
        for(int i=0 ; i<grad->stops.size() ; i++ )
          stops.push_back(grad->stops[i]) ;
    }

}

void DocumentInstance::render(cairo_t *cr, float w, float h, float dpi)
{
    assert(root) ;

    RenderingContext ctx(cr) ;

    ctx.dpix = dpi ;
    ctx.dpiy = dpi ;
    ctx.docWidthHint = w ;
    ctx.docHeightHint = h ;

    ctx.setFilePath(fileName) ;
    ctx.populateRefs(root) ;

    root->render(&ctx) ;

}

static void getDimensions(Document *doc, float dpi, float mw, float mh, float &sw, float &sh)
{
    assert(doc) ;

    RenderingContext ctx(0) ;

    ctx.dpix = dpi ;
    ctx.dpiy = dpi ;
    ctx.docWidthHint ;
    ctx.docHeightHint ;

    if ( doc->width.unknown() ) sw = mw ;
    else sw = doc->width.toPixels(&ctx, Length::HorzDir) ;

    if ( doc->height.unknown() ) sh = mw ;
    else sh = doc->height.toPixels(&ctx, Length::VertDir) ;
}

void DocumentInstance::renderToTarget(cairo_t *target, float x, float y, float size, float dpi)
{
    assert(root) ;

    float width, height ;

    getDimensions(root, dpi, size, size, width, height) ;

    // we render onto recording surface to isolate from target context

    cairo_rectangle_t r ;
    r.x = 0 ;
    r.y = 0 ;
    r.width = width ;
    r.height = height ;

    cairo_surface_t *rsurf = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &r) ;
    cairo_t *cr = cairo_create(rsurf) ;
    render(cr, size, size, dpi) ;
    cairo_destroy(cr) ;

    cairo_surface_write_to_png(rsurf, "/tmp/cairo.png") ;

    // paste on the target context

    cairo_save(target) ;

    cairo_translate(target, x, y) ;
    cairo_scale(target, size/height, size/height) ;
    cairo_set_source_surface (target, rsurf, 0, 0);
    cairo_paint_with_alpha (target, 1.0);
    cairo_restore(target) ;

    cairo_surface_destroy ((cairo_surface_t *)rsurf);
}



}

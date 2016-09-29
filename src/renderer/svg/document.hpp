#ifndef __SVG_DOCUMENT_HPP__
#define __SVG_DOCUMENT_HPP__

#include "util.hpp"

#include <cairo.h>
#include <string.h>

#include "pugixml.hpp"

namespace svg {

class RenderingContext ;

class Exception: public std::runtime_error
{
public:
    Exception(const std::string &w = std::string()): std::runtime_error(w) {}

};

class ViewBox
{
  public:

    ViewBox() {x = y = w = h = 0.0 ; }

    bool fromString(const std::string &str) ;

    float x, y, w, h ;
} ;


class PathData
{
  public:

    enum Command { MoveToCmd, ClosePathCmd, LineToCmd, CurveToCmd, SmoothCurveToCmd,
        QuadCurveToCmd, SmoothQuadCurveToCmd, EllipticArcToCmd } ;

    PathData() {}

    bool fromString(const std::string &str) ;

    struct Element {
        Element(Command cc, double arg1 = 0, double arg2 = 0, double arg3 = 0, double arg4 = 0,
            double arg5 = 0, double arg6 = 0) {
            cmd = cc ;
            args_[0] = arg1 ; args_[1] = arg2 ; args_[2] = arg3 ; args_[3] = arg4 ; args_[4] = arg5 ;
            args_[5] = arg6 ;
        }
        double args_[6] ;
      Command cmd ;
    } ;

    std::vector<Element> elements_ ;
} ;

class Length
{
  public:

    enum UnitType { UnknownLengthType, NumberLengthType, EMSLengthType, EXSLengthType, PXLengthType,
            CMLengthType, MMLengthType, INLengthType, PTLengthType, PCLengthType, PercentageLengthType } ;

    enum Direction { AbsoluteDir, HorzDir, VertDir, ViewportDir } ;

    Length(): unit_type_(UnknownLengthType), value_in_specified_units_(0) {}
    Length(UnitType unit, double val, bool scale = false): unit_type_(unit), value_in_specified_units_(val), scale_to_viewport_(scale) {}

    bool fromString(const std::string &str, bool scale = false)  ;

    static bool parseList(const std::string &str, std::vector<Length> &ls) ;
    double toPixels(RenderingContext *ctx, Direction dir = AbsoluteDir) const ;

    bool unknown() const { return unit_type_ == UnknownLengthType ; }

    UnitType unit_type_ ;
    float value_in_specified_units_ ;
    bool scale_to_viewport_ ;
};

class Stop
{
  public:

    Stop(): stop_color_(), stop_opacity_(255) {
        offset_.fromString("0.0") ;
    }

    bool fromXml(const pugi::xml_node &p) ;

    Length offset_ ;
    unsigned int stop_color_ ;
    int stop_opacity_ ;
} ;


class Style
{
  public:

    enum Flag { FontState, FontFamilyState, FontSizeState, FontSizeAdjustState,
        FontStretchState, FontStyleState, FontVariantState, FontWeightState,
        TextDirectionState, TextLetterSpacingState, TextDecorationState, TextUnicodeBidiState,
        TextWordSpacingState, ClipState, ColorState, CursorState, DisplayState, OverflowState,
        VisibilityState, ClipPathState, ClipRuleState, MaskState, OpacityState, EnableBackgroundState,
        FilterState, FloodColorState, FloodOpacityState, LightingColorState, StopColorState, StopOpacityState,
        PointerEventsState, ColorInterpolationState, ColorInterpolationFiltersState, ColorProfileState,
        ColorRenderingState, FillState, FillOpacityState, FillRuleState, ImageRenderingState,
        MarkerState, MarkerEndState, MarkerMidState, MarkerStartState, ShapeRenderingState,
        StrokeState, StrokeDashArrayState, StrokeDashOffsetState, StrokeLineCapState, StrokeLineJoinState,
        StrokeMiterLimitState, StrokeOpacityState, StrokeWidthState, TextRenderingState,
        AlignmentBaselineState, BaselineShiftState, DominantBaselineState, GlyphOrientationHorizontalState,
        GlyphOrientationVerticalState, KerningState, TextAnchorState, WritingModeState } ;

    enum FillRule { EvenOddFillRule, NonZeroFillRule } ;
    enum LineJoinType { MiterLineJoin, RoundLineJoin, BevelLineJoin } ;
    enum LineCapType { ButtLineCap, RoundLineCap, SquareLineCap } ;
    enum FontStyle { NormalFontStyle, ObliqueFontStyle, ItalicFontStyle } ;
    enum FontVariant { NormalFontVariant, SmallCapsFontVariant } ;
    enum FontWeight { NormalFontWeight, BoldFontWeight, BolderFontWeight, LighterFontWeight,
        W100FontWeight, W200FontWeight, W300FontWeight, W400FontWeight, W500FontWeight,
        W600FontWeight, W700FontWeight, W800FontWeight, W900FontWeight } ;
    enum FontStretch { UltraCondensedFontStretch, ExtraCondensedFontStretch, CondensedFontStretch,
        NarrowerFontStretch, SemiCondensedFontStretch, SemiExpandedFontStretch, ExpandedFontStretch,
        WiderFontStretch, ExtraExpandedFontStretch, UltraExpandedFontStretch } ;
    enum TextDecoration { UnderlineDecoration, OverlineDecoration, StrikeDecoration } ;
    enum TextAnchor { StartTextAnchor, MiddleTextAnchor, EndTextAnchor } ;
    enum ShapeQuality { AutoShapeQuality, OptimizeSpeedShapeQuality, CrispEdgesShapeQuality, GeometricPrecisionShapeQuality } ;
    enum TextQuality { AutoTextQuality, OptimizeSpeedTextQuality, OptimizeLegibilityTextQuality, GeometricPrecisionTextQuality } ;

    enum DisplayMode { NoDisplay, InlineDisplay, BBoxDisplay } ;
    enum VisibilityMode { NoVisibility, HiddenVisibility} ;

    enum PaintType { NoPaint, SolidColorPaint, CurrentColorPaint, PaintServerPaint } ;

    Style() ;
    Style(const Style &other)
    {
        *this = other ;
        //?if ( stroke_paint_type_ == PaintServerPaint ) strokePaint.paintServerId = _wcsdup(other.strokePaint.paintServerId);
      //?if ( fillPaintType == PaintServerPaint ) fillPaint.paintServerId = _wcsdup(other.fillPaint.paintServerId);
    }

    ~Style() {
        if ( stroke_paint_type_ == PaintServerPaint ) delete stroke_paint_.paint_server_id_ ;
        if ( fill_paint_type_ == PaintServerPaint ) delete fill_paint_.paint_server_id_;
    }

    bool parseNameValue(const std::string &name, const std::string &val) ;

    bool fromStyleString(const std::string &str) ;

    static bool parseColor(const std::string &str, unsigned int &clr) ;
    static unsigned int parseOpacity(const std::string &str) ;

    void parsePaint(const std::string &str, bool fill)  ;

    bool hasFlag(Flag f) const ;
    void resetNonInheritable() ;

    std::vector<Flag> flags_ ;

    PaintType stroke_paint_type_, fill_paint_type_ ;

    union Paint {
        unsigned int clr_ ;
        const char *paint_server_id_ ;
    } stroke_paint_, fill_paint_ ;

    FillRule fill_rule_ ;
    unsigned int fill_opacity_, stroke_opacity_, opacity_ ;
    Length stroke_width_ ;
    float miter_limit_ ;
    bool solid_stroke_ ;

    std::vector<Length> dash_array_ ;
    Length dash_offset_ ;
    LineJoinType line_join_ ;
    LineCapType line_cap_ ;
    std::string font_family_ ;
    FontStyle font_style_ ;
    FontVariant font_variant_ ;
    FontWeight font_weight_ ;
    Length font_size_ ;
    FontStretch font_stretch_ ;
    TextAnchor text_anchor_ ;
    TextDecoration text_decoration_ ;
    DisplayMode display_mode_ ;
    VisibilityMode visibility_mode_ ;
    ShapeQuality shape_rendering_quality_ ;
    TextQuality text_rendering_quality_ ;
    unsigned int stop_color_ ;
    unsigned int stop_opacity_ ;
    bool overflow_ ;
    std::string clip_path_id_ ;

} ;

#define DASH_ARRAY_LENGTH 10

struct Color {

    Color(double r, double g, double b, double a = 1.0) ;

    // initialize from a CSS2 name or rgb specification
    Color(const char *name, double alpha = 1.0) ;

    double r_, g_, b_, a_ ;
};


class PointList {
  public:

    bool fromString(const std::string &str) ;

    std::vector<float> points_ ;

} ;

class Transform
{
  public:

    Transform() ;
    Transform(double s0, double s1, double s2, double s3, double s4, double s5) ;

    bool fromString(const std::string &str) ;
    static Transform inverse(const Transform &) ;
    static Transform flip(const Transform &src, bool horz, bool vert) ;
    static Transform multiply(const Transform &src1, const Transform &src2) ;
    static Transform identity() ;
    static Transform scaling(double sx, double sy) ;
    static Transform rotation(double theta) ;
    static Transform shearing(double theta) ;
    static Transform translation(double tx, double ty) ;
    static double expansion(const Transform &src) ;
    bool isRectilinear(const Transform &src) ;
    bool isEqual(const Transform &src1, const Transform &src2) ;

    double m_[6] ;
} ;


enum FillRule { EvenOddFillRule, NonZeroFillRule } ;

// abstract class for all brush type

class PreserveAspectRatio
{
  public:

    enum ViewBoxAlign { NoViewBoxAlign, XMinYMin, XMidYMin, XMaxYMin, XMinYMid, XMidYMid, XMaxYMid,
        XMinYMax, XMidYMax, XMaxYMax } ;
    enum ViewBoxPolicy { MeetViewBoxPolicy, SliceViewBoxPolicy } ;

    PreserveAspectRatio(): view_box_align_(XMidYMid), view_box_policy_(MeetViewBoxPolicy), defer_aspect_ratio_(false) {}

    bool fromString(const std::string &str) ;
    void constrainViewBox(double width, double height, ViewBox &orig) ;
    Transform getViewBoxTransform(double sw, double sh, double vwidth, double vheight, double vx, double vy);

    bool defer_aspect_ratio_ ;
    ViewBoxAlign view_box_align_ ;
    ViewBoxPolicy view_box_policy_ ;
} ;

class Stylable
{

public:

    virtual ~Stylable() {}

    bool parseAttributes(const pugi::xml_node &p) ;

    Style style_ ;
} ;

class Transformable
{
    public:

    virtual ~Transformable() {}

    bool parseAttributes(const pugi::xml_node &pNode) ;

    Transform trans_ ;
} ;

class FitToViewBox
{
  public:

    bool parseAttributes(const pugi::xml_node &pNode) ;

    ViewBox view_box_ ;
    PreserveAspectRatio preserve_aspect_ratio_ ;

} ;

class RenderingContext ;

class Element
{
  public:

    enum Type { DocumentElement, GroupElement, AnchorElement, PathElement, SwitchElement, DefsElement, UseElement, LineElement,
        RectElement, EllipseElement, CircleElement, PolygonElement, PolyLineElement, SymbolElement,
        MaskElement, ClipPathElement, ImageElement, MarkerElement, StopElement, PatternElement,
        LinearGradientElement, ConicalGradientElement, RadialGradientElement, FilterElement, MyltiImageElement,
        SubImageRefElement, SubImageElement, TextElement, TSpanElement, TRefElement, StyleElement
    } ;

  public:

    Element() { }
    virtual ~Element() { }

    bool parseAttributes(const pugi::xml_node &pNode) ;
    virtual Type getType() const = 0 ;
    virtual bool fromXml(const pugi::xml_node &pNode) = 0 ;
    virtual void render(RenderingContext *) {}

    std::string id_ ;
    Element *parent_ ;

} ;

typedef std::shared_ptr<Element> ElementPtr ;

class StyleDefinition: public Element
{
  public:

    StyleDefinition() {}

    Type getType() const { return Element::StyleElement ; }
    virtual bool fromXml(const pugi::xml_node &pNode) ;

    std::string type_, media_ ;
} ;

class StylableElement: public Element, public Stylable {
} ;

class Container {

public:

    virtual ~Container() ;
    std::vector<ElementPtr> children_ ;
};

class GradientElement: public Element, public Stylable
{
  public:

    enum SpreadMethod { UnknownSpreadMethod, PadSpreadMethod, ReflectSpreadMethod, RepeatSpreadMethod } ;
    enum GradientUnits { UserSpaceOnUse, ObjectBoundingBox } ;

    GradientElement(): spread_method_(PadSpreadMethod), gradient_units_(ObjectBoundingBox) { }
    bool parseAttributes(const pugi::xml_node &pNode) ;

    bool parseStops(const pugi::xml_node &pNode) ;

    enum Fallback { SpreadMethodDefined = 0x01, GradientUnitsDefined = 0x02, TransformDefined = 0x04 } ;
    void resolveHRef(RenderingContext *ctx) ;

    SpreadMethod spread_method_ ;
    GradientUnits gradient_units_ ;
    Transform trans_ ;

    std::vector<Stop> stops_ ;

    int fallback_ ;
    std::string href_ ;

} ;

class LinearGradient: public GradientElement
{
  public:

    LinearGradient() {
        x1_ = y1_ = y2_ = x2_ = Length(Length::UnknownLengthType, 0.5) ;
    }

    Type getType() const { return LinearGradientElement ; }
    bool fromXml(const pugi::xml_node &pNode) ;


    Length x1_, y1_, x2_, y2_ ;
} ;

class RadialGradient: public GradientElement
{
  public:

    RadialGradient() {
        cx_ = cy_ = r_ = fx_ = fy_  = Length(Length::UnknownLengthType, 0.5) ;
    }

    Type getType() const { return RadialGradientElement ; }
    bool fromXml(const pugi::xml_node &pNode) ;

    Length cx_, cy_, r_, fx_, fy_ ;
} ;

class Pattern: public Element, public Stylable, public FitToViewBox, public Container
{
  public:

    enum PatternUnits { UserSpaceOnUse, ObjectBoundingBox } ;

    Pattern(): pattern_content_units_(UserSpaceOnUse), pattern_units_(ObjectBoundingBox) {  }

    Type getType() const { return Element::PatternElement ; }

    bool parseAttributes(const pugi::xml_node &pNode) ;
    bool fromXml(const pugi::xml_node &pNode) ;

    void render(RenderingContext *ctx) ;

    enum Fallback { PatternUnitsDefined = 0x01, PatternContentUnitsDefined = 0x02, TransformDefined = 0x04 } ;

    void ResolveHRef(RenderingContext *ctx) ;

    PatternUnits pattern_units_ ;
    PatternUnits pattern_content_units_ ;
    Transform trans_ ;

    int fallback_ ;
    std::string href_ ;
    Length x_, y_, width_, height_ ;

} ;


class Image: public Element, public Transformable, public Stylable
{
public:

    Image() {
        x_ = y_ = width_ = height_ = Length(Length::NumberLengthType, 0) ;
    }
    Type getType() const { return ImageElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    virtual void render(RenderingContext *)  ;

    std::string img_path_ ;
    Length x_, y_, width_, height_ ;
    PreserveAspectRatio preserve_aspect_ratio_ ;
} ;

class ClipPath: public Element, public Transformable, public Stylable, public Container
{
  public:

    enum ClipPathUnits { UserSpaceOnUse, ObjectBoundingBox } ;

    ClipPath(): clip_path_units_(UserSpaceOnUse) {}

    Type getType() const { return ClipPathElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;

    virtual void render(RenderingContext *) ;

    ClipPathUnits clip_path_units_ ;

} ;

class Use: public Element, public Transformable, public Stylable
{
     public:

    Use() { x_ = y_ =  Length(Length::NumberLengthType, 0.0) ; }
    ~Use() { }

    Type getType() const { return UseElement ; }

    bool fromXml(const pugi::xml_node &pNode)  ;
    virtual void render(RenderingContext *) ;

    Length x_, y_, width_, height_ ;
    std::string ref_id_ ;
} ;


class Group: public Element, public Transformable, public Stylable, public Container
{
  public:

    Group() {}

    Type getType() const { return GroupElement ; }

    bool fromXml(const pugi::xml_node &pNode)  ;
    virtual void render(RenderingContext *)  ;
} ;

class Defs: public Element, public Transformable, public Stylable, public Container
{
  public:

    Defs() {}

    Type getType() const { return DefsElement ; }

    bool fromXml(const pugi::xml_node &pNode)  ;
    virtual void render(RenderingContext *)  {}
} ;

class Path: public Element, public Transformable, public Stylable
{
  public:

    Path() {}

    Type getType() const { return PathElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    virtual void render(RenderingContext *) ;

    PathData data_ ;
} ;

class Rect: public Element, public Stylable, public Transformable
{
  public:

    Rect() {}

    Type getType() const { return RectElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    virtual void render(RenderingContext *)  ;

    Length x_, y_, width_, height_, rx_, ry_ ;

} ;

class Circle: public Element,  public Stylable, public Transformable
{
  public:

    Circle() {}

    Type getType() const { return CircleElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *) ;

    Length cx_, cy_, r_ ;

} ;

class Line: public Element,  public Stylable, public Transformable
{
  public:

    Line() {}

    Type getType() const { return LineElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *) ;

    Length x1_, y1_, x2_, y2_ ;
} ;

class Ellipse: public Element, public Stylable, public Transformable
{
  public:

    Ellipse() {}

    Type getType() const { return EllipseElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) ;

    Length cx_, cy_, rx_, ry_ ;

} ;

class PolyLine: public Element, public Stylable, public Transformable
{
  public:

    PolyLine() {}

    Type getType() const { return PolyLineElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) ;

    PointList points_ ;
} ;

class Polygon: public Element , public Stylable, public Transformable
{
  public:

    Polygon() {}

    Type getType() const { return PolygonElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) ;

    PointList points_ ;

} ;

class TextPosElement: public Element, public Stylable
{
  public:

    TextPosElement() {
        x_ = y_ = Length(Length::UnknownLengthType, 0.0) ;
        dx_ = dy_ = Length(Length::NumberLengthType, 0.0) ;
        preserve_white_ = false ;
    }
    bool parseAttributes(const pugi::xml_node &pNode) ;

    Length x_, y_, dx_, dy_ ;
    bool preserve_white_ ;

} ;

class SpanElement ;
class Text: public TextPosElement, public Transformable, public Container
{
  public:

    Text() {}


    Type getType() const { return TextElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) ;
} ;


class SpanElement: public TextPosElement, public Container
{
  public:

    SpanElement() {}

    Type getType() const { return TSpanElement ; }

    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) ;

    std::string text_ ;

} ;

class Document: public Element, public Stylable, public FitToViewBox, public Container {

  public:

    Document(): x_{Length::NumberLengthType, 0.0}, y_{Length::NumberLengthType, 0.0},
        width_{Length::PercentageLengthType, 1.0}, height_{Length::PercentageLengthType, 1.0} {
    }

    Type getType() const { return DocumentElement ; }
    bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) ;

    void parseCSS(const std::string &str) ;

    Length x_, y_, width_, height_ ;

} ;

class Symbol: public Document {

  public:

    Symbol() {}

    Type getType() const { return SymbolElement ; }
    //bool fromXml(const pugi::xml_node &pNode) ;
    void render(RenderingContext *ctx) {}

} ;








} // namespace svg







#endif

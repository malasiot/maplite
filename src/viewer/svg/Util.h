#ifndef __SVG_PRIVATE_H__
#define __SVG_PRIVATE_H__

#include <boost/shared_ptr.hpp>
#include <vector>
#include <stdexcept>
#include <string.h>

namespace svg {

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
            args[0] = arg1 ; args[1] = arg2 ; args[2] = arg3 ; args[3] = arg4 ; args[4] = arg5 ;
            args[5] = arg6 ;
        }
        double args[6] ;
      Command cmd ;
    } ;

    std::vector<Element> elements ;
} ;

class RenderingContext ;

class Length
{
  public:

    enum UnitType { UnknownLengthType, NumberLengthType, EMSLengthType, EXSLengthType, PXLengthType,
            CMLengthType, MMLengthType, INLengthType, PTLengthType, PCLengthType, PercentageLengthType } ;

    enum Direction { AbsoluteDir, HorzDir, VertDir, ViewportDir } ;

    Length() { unitType = UnknownLengthType ; valueInSpecifiedUnits = 0 ; } ;
    Length(UnitType unit, double val, bool scale = false): unitType(unit), valueInSpecifiedUnits(val), scaleToViewport(scale) {} ;

    bool fromString(const std::string &str, bool scale = false)  ;

    static bool parseList(const std::string &str, std::vector<Length> &ls) ;
    double toPixels(RenderingContext *ctx, Direction dir = AbsoluteDir) const ;

    bool unknown() const {
        return unitType == UnknownLengthType ;
    }

    UnitType unitType ;
    float valueInSpecifiedUnits ;
    bool scaleToViewport ;
};


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
        //?if ( strokePaintType == PaintServerPaint ) strokePaint.paintServerId = _wcsdup(other.strokePaint.paintServerId);
      //?if ( fillPaintType == PaintServerPaint ) fillPaint.paintServerId = _wcsdup(other.fillPaint.paintServerId);
    }

    ~Style() {
        if ( strokePaintType == PaintServerPaint ) delete strokePaint.paintServerId ;
        if ( fillPaintType == PaintServerPaint ) delete fillPaint.paintServerId ;
    }

    bool parseNameValue(const std::string &name, const std::string &val) ;

    bool fromStyleString(const std::string &str) ;

    static bool parseColor(const std::string &str, unsigned int &clr) ;
    static unsigned int parseOpacity(const std::string &str) ;

    void parsePaint(const std::string &str, bool fill)  ;

    bool hasFlag(Flag f) const ;
    void resetNonInheritable() ;

    std::vector<Flag> flags ;

    PaintType strokePaintType, fillPaintType ;

    union Paint {
        unsigned int clr ;
        const char *paintServerId ;
    } strokePaint, fillPaint ;

    FillRule fillRule ;
    unsigned int fillOpacity, strokeOpacity, opacity ;
    Length strokeWidth ;
    float miterLimit ;
    bool solidStroke ;

    std::vector<Length> dashArray ;
    Length dashOffset ;
    LineJoinType lineJoin ;
    LineCapType lineCap ;
    std::string fontFamily ;
    FontStyle fontStyle ;
    FontVariant fontVariant ;
    FontWeight fontWeight ;
    Length fontSize ;
    FontStretch fontStretch ;
    TextAnchor textAnchor ;
    TextDecoration textDecoration ;
    DisplayMode displayMode ;
    VisibilityMode visibilityMode ;
    ShapeQuality shapeRenderingQuality ;
    TextQuality textRenderingQuality ;
    unsigned int stopColor ;
    unsigned int stopOpacity ;
    bool overflow ;
    std::string clipPathId ;

} ;

#define DASH_ARRAY_LENGTH 10

struct Color {

    Color(double r, double g, double b, double a = 1.0) ;

    // initialize from a CSS2 name or rgb specification
    Color(const char *name, double alpha = 1.0) ;

    double r, g, b, a ;
};


class PointList {
  public:

    bool fromString(const std::string &str) ;

    std::vector<float> points ;

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

    double m[6] ;
} ;


enum FillRule { EvenOddFillRule, NonZeroFillRule } ;

// abstract class for all brush type


class PreserveAspectRatio
{
  public:

    enum ViewBoxAlign { NoViewBoxAlign, XMinYMin, XMidYMin, XMaxYMin, XMinYMid, XMidYMid, XMaxYMid,
        XMinYMax, XMidYMax, XMaxYMax } ;
    enum ViewBoxPolicy { MeetViewBoxPolicy, SliceViewBoxPolicy } ;

    PreserveAspectRatio(): viewBoxAlign(XMidYMid), viewBoxPolicy(MeetViewBoxPolicy), deferAspectRatio(false) {} ;

    bool fromString(const std::string &str) ;
    void constrainViewBox(double width, double height, ViewBox &orig) ;
    Transform getViewBoxTransform(double sw, double sh, double vwidth, double vheight, double vx, double vy);

    bool deferAspectRatio ;
    ViewBoxAlign viewBoxAlign ;
    ViewBoxPolicy viewBoxPolicy ;

} ;


void eatwhite(const char *&p) ;
void eatwhite_comma(const char *&p) ;
void css_parse_number_list(const char *&p, char terminator, std::vector<double> &numbers) ;
bool parseFloatingPoint(const std::string &str, double &val) ;

std::string parseUri(const std::string &str) ;

} // namespace svg














#endif

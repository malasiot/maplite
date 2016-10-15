#ifndef __SVG_DOCUMENT_H__
#define __SVG_DOCUMENT_H__

#include "Util.h"
#include "XmlDocument.h"

#include <cairo.h>
#include <string.h>

namespace svg {

class Stop
{
  public:

    Stop() {
        offset.fromString("0.0") ; stopColor = 0 ; stopOpacity = 0xff ;
    }

    bool fromXml(const XmlElement *pNode) ;

    Length offset ;
    unsigned int stopColor ;
    int stopOpacity ;
} ;

class Stylable
{

public:

    virtual ~Stylable() {}

    bool parseAttributes(const XmlElement *pNode) ;

    Style style ;
} ;

class Transformable
{
    public:

    virtual ~Transformable() {}

    bool parseAttributes(const XmlElement *pNode) ;

    Transform trans ;
} ;

class FitToViewBox
{
  public:

    bool parseAttributes(const XmlElement *pNode) ;

    ViewBox viewBox ;
    PreserveAspectRatio preserveAspectRatio ;

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

    bool parseAttributes(const XmlElement *pNode) ;
    virtual Type getType() const = 0 ;
    virtual bool fromXml(const XmlElement *pNode) = 0 ;
    virtual void render(RenderingContext *) {}

    std::string id ;
    Element *parent ;

} ;

class StyleDefinition: public Element
{
  public:

    StyleDefinition() {}

    Type getType() const { return Element::StyleElement ; }
    virtual bool fromXml(const XmlElement *pNode) ;

    std::string type, media ;
} ;

class StylableElement: public Element, public Stylable {
} ;

class Container {

public:

    virtual ~Container() ;
    std::vector<Element *> children ;
};

class GradientElement: public Element, public Stylable
{
  public:

    enum SpreadMethod { UnknownSpreadMethod, PadSpreadMethod, ReflectSpreadMethod, RepeatSpreadMethod } ;
    enum GradientUnits { UserSpaceOnUse, ObjectBoundingBox } ;

    GradientElement() { spreadMethod = PadSpreadMethod ; gradientUnits = ObjectBoundingBox ; }
    bool parseAttributes(const XmlElement *pNode) ;

    bool parseStops(const XmlElement *pNode) ;

    enum Fallback { SpreadMethodDefined = 0x01, GradientUnitsDefined = 0x02, TransformDefined = 0x04 } ;
    void resolveHRef(RenderingContext *ctx) ;

    SpreadMethod spreadMethod ;
    GradientUnits gradientUnits ;
    Transform trans ;

    std::vector<Stop> stops ;

    int fallback ;
    std::string href ;

} ;

class LinearGradient: public GradientElement
{
  public:

    LinearGradient() {
        x1 = y1 = y2 = x2 = Length(Length::UnknownLengthType, 0.5) ;
    }

    Type getType() const { return LinearGradientElement ; }
    bool fromXml(const XmlElement *pNode) ;


    Length x1, y1, x2, y2 ;
} ;

class RadialGradient: public GradientElement
{
  public:

    RadialGradient() {
        cx = cy = r = fx = fy  = Length(Length::UnknownLengthType, 0.5) ;
    }

    Type getType() const { return RadialGradientElement ; }
    bool fromXml(const XmlElement *pNode) ;

    Length cx, cy, r, fx, fy;
} ;

class Pattern: public Element, public Stylable, public FitToViewBox, public Container
{
  public:

    enum PatternUnits { UserSpaceOnUse, ObjectBoundingBox } ;

    Pattern() { patternContentUnits = UserSpaceOnUse ; patternUnits = ObjectBoundingBox ; }

    Type getType() const { return Element::PatternElement ; }

    bool parseAttributes(const XmlElement *pNode) ;
    bool fromXml(const XmlElement *pNode) ;

    void render(RenderingContext *ctx) ;

    enum Fallback { PatternUnitsDefined = 0x01, PatternContentUnitsDefined = 0x02, TransformDefined = 0x04 } ;
    void ResolveHRef(RenderingContext *ctx) ;

    PatternUnits patternUnits ;
    PatternUnits patternContentUnits ;
    Transform trans ;

    int fallback ;
    std::string href ;
    Length x, y, width, height ;

} ;


class Image: public Element, public Transformable, public Stylable
{
public:

    Image() {
        x = y = width = height = Length(Length::NumberLengthType, 0) ;
    }
    Type getType() const { return ImageElement ; }

    bool fromXml(const XmlElement *pNode) ;
    virtual void render(RenderingContext *)  ;

    std::string img_path ;
    Length x, y, width, height ;
    PreserveAspectRatio preserveAspectRatio ;
} ;

class ClipPath: public Element, public Transformable, public Stylable, public Container
{
  public:

    enum ClipPathUnits { UserSpaceOnUse, ObjectBoundingBox } ;

    ClipPath() { clipPathUnits = UserSpaceOnUse ; }
    ~ClipPath() { for(int i=0 ; i<children.size() ; i++ ) delete children[i] ; }

    Type getType() const { return ClipPathElement ; }

    bool fromXml(const XmlElement *pNode) ;
    virtual void render(RenderingContext *) ;

    ClipPathUnits clipPathUnits ;

} ;

class Use: public Element, public Transformable, public Stylable
{
     public:

    Use() { x = y =  Length(Length::NumberLengthType, 0.0) ;   }
    ~Use() { }

    Type getType() const { return UseElement ; }

    bool fromXml(const XmlElement *pNode)  ;
    virtual void render(RenderingContext *) ;

    Length x, y, width, height ;
    std::string refId ;
} ;


class Group: public Element, public Transformable, public Stylable, public Container
{
  public:

    Group() {} ;
 //   ~Group() { for(int i=0 ; i<children.size() ; i++ ) delete children[i] ; }

    Type getType() const { return GroupElement ; }

    bool fromXml(const XmlElement *pNode)  ;
    virtual void render(RenderingContext *)  ;

} ;

class Defs: public Element, public Transformable, public Stylable, public Container
{
  public:

    Defs() {}

    Type getType() const { return DefsElement ; }

    bool fromXml(const XmlElement *pNode)  ;
    virtual void render(RenderingContext *)  {}
} ;

class Path: public Element, public Transformable, public Stylable
{
  public:

    Path() {}

    Type getType() const { return PathElement ; }

    bool fromXml(const XmlElement *pNode) ;
    virtual void render(RenderingContext *) ;

    PathData data ;
} ;

class Rect: public Element, public Stylable, public Transformable
{
  public:

    Rect() {}

    Type getType() const { return RectElement ; }

    bool fromXml(const XmlElement *pNode) ;
    virtual void render(RenderingContext *)  ;

    Length x, y, width, height, rx, ry ;

} ;

class Circle: public Element,  public Stylable, public Transformable
{
  public:

    Circle() {}

    Type getType() const { return CircleElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *) ;

    Length cx, cy, r ;

} ;

class Line: public Element,  public Stylable, public Transformable
{
  public:

    Line() {}

    Type getType() const { return LineElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *) ;

    Length x1, y1, x2, y2 ;
} ;

class Ellipse: public Element, public Stylable, public Transformable
{
  public:

    Ellipse() {};

    Type getType() const { return EllipseElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) ;

    Length cx, cy, rx, ry ;

} ;

class PolyLine: public Element, public Stylable, public Transformable
{
  public:

    PolyLine() {};

    Type getType() const { return PolyLineElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) ;

    PointList points ;
} ;

class Polygon: public Element , public Stylable, public Transformable
{
  public:

    Polygon() {}

    Type getType() const { return PolygonElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) ;

    PointList points ;

} ;

class TextPosElement: public Element, public Stylable
{
  public:

    TextPosElement() {
        x = y = Length(Length::UnknownLengthType, 0.0) ;
        dx = dy = Length(Length::NumberLengthType, 0.0) ;
        preserveWhite = false ;
    }
    bool parseAttributes(const XmlElement *pNode) ;

    Length x, y, dx, dy ;
    bool preserveWhite ;

} ;

class SpanElement ;
class Text: public TextPosElement, public Transformable, public Container
{
  public:

    Text() {}


    Type getType() const { return TextElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) ;


} ;

class SpanElement: public TextPosElement, public Container
{
  public:

    SpanElement() {}

    Type getType() const { return TSpanElement ; }

    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) ;

    std::string text ;
    std::vector<SpanElement *> elements ;
} ;

class Document: public Element, public Stylable, public FitToViewBox, public Container {

  public:

    Document() { x = y =  Length(Length::NumberLengthType, 0.0) ;
        width = height = Length(Length::PercentageLengthType, 1.0) ;
    }

    Type getType() const { return DocumentElement ; }
    bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) ;

    void parseCSS(const std::string &str) ;

    Length x, y, width, height ;

} ;

class Symbol: public Document {

  public:

    Symbol() {}

    Type getType() const { return SymbolElement ; }
    //bool fromXml(const XmlElement *pNode) ;
    void render(RenderingContext *ctx) {}

} ;


} // namespace svg







#endif

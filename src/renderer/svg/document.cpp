#include "document.hpp"
#include "rendering.hpp"
#include "util.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <cerrno>
#include <math.h>

using namespace std ;



namespace svg {

const char *NS_SVG = "http://www.w3.org/2000/svg" ;

#define SAFE_ASSIGN(x) if (!(x)) return false ;
#define XML_FOREACH_ATTRIBUTE(p, a) for( const pugi::xml_attribute &attr: p.attributes())
#define XML_FOREACH_CHILD_ELEMENT(p, q) for( const pugi::xml_node &q:  p.children())

bool Element::parseAttributes(const pugi::xml_node &pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string aname = attr.name() ;

        if ( aname == "id" )
            id_ = attr.as_string() ;
    }

    return true ;
}

bool Stylable::parseAttributes(const pugi::xml_node &pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "style" ) {
           if ( !style_.fromStyleString(val) ) return false;
        }
        else
           if ( !style_.parseNameValue(key, val) ) return false ;
    }

    return true ;
}


bool GradientElement::parseAttributes(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    fallback_ = 0 ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "gradientUnits" )
        {
            if ( val == "userSpaceOnUse" )
                gradient_units_ = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                gradient_units_ = ObjectBoundingBox ;

            fallback_ |= GradientUnitsDefined ;
        }
        else if ( key == "gradientTransform" )
        {
            if ( !trans_.fromString(val) ) return false ;

            fallback_ |= TransformDefined ;
        }
        else if ( key == "spreadMethod" )
        {
            if ( val == "pad" )
                spread_method_ = PadSpreadMethod ;
            else if ( val == "repeat" )
                spread_method_ = RepeatSpreadMethod ;
            else if ( val == "reflect" )
                spread_method_ = ReflectSpreadMethod ;

            fallback_ |= SpreadMethodDefined ;
        }
        else if ( key == "xlink:href" )
        {
            href_ = val ;
        }
    }

    return true ;
}


Container::~Container()
{

}

bool Pattern::parseAttributes(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !FitToViewBox::parseAttributes(pNode) ) return false ;

    fallback_ = 0 ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "patternUnits" )
        {
            if ( val == "userSpaceOnUse" )
                pattern_units_ = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                pattern_units_ = ObjectBoundingBox ;

            fallback_ |= PatternUnitsDefined ;
        }
        else if ( key == "patternContentUnits" )
        {
            if ( val == "userSpaceOnUse" )
                pattern_content_units_ = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                pattern_content_units_ = ObjectBoundingBox ;

            fallback_ |= PatternContentUnitsDefined ;
        }
        else if ( key == "patternTransform" )
        {
            if ( !trans_.fromString(val) ) return false ;

            fallback_ |= TransformDefined ;
        }
        else if ( key  == "xlink:href" )
        {
           href_ = val ;
        }
        else if ( key == "x" ) {
            if ( !x_.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y_.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width_.fromString(val, true) ) return false ;
        }
        else if ( key == "height" ) {
            if (!height_.fromString(val, true) ) return false ;
        }
    }

    return true ;
}


bool Transformable::parseAttributes(const pugi::xml_node &pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "transform" )
            if ( !trans_.fromString(val) ) return false ;
    }

    return true ;
}

bool FitToViewBox::parseAttributes(const pugi::xml_node &pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "viewBox" )  {
           if ( !view_box_.fromString(val) ) return false ;
        }
        else if ( key == "preserveAspectRatio" ) {
           if ( !preserve_aspect_ratio_.fromString(val) ) return false ;
        }
    }

    return true ;
}

bool TextPosElement::parseAttributes(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x" ) {
            if ( !x_.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y_.fromString(val) ) return false ;
        }
        else if ( key == "dx" ) {
            if ( !dx_.fromString(val) ) return false ;
        }
        else if ( key == "dy" ) {
            if ( !dy_.fromString(val) ) return false ;
        }
        else if ( key == "xml::space" ) {
            if ( val == "default" )
                preserve_white_ = false ;
            else if ( val == "preserve" )
                preserve_white_ = true ;
        }
    }

    return true ;
}

bool Document::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !FitToViewBox::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x" ) ;
        else if ( key == "y" ) ;
        else if ( key == "width" ) {
           if ( !width_.fromString(val, true) ) return false ;
        }
        else if ( key == "height" ) {
           if ( !height_.fromString(val, true) ) return false ;
        }
    }

    //string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;
    string nsPrefixSVG ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        ElementPtr pElem ;

        if ( qNode.name() == nsPrefixSVG + "g" )
            pElem.reset(new Group) ;
        else if ( qNode.name() == nsPrefixSVG + "svg" )
            pElem.reset(new Document) ;
        else if ( qNode.name() == nsPrefixSVG + "rect" )
            pElem.reset(new Rect) ;
        else if ( qNode.name() == nsPrefixSVG + "line" )
            pElem.reset(new Line) ;
        else if ( qNode.name() == nsPrefixSVG + "path" )
            pElem.reset(new Path) ;
        else if ( qNode.name() == nsPrefixSVG + "ellipse" )
            pElem.reset(new Ellipse) ;
        else if ( qNode.name() == nsPrefixSVG + "polygon" )
            pElem.reset(new Polygon) ;
        else if ( qNode.name() == nsPrefixSVG + "polyline" )
            pElem.reset(new PolyLine) ;
        else if ( qNode.name() == nsPrefixSVG + "circle" )
            pElem.reset(new Circle) ;
        else if ( qNode.name() == nsPrefixSVG + "text" )
            pElem.reset(new Text) ;
        else if ( qNode.name() == nsPrefixSVG + "defs" )
            pElem.reset(new Defs) ;
        else if ( qNode.name() == nsPrefixSVG + "symbol" )
            pElem.reset(new Symbol) ;
        else if ( qNode.name() == nsPrefixSVG + "linearGradient" )
            pElem.reset(new LinearGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "radialGradient" )
            pElem.reset(new RadialGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "use" )
            pElem.reset(new Use) ;
        else if ( qNode.name() == nsPrefixSVG + "image" )
            pElem.reset(new Image) ;
        else if ( qNode.name() == nsPrefixSVG + "pattern" )
            pElem.reset(new Pattern) ;
        else if ( qNode.name() == nsPrefixSVG + "clipPath" )
            pElem.reset(new ClipPath) ;
        else if ( qNode.name() == nsPrefixSVG + "style" )
            pElem.reset(new StyleDefinition) ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children_.push_back(pElem) ;
        pElem->parent_ = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node

    }

    return true ;
}

bool StyleDefinition::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "type" ) type_ = val ;
        else if ( key == "media" ) media_ = val ;
    }

    if ( type_ != "text/css" ) return false ;

    string cssStr = pNode.value() ;

    return true ;
}


bool Image::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x" ) {
            if ( !x_.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y_.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width_.fromString(val, true) ) return false ;
        }
        else if ( key == "height" ) {
            if ( !height_.fromString(val, true) ) return false ;
        }
        else if ( key == "preserve_aspect_ratio_" ) {
            if ( !preserve_aspect_ratio_.fromString(val) ) return false ;
        }
        else if ( key == "xlink:href" )
            img_path_ = val ;

    }
    return true ;
}


bool ClipPath::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "clipPathUnits" )
        {
            if ( val == "userSpaceOnUse" )
                clip_path_units_ = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                clip_path_units_ = ObjectBoundingBox ;
        }
    }

//    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;
    string nsPrefixSVG ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        ElementPtr pElem  ;

        if ( qNode.name() == nsPrefixSVG + "path" )
            pElem.reset(new Path) ;
        else if ( qNode.name() == nsPrefixSVG + "text" )
            pElem.reset(new Text) ;
        else if ( qNode.name() == nsPrefixSVG + "rect" )
            pElem.reset(new Rect) ;
        else if ( qNode.name() == nsPrefixSVG + "line" )
            pElem.reset(new Line) ;
        else if ( qNode.name() == nsPrefixSVG + "ellipse" )
            pElem.reset(new Ellipse) ;
        else if ( qNode.name() == nsPrefixSVG + "polygon" )
            pElem.reset(new Polygon) ;
        else if ( qNode.name() == nsPrefixSVG + "polyline" )
            pElem.reset(new PolyLine) ;
        else if ( qNode.name() == nsPrefixSVG + "circle" )
            pElem.reset(new Circle) ;
        else if ( qNode.name() == nsPrefixSVG + "use" )
            pElem.reset(new Use) ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children_.push_back(pElem) ;
        pElem->parent_ = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node
    }

    return true ;
}


bool Use::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x" ) {
            if ( !x_.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y_.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width_.fromString(val) ) return false ;
        }
        else if ( key == "height" ) {
            if ( !height_.fromString(val) ) return false ;
        }
        else if ( key == "xlink:href" )
        {
            ref_id_ = val ;
        }
    }

    return true ;
}



bool Text::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !TextPosElement::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "xml::space" ) {
            if ( val == "default" )
                preserve_white_ = false ;
            else if ( val == "preserve" )
                preserve_white_ = true ;
        }
    }

    XML_FOREACH_CHILD_ELEMENT(pNode, node)
    {
        if ( node.type() == pugi::node_pcdata )
        {
            SpanElement *elem = new SpanElement() ;
            elem->x_ = x_ ;
            elem->y_ = y_ ;
            elem->text_ = node.text().as_string() ;
            children_.push_back(ElementPtr(elem)) ;
            elem->parent_ = this ;
        }
        else if ( node.type() == pugi::node_element &&
                  strcmp(node.name(), "tspan" ) == 0 )
        {
            SpanElement *elem = new SpanElement() ;

            if ( elem->fromXml(node) )
            {
                children_.push_back(ElementPtr(elem)) ;
                elem->parent_ = this ;
            }
            else delete elem ;
        }
    }

    return true ;
}


bool SpanElement::fromXml(const pugi::xml_node &pNode)
{
  // Parse attributes

    if ( !TextPosElement::parseAttributes(pNode) ) return false ;

    bool preserve_white = ((TextPosElement *)parent_)->preserve_white_ ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "xml::space" ) {
            if ( val == "default" )
                preserve_white_ = false ;
            else if ( val == "preserve" )
                preserve_white_ = true ;
        }
    }

    XML_FOREACH_CHILD_ELEMENT(pNode, node)
    {
        if ( node.type() == pugi::node_pcdata )
        {
            text_ = node.value() ;
            if ( text_.empty() ) return false ;
        }
        else if ( node.type() == pugi::node_element &&
                  node.name() == "tspan" )
        {
            SpanElement *elem = new SpanElement() ;

            if ( elem->fromXml(node) )
            {
                children_.push_back(ElementPtr(elem)) ;
                elem->parent_ = this ;
            }
            else delete elem ;
        }
    }

    return true ;
}

bool Line::fromXml(const pugi::xml_node &pNode)
{
    x1_ = y1_ = x2_  = y2_ = Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x1" ) {
            if ( !x1_.fromString(val) ) return false ;
        }
        else if ( key == "y1" ) {
            if ( !y1_.fromString(val) ) return false ;
        }
        else if ( key == "x2" ) {
            if ( !x2_.fromString(val) ) return false ;
        }
        else if ( key == "y2" ) {
            if ( !y2_.fromString(val) ) return false ;
        }
    }

    return true ;
}


bool PolyLine::fromXml(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "points" )
            if ( !points_.fromString(val) ) return false ;
    }

    return true ;
}

bool Polygon::fromXml(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "points" )
            if ( !points_.fromString(val) ) return false ;
    }

    return true ;
}

bool Circle::fromXml(const pugi::xml_node &pNode)
{
    cx_ = cy_ = r_  = Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "cx" ) {
            if ( !cx_.fromString(val) ) return false ;
        }
        else if ( key == "cy" ) {
            if ( !cy_.fromString(val) ) return false ;
        }
        else if ( key == "r" ) {
            if ( !r_.fromString(val) ) return false ;
        }
    }

    return true ;
}

bool Ellipse::fromXml(const pugi::xml_node &pNode)
{
    cx_ = cy_ = rx_ = ry_  = Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "cx" ) {
            if ( !cx_.fromString(val) ) return false ;
        }
        else if ( key == "cy" ) {
            if ( !cy_.fromString(val) ) return false ;
        }
        else if ( key == "rx" ) {
            if ( !rx_.fromString(val) ) return false ;
        }
        else if ( key == "ry" ) {
            if ( !ry_.fromString(val) ) return false ;
        }
    }

    return true ;
}

bool Rect::fromXml(const pugi::xml_node &pNode)
{
    x_ = y_ = width_ = height_ = rx_ = ry_  =
        Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x" ) {
            if ( !x_.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y_.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width_.fromString(val) ) return false ;
        }
        else if ( key == "height" ) {
            if ( !height_.fromString(val) ) return false ;
        }
        else if ( key == "rx" ) {
            if ( !rx_.fromString(val) ) return false ;
        }
        else if ( key == "ry" ) {
            if ( !ry_.fromString(val) ) return false ;
        }
    }

    return true ;
}


bool Path::fromXml(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "d" )
            if ( !data_.fromString(val) ) return false ;
    }

    return true ;
}

bool Group::fromXml(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;

  //  string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;
    string nsPrefixSVG ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        ElementPtr pElem ;

        if ( qNode.name() == nsPrefixSVG + "g" )
            pElem.reset(new Group) ;
        else if ( qNode.name() == nsPrefixSVG + "svg" )
            pElem.reset(new Document) ;
        else if ( qNode.name() == nsPrefixSVG + "symbol" )
            pElem.reset(new Symbol) ;
        else if ( qNode.name() == nsPrefixSVG + "rect" )
            pElem.reset(new Rect) ;
        else if ( qNode.name() == nsPrefixSVG + "line" )
            pElem.reset(new Line) ;
        else if ( qNode.name() == nsPrefixSVG + "path" )
            pElem.reset(new Path) ;
        else if ( qNode.name() == nsPrefixSVG + "ellipse" )
            pElem.reset(new Ellipse) ;
        else if ( qNode.name() == nsPrefixSVG + "polygon" )
            pElem.reset(new Polygon) ;
        else if ( qNode.name() == nsPrefixSVG + "polyline" )
            pElem.reset(new PolyLine) ;
        else if ( qNode.name() == nsPrefixSVG + "circle" )
            pElem.reset(new Circle) ;
        else if ( qNode.name() == nsPrefixSVG + "text" )
            pElem.reset(new Text) ;
        else if ( qNode.name() == nsPrefixSVG + "defs" )
            pElem.reset(new Defs) ;
        else if ( qNode.name() == nsPrefixSVG + "linearGradient" )
            pElem.reset(new LinearGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "radialGradient" )
            pElem.reset(new RadialGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "use" )
            pElem.reset(new Use) ;
        else if ( qNode.name() == nsPrefixSVG + "image" )
            pElem.reset(new Image) ;
        else if ( qNode.name() == nsPrefixSVG + "pattern" )
            pElem.reset(new Pattern);
        else if ( qNode.name() == nsPrefixSVG + "clipPath" )
            pElem.reset(new ClipPath) ;
        else if ( qNode.name() == nsPrefixSVG + "style" )
            pElem.reset(new StyleDefinition) ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children_.push_back(pElem) ;
        pElem->parent_ = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node
    }

    return true ;
}


bool Defs::fromXml(const pugi::xml_node &pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;

 //   string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;
    string nsPrefixSVG ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        ElementPtr pElem ;

        if ( qNode.name() == nsPrefixSVG + "g" )
            pElem.reset(new Group) ;
        else if ( qNode.name() == nsPrefixSVG + "svg" )
            pElem.reset(new Document) ;
        else if ( qNode.name() == nsPrefixSVG + "symbol" )
            pElem.reset(new Symbol) ;
        else if ( qNode.name() == nsPrefixSVG + "rect" )
            pElem.reset(new Rect) ;
        else if ( qNode.name() == nsPrefixSVG + "line" )
            pElem.reset(new Line) ;
        else if ( qNode.name() == nsPrefixSVG + "path" )
            pElem.reset(new Path) ;
        else if ( qNode.name() == nsPrefixSVG + "ellipse" )
            pElem.reset(new Ellipse) ;
        else if ( qNode.name() == nsPrefixSVG + "polygon" )
            pElem.reset(new Polygon) ;
        else if ( qNode.name() == nsPrefixSVG + "polyline" )
            pElem.reset(new PolyLine) ;
        else if ( qNode.name() == nsPrefixSVG + "circle" )
            pElem.reset(new Circle) ;
        else if ( qNode.name() == nsPrefixSVG + "text" )
            pElem.reset(new Text) ;
        else if ( qNode.name() == nsPrefixSVG + "linearGradient" )
            pElem.reset(new LinearGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "radialGradient" )
            pElem.reset(new RadialGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "use" )
            pElem.reset(new Use) ;
        else if ( qNode.name() == nsPrefixSVG + "image" )
            pElem.reset(new Image) ;
        else if ( qNode.name() == nsPrefixSVG + "pattern" )
            pElem.reset(new Pattern) ;
        else if ( qNode.name() == nsPrefixSVG + "clipPath" )
            pElem.reset(new ClipPath) ;
        else if ( qNode.name() == nsPrefixSVG + "style" )
            pElem.reset(new StyleDefinition) ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children_.push_back(pElem) ;
        pElem->parent_ = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node
    }

    return true ;
}

bool GradientElement::parseStops(const pugi::xml_node &pNode)
{
   // string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;
     string nsPrefixSVG  ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        ElementPtr pElem  ;

        if ( qNode.name() == nsPrefixSVG + "stop" )
        {
            Stop stop ;

            XML_FOREACH_ATTRIBUTE(qNode, attr)
            {
                string key = attr.name(), val = attr.as_string() ;

                if ( key == "offset" ) {
                     if ( !stop.offset_.fromString(val) ) return false ;
                }
                else if ( key == "stop-color" )
                {
                    unsigned int clr ;
                    if ( parse_css_color(val, clr) ) stop.stop_color_ = clr ;
                    else return false ;
                 }
                 else if ( key == "stop-opacity" )
                    stop.stop_opacity_ =  Style::parseOpacity(val.c_str()) ;
                 else if ( key == "style")
                 {
                     Style st ;
                     if ( !st.fromStyleString(val.c_str()) ) return false ;

                     for( int i=0 ; i<st.flags_.size() ; i++ )
                     {
                         if ( st.flags_[i] == Style::StopColorState )
                             stop.stop_color_ = st.stop_color_ ;
                         else if ( st.flags_[i] == Style::StopOpacityState )
                             stop.stop_opacity_ = st.stop_opacity_ ;
                     }
                 }
             }
             stops_.push_back(stop) ;
        }
    }

    return true ;
}

bool LinearGradient::fromXml(const pugi::xml_node &pNode)
{
    if ( !GradientElement::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "x1" ) {
            if( !x1_.fromString(val) ) return false ;
        }
        else if ( key == "x2" ) {
            if ( !x2_.fromString(val) ) return false ;
        }
        else if ( key == "y1" ) {
            if ( !y1_.fromString(val) ) return false ;
        }
        else if ( key == "y2" ) {
            if ( !y2_.fromString(val) ) return false  ;
        }
    }

    if ( !GradientElement::parseStops(pNode) ) return false ;

    return true ;
}

bool RadialGradient::fromXml(const pugi::xml_node &pNode)
{
    if ( !GradientElement::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        string key = attr.name(), val = attr.as_string() ;

        if ( key == "cx" ) {
            if ( !cx_.fromString(val) ) return false ;
        }
        else if ( key == "cy" ) {
            if ( !cy_.fromString(val) ) return false ;
        }
        else if ( key == "fx" ) {
            if ( !fx_.fromString(val) ) return false ;
        }
        else if ( key == "fy" ) {
            if ( !fy_.fromString(val) ) return false ;
        }
        else if ( key == "r" ) {
            if ( !r_.fromString(val) ) return false ;
        }
    }

    if ( !GradientElement::parseStops(pNode) ) return false ;

    return true ;
}


bool Pattern::fromXml(const pugi::xml_node &pNode)
{
    if ( !parseAttributes(pNode) ) return false ;

    //string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;
    string nsPrefixSVG ;


    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        ElementPtr pElem ;

        if ( qNode.name() == nsPrefixSVG + "g" )
            pElem.reset(new Group) ;
        else if ( qNode.name() == nsPrefixSVG + "svg" )
            pElem.reset(new Document) ;
        else if ( qNode.name() == nsPrefixSVG + "symbol" )
            pElem.reset(new Symbol) ;
        else if ( qNode.name() == nsPrefixSVG + "rect" )
            pElem.reset(new Rect) ;
        else if ( qNode.name() == nsPrefixSVG + "line" )
            pElem.reset(new Line) ;
        else if ( qNode.name() == nsPrefixSVG + "path" )
            pElem.reset(new Path) ;
        else if ( qNode.name() == nsPrefixSVG + "ellipse" )
            pElem.reset(new Ellipse) ;
        else if ( qNode.name() == nsPrefixSVG + "polygon" )
            pElem.reset(new Polygon) ;
        else if ( qNode.name() == nsPrefixSVG + "polyline" )
            pElem.reset(new PolyLine) ;
        else if ( qNode.name() == nsPrefixSVG + "circle" )
            pElem.reset(new Circle) ;
        else if ( qNode.name() == nsPrefixSVG + "text" )
            pElem.reset(new Text) ;
        else if ( qNode.name() == nsPrefixSVG + "linearGradient" )
            pElem.reset(new LinearGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "radialGradient" )
            pElem.reset(new RadialGradient) ;
        else if ( qNode.name() == nsPrefixSVG + "use" )
            pElem.reset(new Use) ;
        else if ( qNode.name() == nsPrefixSVG + "image" )
            pElem.reset(new Image) ;
        else if ( qNode.name() == nsPrefixSVG + "pattern" )
            pElem.reset(new Pattern) ;
        else if ( qNode.name() == nsPrefixSVG + "clipPath" )
            pElem.reset(new ClipPath) ;
        else if ( qNode.name() == nsPrefixSVG + "style" )
            pElem.reset(new StyleDefinition) ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        if ( pElem->fromXml(qNode) )
        {
            children_.push_back(pElem) ;
            pElem->parent_ = this ;
        }

    }

    return true ;
}

bool Style::parseNameValue(const std::string &name, const std::string &value)
{
    string val = boost::trim_copy(value) ;

    if ( name == "fill-rule" )
    {
        if ( val == "nonzero" ) {
            fill_rule_ = NonZeroFillRule ;
            flags_.push_back(FillRuleState) ;
        }
        else if ( val == "evenodd" ) {
            fill_rule_ = EvenOddFillRule ;
            flags_.push_back(FillRuleState) ;
        }
        else if ( val == "inherit" ) ;
    }
    else if ( name == "fill-opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            fill_opacity_ = parseOpacity(val) ;
            flags_.push_back(FillOpacityState) ;
        }
    }
    else if ( name == "opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            opacity_ = parseOpacity(val) ;
           flags_.push_back(OpacityState) ;
        }
    }
    else if ( name == "stoke-opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            stroke_opacity_ = parseOpacity(val) ;
            flags_.push_back(StrokeOpacityState) ;
        }
    }
    else if ( name == "clip-rule" ) flags_.push_back(ClipRuleState) ;
    else if ( name == "fill")
    {
        parsePaint(val, true) ;
        flags_.push_back(FillState) ;
    }
    else if ( name == "stroke" )
    {
        parsePaint(val, false) ;
        flags_.push_back(StrokeState) ;
    }
    else if ( name == "stroke-width" )
    {
        if ( val == "inherit" ) ;
        else {
          flags_.push_back(StrokeWidthState) ;
          stroke_width_.fromString(val) ;
        }
    }
    else if ( name == "stroke-miterlimit" )
    {
        if ( val == "inherit" ) ;
        else
        {
            double dval ;
            if ( !parseFloatingPoint(val, dval) ) return false ;

            miter_limit_ = dval ;
            flags_.push_back(StrokeMiterLimitState) ;
        }
    }
    else if ( name == "stroke-dasharray" )
    {
      if ( val == "none" )
      {
          solid_stroke_ = true ;
          flags_.push_back(StrokeDashArrayState) ;
      }
      else if ( val == "inherit" ) ;
      else
      {
          if ( !Length::parseList(val, dash_array_) ) return false ;

            solid_stroke_ = false ;
            flags_.push_back(StrokeDashArrayState) ;
        }
    }
    else if ( name == "stroke-dashoffset" )
    {
        if ( val == "inherit" );
        else {
            flags_.push_back(StrokeDashOffsetState) ;
            if ( !dash_offset_.fromString(val) ) return false;
        }
    }
    else if ( name == "stroke-linejoin" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "miter" )
        {
            flags_.push_back(StrokeLineJoinState) ;
            line_join_ = MiterLineJoin ;
        }
        else if ( val == "round" )
        {
            flags_.push_back(StrokeLineJoinState) ;
            line_join_ = RoundLineJoin ;
        }
        else if ( val == "bevel" )
        {
            flags_.push_back(StrokeLineJoinState) ;
            line_join_ = BevelLineJoin ;
        }
    }
    else if ( name == "stroke-linecap" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "butt" )
        {
            flags_.push_back(StrokeLineCapState) ;
            line_cap_ = ButtLineCap ;
        }
        else if ( val == "round" )
        {
            flags_.push_back(StrokeLineCapState) ;
            line_cap_ = RoundLineCap ;
        }
        else if ( val == "square" )
        {
            flags_.push_back(StrokeLineCapState) ;
            line_cap_ = SquareLineCap ;
        }
    }
    else if ( name == "font-family" )
    {
        if ( val.empty() ) ;
        else if ( val == "inherit" ) ;
        else {
            font_family_ = val ;
            flags_.push_back(FontFamilyState) ;
        }
    }
    else if ( name == "font-style" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "normal" )
        {
            font_style_ = NormalFontStyle ;
            flags_.push_back(FontStyleState) ;
        }
        else if ( val == "oblique" )
        {
            font_style_ = ObliqueFontStyle ;
            flags_.push_back(FontStyleState) ;
        }
        else if ( val == "italic" )
        {
            font_style_ = ItalicFontStyle ;
            flags_.push_back(FontStyleState) ;
        }
    }
    else if ( name == "font-variant" )
    {
        if ( val == "inherit" );
        else if ( val == "normal" )
        {
            font_variant_ = NormalFontVariant ;
            flags_.push_back(FontVariantState) ;
        }
        else if ( val == "small-caps" )
        {
            font_variant_ = SmallCapsFontVariant ;
            flags_.push_back(FontVariantState) ;
        }
    }
    else if ( name == "font-weight" )
    {
        if ( val == "normal" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = NormalFontWeight ;
        }
        else if ( val == "bold" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = BoldFontWeight ;
        }
        else if ( val == "bolder" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = BolderFontWeight ;
        }
        else if ( val == "lighter" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = LighterFontWeight ;
        }
        else if ( val == "100" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W100FontWeight ;
        }
        else if ( val == "200" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W200FontWeight ;
        }
        else if ( val == "300" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W300FontWeight ;
        }
        else if ( val == "400" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W400FontWeight ;
        }
        else if ( val == "500" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W500FontWeight ;
        }
        else if ( val == "600" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W600FontWeight ;
        }
        else if ( val == "700" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W700FontWeight ;
        }
        else if ( val == "800" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W800FontWeight ;
        }
        else if ( val == "900" )
        {
            flags_.push_back(FontWeightState) ;
            font_weight_ = W900FontWeight ;
        }
        else if ( val == "inherit" ) ;
    }
    else if ( name == "font-stretch" )
    {
        if ( val == "ultra-condensed" )
        {
            font_stretch_ = UltraCondensedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "extra-condensed" )
        {
            font_stretch_ = ExtraCondensedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "condensed" )
        {
            font_stretch_ = CondensedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "narrower" )
        {
            font_stretch_ = NarrowerFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "semi-condensed" )
        {
            font_stretch_ = SemiCondensedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "semi-expanded" )
        {
            font_stretch_ = SemiExpandedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "expanded" )
        {
            font_stretch_ = ExpandedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "wider" )
        {
            font_stretch_ = WiderFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "extra-expanded" )
        {
            font_stretch_ = ExtraExpandedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "ultra-expanded" )
        {
            font_stretch_ = UltraExpandedFontStretch ;
            flags_.push_back(FontStretchState) ;
        }
        else if ( val == "inherit" ) ;
    }
    else if ( name == "font-size" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "xx-small" )
        {
            font_size_ = Length(Length::PTLengthType,  6.94) ;
            flags_.push_back(FontSizeState) ;
        }
        else if ( val == "x-small" )
        {
            font_size_ = Length(Length::PTLengthType,  8.33) ;
            flags_.push_back(FontSizeState) ;
        }
        else if ( val == "small" )
        {
            font_size_ = Length(Length::PTLengthType,  10) ;
            flags_.push_back(FontSizeState) ;
        }
        else if ( val == "medium" )
        {
            font_size_ = Length(Length::PTLengthType,  12) ;
            flags_.push_back(FontSizeState) ;
        }
        else if ( val == "large" )
        {
            font_size_ = Length(Length::PTLengthType,  14.4) ;
            flags_.push_back(FontSizeState) ;
        }
        else if ( val == "x-large" )
        {
            font_size_ = Length(Length::PTLengthType,  17.28) ;
            flags_.push_back(FontSizeState) ;
        }
        else if ( val == "xx-large" )
        {
            font_size_ = Length(Length::PTLengthType,  20.73) ;
            flags_.push_back(FontSizeState) ;
        }
        else
        {
            if ( !font_size_.fromString(val) ) return false ;
            flags_.push_back(FontSizeState) ;
        }
    }
    else if ( name == "text-decoration" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "none" ) ;
        else if ( val == "underline" )
        {
            flags_.push_back(Style::TextDecorationState) ;
            text_decoration_ = UnderlineDecoration ;
        }
        else if ( val == "overline" )
        {
            flags_.push_back(Style::TextDecorationState) ;
            text_decoration_ = OverlineDecoration ;
        }
        else if ( val == "strike" )
        {
            flags_.push_back(Style::TextDecorationState) ;
            text_decoration_ = StrikeDecoration ;
        }
        else if ( val == "line-through" )
        {
            flags_.push_back(Style::TextDecorationState) ;
            text_decoration_ = OverlineDecoration ;
        }
    }
    else if ( name == "text-anchor" )
    {
        if ( val == "inherit" );
        else if ( val == "start" )
        {
            flags_.push_back(Style::TextAnchorState) ;
            text_anchor_ = StartTextAnchor ;

        }
        else if ( val == "middle" )
        {
            flags_.push_back(Style::TextAnchorState) ;
            text_anchor_ = MiddleTextAnchor ;
        }
        else if ( val == "end" )
        {
            flags_.push_back(Style::TextAnchorState) ;
            text_anchor_ = EndTextAnchor ;
        }
    }
    else if ( name == "display" )
    {
        if ( val == "none" ) {
            display_mode_ = NoDisplay ;
            flags_.push_back(Style::DisplayState) ;
        }
        else if ( val == "inline" ) 	{
            display_mode_ = InlineDisplay ;
            flags_.push_back(Style::DisplayState) ;
        }

    }
    else if ( name == "visibility" )
    {
        if ( val == "none" ) {
            visibility_mode_ = NoVisibility ;
            flags_.push_back(Style::VisibilityState) ;
        }
        else if ( val == "hidden" ||  val == "collapsed" ) {
            visibility_mode_ = HiddenVisibility ;
            flags_.push_back(Style::VisibilityState) ;
        }
    }
    else if ( name == "shape-rendering" )
    {
        if ( val == "auto" || val == "default" )
        {
            shape_rendering_quality_ = AutoShapeQuality ;
            flags_.push_back(Style::ShapeRenderingState) ;
        }
        else if ( val == "optimizeSpeed" )
        {
            shape_rendering_quality_ = OptimizeSpeedShapeQuality ;
            flags_.push_back(Style::ShapeRenderingState) ;
        }
        else if ( val == "crispEdges" )
        {
            shape_rendering_quality_ = CrispEdgesShapeQuality ;
            flags_.push_back(Style::ShapeRenderingState) ;
        }
        else if ( val == "geometricPrecision" )
        {
            shape_rendering_quality_ = GeometricPrecisionShapeQuality ;
            flags_.push_back(Style::ShapeRenderingState) ;
        }
    }
    else if ( name == "text-rendering" )
    {
        if ( val == "auto" || val == "default" )
        {
            text_rendering_quality_ = AutoTextQuality ;
            flags_.push_back(Style::TextRenderingState) ;
        }
        else if ( val == "optimizeSpeed" )
        {
            text_rendering_quality_ = OptimizeSpeedTextQuality ;
            flags_.push_back(Style::TextRenderingState) ;
        }
        else if ( val == "optimizeLegibility" )
        {
            text_rendering_quality_ = OptimizeLegibilityTextQuality ;
            flags_.push_back(Style::TextRenderingState) ;
        }
        else if ( val == "geometricPrecision" )
        {
            text_rendering_quality_ = GeometricPrecisionTextQuality ;
            flags_.push_back(Style::TextRenderingState) ;
        }
    }
    else if ( name == "stop-opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            stop_opacity_ = parseOpacity(val) ;
            flags_.push_back(StopOpacityState) ;
        }
    }
    else if ( name == "stop-color" )
    {
        if ( val == "inherit" ) ;
        else {
            unsigned int clr ;
            if ( parse_css_color(val, clr) )
            {
                stop_color_ = clr ;
                flags_.push_back(StopColorState) ;
            }
        }
    }
    else if ( name == "overflow_" )
    {
        if ( val == "visible"  || val == "auto" )
        {
            overflow_ = true ;
            flags_.push_back(OverflowState) ;
        }
        else if ( val == "hidden"  || val == "scroll" )
        {
            overflow_ = false ;
            flags_.push_back(OverflowState) ;
        }
    }
    else if ( name == "clip-path" )
    {
        if ( val == "none" )
            flags_.push_back(OverflowState) ;
        else
        {
            clip_path_id_ = parseUri(val) ;
            flags_.push_back(ClipPathState) ;
        }
    }

    return true ;
}

bool Style::fromStyleString(const std::string &str)
{

    boost::sregex_iterator it(str.begin(), str.end(), boost::regex("([a-zA-Z-]+)[\\s]*:[\\s]*([^:;]+)[\\s]*[;]?")) ;
    boost::sregex_iterator end ;

    while ( it != end )  {
        if ( !parseNameValue(it->str(1), it->str(2)) ) return false ;
        ++it ;
    }

    return true ;
}

Transform Transform::inverse(const Transform &src)
{
    Transform dst ;

    double r_det;

    r_det = 1.0 / (src.m_[0] * src.m_[3] - src.m_[1] * src.m_[2]);
    dst.m_[0] = src.m_[3] * r_det;
    dst.m_[1] = -src.m_[1] * r_det;
    dst.m_[2] = -src.m_[2] * r_det;
    dst.m_[3] = src.m_[0] * r_det;
    dst.m_[4] = -src.m_[4] * dst.m_[0] - src.m_[5] * dst.m_[2];
    dst.m_[5] = -src.m_[4] * dst.m_[1] - src.m_[5] * dst.m_[3];

    return dst ;
}

Transform Transform::flip(const Transform &src, bool horz, bool vert)
{
    Transform dst ;

    dst.m_[0] = horz ? -src.m_[0] : src.m_[0];
    dst.m_[1] = horz ? -src.m_[1] : src.m_[1];
    dst.m_[2] = vert ? -src.m_[2] : src.m_[2];
    dst.m_[3] = vert ? -src.m_[3] : src.m_[3];
    dst.m_[4] = horz ? -src.m_[4] : src.m_[4];
    dst.m_[5] = vert ? -src.m_[5] : src.m_[5];

    return dst ;
}

#define EPSILON 1e-6

Transform Transform::multiply(const Transform &src1, const Transform &src2)
{
    Transform dst ;

    double d0, d1, d2, d3, d4, d5;

    d0 = src1.m_[0] * src2.m_[0] + src1.m_[1] * src2.m_[2];
    d1 = src1.m_[0] * src2.m_[1] + src1.m_[1] * src2.m_[3];
    d2 = src1.m_[2] * src2.m_[0] + src1.m_[3] * src2.m_[2];
    d3 = src1.m_[2] * src2.m_[1] + src1.m_[3] * src2.m_[3];
    d4 = src1.m_[4] * src2.m_[0] + src1.m_[5] * src2.m_[2] + src2.m_[4];
    d5 = src1.m_[4] * src2.m_[1] + src1.m_[5] * src2.m_[3] + src2.m_[5];
    dst.m_[0] = d0;
    dst.m_[1] = d1;
    dst.m_[2] = d2;
    dst.m_[3] = d3;
    dst.m_[4] = d4;
    dst.m_[5] = d5;

    return dst ;
}

Transform Transform::identity()
{
    Transform dst ;

    dst.m_[0] = 1;  dst.m_[1] = 0;  dst.m_[2] = 0;
    dst.m_[3] = 1;  dst.m_[4] = 0;  dst.m_[5] = 0;

    return dst ;
}

Transform Transform::scaling(double sx, double sy)
{
    Transform dst ;

    dst.m_[0] = sx;  dst.m_[1] = 0;  dst.m_[2] = 0;
    dst.m_[3] = sy;  dst.m_[4] = 0;  dst.m_[5] = 0;

    return dst ;
}

Transform Transform::rotation(double theta)
{
    Transform dst ;

    double s, c;

    s = sin (theta * M_PI / 180.0);
    c = cos (theta * M_PI / 180.0);

    dst.m_[0] = c;    dst.m_[1] = s;    dst.m_[2] = -s;
    dst.m_[3] = c;    dst.m_[4] = 0;    dst.m_[5] = 0;

    return dst ;
}

Transform Transform::shearing(double theta)
{
    Transform dst ;
    double t;

    t = tan (theta * M_PI / 180.0);
    dst.m_[0] = 1;    dst.m_[1] = 0;    dst.m_[2] = t;
    dst.m_[3] = 1;    dst.m_[4] = 0;    dst.m_[5] = 0;

    return dst ;
}

Transform Transform::translation(double tx, double ty)
{
    Transform dst ;

    dst.m_[0] = 1;  dst.m_[1] = 0;  dst.m_[2] = 0;
    dst.m_[3] = 1;  dst.m_[4] = tx;  dst.m_[5] = ty;

    return dst ;
}

double Transform::expansion(const Transform &src)
{
    return sqrt (fabs (src.m_[0] * src.m_[3] - src.m_[1] * src.m_[2]));
}

bool Transform::isRectilinear(const Transform &src)
{
    return ((fabs (src.m_[1]) < EPSILON && fabs (src.m_[2]) < EPSILON) ||
            (fabs (src.m_[0]) < EPSILON && fabs (src.m_[3]) < EPSILON));
}

bool Transform::isEqual(const Transform &matrix1, const Transform &matrix2)
{
    return (fabs (matrix1.m_[0] - matrix2.m_[0]) < EPSILON &&
            fabs (matrix1.m_[1] - matrix2.m_[1]) < EPSILON &&
            fabs (matrix1.m_[2] - matrix2.m_[2]) < EPSILON &&
            fabs (matrix1.m_[3] - matrix2.m_[3]) < EPSILON &&
            fabs (matrix1.m_[4] - matrix2.m_[4]) < EPSILON &&
                        fabs (matrix1.m_[5] - matrix2.m_[5]) < EPSILON);
}

Transform::Transform()
{
    m_[0] = 1;  m_[1] = 0;  m_[2] = 0;
    m_[3] = 1;  m_[4] = 0;  m_[5] = 0;
}

Transform::Transform(double s0, double s1, double s2, double s3, double s4, double s5)
{
    m_[0] = s0 ; m_[1] = s1 ; m_[2] = s2 ;
    m_[3] = s3 ; m_[4] = s4 ; m_[5] = s5 ;
}

bool Transform::fromString(const std::string &str)
{
    Transform &trs = *this ;

    const char *p = str.c_str() ;
    eatwhite(p) ;

    while ( *p )
    {
        if ( strncmp(p, "matrix", 6) == 0 )
        {
            vector<double> nums ;
            p += 6 ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p,  ')', nums) ;

            if ( nums.size() >= 6 )
            {
                Transform d(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5]) ;
                trs = Transform::multiply(d, trs) ;
            }

        }
        else if ( strncmp(p, "translate", 9) == 0 )
        {
            p += 9 ;
            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p, ')', nums) ;

            if ( nums.size() >= 2 )
            {
                Transform d = Transform::translation(nums[0], nums[1]) ;
                trs = Transform::multiply(d, trs) ;
            }
            else if ( nums.size() == 1 )
            {
                Transform d = Transform::translation(nums[0], 0) ;
                trs = Transform::multiply(d, trs) ;
            }

        }
        else if ( strncmp(p, "rotate", 6) == 0 )
        {
            p += 6 ;
            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p,  ')', nums) ;

            if ( nums.size() == 1 )
            {
                Transform d = Transform::rotation(nums[0]) ;
                trs = Transform::multiply(d, trs) ;
            }
            else if ( nums.size() == 3 )
            {
                //?
                Transform tc =  Transform::translation(-nums[1], -nums[2]) ;
                Transform r =   Transform::rotation(nums[0]) ;
                Transform tmc = Transform::translation(nums[1], nums[2]) ;
                trs = Transform::multiply(trs, Transform::multiply(tc, Transform::multiply(r, tmc))) ;
            }

        }
        else if ( strncmp(p, "scale", 5) == 0 )
        {
            p += 5 ;

            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p,  ')', nums) ;

            if ( nums.size() == 1 )
            {
                Transform d = Transform::scaling(nums[0], nums[0]) ;
                trs = Transform::multiply(d, trs) ;
            }
            else if ( nums.size() >= 2 )
            {
                Transform d = Transform::scaling(nums[0], nums[1]) ;
                trs = Transform::multiply(d, trs) ;
            }
        }
        else if ( strncmp(p, "skewX", 5) == 0 )
        {
            p += 5 ;

            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p, ')', nums) ;

            if ( nums.size() >= 1 )
            {
                Transform d = Transform::shearing(nums[0]) ;
                trs = Transform::multiply(d, trs) ;
            }

        }
        else if ( strncmp(p, "skewY", 5) == 0 )
        {
            p += 5 ;

            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p, ')', nums) ;

            if ( nums.size() >= 1 )
            {
                Transform d = Transform::shearing(nums[0]) ;
                d.m_[1] = d.m_[2] ;
                d.m_[2] = 0 ;
                trs = Transform::multiply(d, trs) ;
            }
        }

        eatwhite_comma(p) ;
    }

    return true ;

}


bool Length::fromString(const std::string &str, bool scale)
{
    static boost::regex rx("(?:([+-]?[0-9]+)|([+-]?[0-9]*\\.[0-9]+(?:[Ee][+-]?[0-9]+)?))(em|ex|px|in|cm|mm|pt|pc|\\%)?", boost::regex::icase) ;

    boost::smatch what ;
    float length ;

    if ( boost::regex_match(str, what, rx) )
    {
        double val ;

        string num = ( what.str(1).empty() ) ? what[2] : what[1] ;

        if ( !parseFloatingPoint(num, val) )
            return false ;
        else {
            value_in_specified_units_ = val ;
            scale_to_viewport_ = scale ;

            string unit = what[3] ;

            if ( boost::iequals(unit, "em" ) )
                unit_type_ = EMSLengthType ;
            else if ( boost::iequals(unit, "ex") )
                unit_type_ = EXSLengthType;
            else if ( boost::iequals(unit, "px") )
                unit_type_ = PXLengthType;
            else if ( boost::iequals(unit, "in") )
                unit_type_ = INLengthType;
            else if ( boost::iequals(unit, "cm") )
                unit_type_ = CMLengthType;
            else if ( boost::iequals(unit, "mm") )
                unit_type_ = MMLengthType;
            else if ( boost::iequals(unit, "pt") )
                unit_type_ = PTLengthType;
            else if ( boost::iequals(unit, "pc") )
                unit_type_ = PCLengthType;
            else if ( unit == "%" ) {
                unit_type_ = PercentageLengthType ;
                value_in_specified_units_ /= 100.0 ;
            }
            else
                unit_type_ = NumberLengthType;
        }
    }
    else return false ;

    return true ;
}

bool Length::parseList(const string &str, std::vector<Length> &ls)
{
    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    boost::char_separator<char> sep(" ,");
    tokenizer tokens(str, sep);

    for (tokenizer::iterator tok_iter = tokens.begin();  tok_iter != tokens.end(); ++tok_iter)
    {
        Length l ;

        string token = *tok_iter  ;

        if ( !l.fromString(token) ) return false ;
        else ls.push_back(l) ;
    }

    return true ;
}

bool ViewBox::fromString(const std::string &str)
{
    return parse_coordinate_list(str, x, y, w, h) ;
}

bool PreserveAspectRatio::fromString(const std::string &str)
{
    boost::regex rx("(?:(defer)[\\s]+)?([a-zA-Z]+)(?:[\\s]+(meet|slice))?") ;

    view_box_align_ = NoViewBoxAlign ;
    view_box_policy_ = MeetViewBoxPolicy ;

    boost::smatch what ;

    if ( !boost::regex_match(str, what, rx) ) return false ;

    defer_aspect_ratio_ = false ;

    if ( what.str(1) == "defer" )
        defer_aspect_ratio_ = true ;

    string align = what.str(2) ;

    if ( align == "none" )
        view_box_align_ = NoViewBoxAlign ;
    else if ( align == "xMinYMin" )
        view_box_align_ = XMinYMin ;
    else if ( align == "xMidYMin" )
        view_box_align_ = XMidYMin ;
    else if ( align == "xMaxYMin" )
        view_box_align_ = XMaxYMin ;
    else if ( align == "xMinYMid" )
        view_box_align_ = XMinYMid ;
    else if ( align == "xMidYMid" )
        view_box_align_ = XMidYMid ;
    else if ( align == "xMaxYMid" )
        view_box_align_ = XMaxYMid ;
    else if ( align == "xMinYMax" )
        view_box_align_ = XMinYMax ;
    else if ( align == "xMidYMax" )
        view_box_align_ = XMidYMax ;
    else if ( align == "xMaxYMax" )
        view_box_align_ = XMaxYMax ;

    if ( what.str(3) == "meet" )
        view_box_policy_ = MeetViewBoxPolicy ;
    else if ( what.str(3) == "slice" )
        view_box_policy_ = SliceViewBoxPolicy ;

    return true ;
}


void PreserveAspectRatio::constrainViewBox(double width_, double height_, ViewBox &orig)
{
    double origx = orig.x ;
    double origy = orig.y ;
    double origw = orig.w ;
    double origh = orig.h ;

    double neww, newh;

    if ( view_box_policy_ == MeetViewBoxPolicy )
    {
        neww = width_ ;
        newh = height_ ;

        if ( height_ * origw > width_ * origh )
            newh = origh * width_ / origw ;
        else
            neww = origw * height_ / origh;
    }
    else
    {
        neww = width_ ;
        newh = height_ ;

        if ( height_ * origw < width_ * origh )
            newh = origh * width_ / origw ;
        else
            neww = origw * height_ / origh;
    }

    if ( view_box_align_ == XMinYMin  || view_box_align_ == XMinYMid  || view_box_align_ == XMinYMax  ) ;
    else if ( view_box_align_ == XMidYMin  ||	view_box_align_ == XMidYMid  || view_box_align_ == XMidYMax  )
        origx -= (neww - width_) / 2 ;
    else
        origx -= neww - width_ ;

    if ( view_box_align_ == XMinYMin || view_box_align_ == XMidYMin || view_box_align_ == XMaxYMin ) ;
    else if ( view_box_align_ == XMinYMid || view_box_align_ == XMidYMid || view_box_align_ == XMaxYMid )
        origy -= (newh - height_) / 2;
    else
        origy -= newh - height_ ;

    origw = neww ;
    origh = newh ;

    orig.x = origx ;
    orig.y = origy ;
    orig.w = origw ;
    orig.h = origh ;
}

Transform PreserveAspectRatio::getViewBoxTransform(double sw, double sh, double vwidth, double vheight, double vx, double vy)
{
    Transform trs ;

    if ( vwidth != 0.0 && vheight != 0.0 )
    {
        double vboxx = vx ;
        double vboxy = vy ;
        double vboxw = vwidth ;
        double vboxh = vheight ;

        double ofx = 0, ofy = 0 ;
        double aspScaleX = 1.0 ;
        double aspScaleY = 1.0 ;

        if ( view_box_align_ != NoViewBoxAlign )
        {
            ViewBox vbox ;

            vbox.x = vboxx ;
            vbox.y = vboxy ;
            vbox.w = vboxw ;
            vbox.h = vboxh ;

            constrainViewBox(sw, sh, vbox) ;

            ofx = vbox.x ;
            ofy = vbox.y ;


            aspScaleX = vbox.w/vwidth ;
            aspScaleY = vbox.h/vheight ;
        }
        else {
            aspScaleX = sw/vboxw ;
            aspScaleY = sh/vboxh ;
        }

        trs = Transform::translation(-vx, -vy) ;
        trs = Transform::multiply(trs, Transform::scaling(aspScaleX, aspScaleY)) ;
        trs = Transform::multiply(trs, Transform::translation(ofx, ofy)) ;
    }

    return trs ;
}


static void
svg_path_arc_segment (double ctx[6],
                       double xc, double yc,
                       double th0, double th1, double rx, double ry, double x_axis_rotation)
{
    double sin_th, cos_th;
    double a00, a01, a10, a11;
    double x1, y1, x2, y2, x3, y3;
    double t;
    double th_half;

    sin_th = sin (x_axis_rotation * (M_PI / 180.0));
    cos_th = cos (x_axis_rotation * (M_PI / 180.0));
    /* inverse transform compared with rsvg_path_arc */
    a00 = cos_th * rx;
    a01 = -sin_th * ry;
    a10 = sin_th * rx;
    a11 = cos_th * ry;

    th_half = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * sin (th_half * 0.5) * sin (th_half * 0.5) / sin (th_half);
    x1 = xc + cos (th0) - t * sin (th0);
    y1 = yc + sin (th0) + t * cos (th0);
    x3 = xc + cos (th1);
    y3 = yc + sin (th1);
    x2 = x3 + t * sin (th1);
    y2 = y3 - t * cos (th1);

        ctx[0] = a00 * x1 + a01 * y1 ;
        ctx[1] = a10 * x1 + a11 * y1 ;
        ctx[2] = a00 * x2 + a01 * y2 ;
        ctx[3] = a10 * x2 + a11 * y2 ;
        ctx[4] = a00 * x3 + a01 * y3 ;
        ctx[5] = a10 * x3 + a11 * y3;
}


int nPath = 0 ;
bool PathData::fromString(const std::string &str)
{
    Command previousCmd  ;

    bool insideGroup = false ;

    bool isFirst = true ;

    float arg1, arg2, arg3, arg4, arg5, arg6, arg7 ;
    float cx , cy  ;
    float rx, ry ;


    boost::sregex_iterator it(str.begin(), str.end(), boost::regex("(?:([mMsShHvVlLcCQqQtTaA]+)([^mMsShHvVlLcCqQtTaAzZ]+))|([zZ])[\\s]*")) ;
    boost::sregex_iterator end ;

    while ( it != end )
    {

        string args ;
        char cmd ;

        if ( it->str(3).empty() )
        {
            cmd = it->str(1).at(0) ;
            args = it->str(2) ;
        }
        else
            cmd = 'z' ;

        if ( cmd == 'M' || cmd == 'm' )
        {
            bool isRel = ( cmd == 'm') ;

            insideGroup = true ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            if ( !isRel || isFirst )
            {
                cx = argList[0] ;
                cy = argList[1] ;
            }
            else {
                cx += argList[0] ;
                cy += argList[1] ;
            }

            elements_.push_back(PathData::Element(MoveToCmd, cx, cy)) ;
            isFirst = false ;

            for(int i=2 ; i<argList.size() ; i += 2)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;

                if ( isRel )
                    elements_.push_back(PathData::Element(LineToCmd, cx = arg1 + cx, cy = arg2 + cy)) ;
                else
                    elements_.push_back(PathData::Element(LineToCmd, cx = arg1, cy = arg2)) ;
            }

            previousCmd = MoveToCmd ;
        }
        else if ( cmd == 'z' )
        {
            insideGroup = false ;

            elements_.push_back(PathData::Element(ClosePathCmd)) ;

            previousCmd = ClosePathCmd ;
        }
        else if ( cmd == 'l' || cmd == 'L' )
        {
            bool isRel = ( cmd == 'l' ) ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i += 2)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;

                if ( isRel )
                    elements_.push_back(PathData::Element(LineToCmd, cx = arg1 + cx, cy = arg2 + cy)) ;
                else
                    elements_.push_back(PathData::Element(LineToCmd, cx = arg1, cy = arg2)) ;
            }

            previousCmd = LineToCmd ;
        }
        else if ( cmd == 'h' || cmd == 'H' )
        {
            bool isRel = (cmd == 'h') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i += 2)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;

                if ( isRel )
                    elements_.push_back(PathData::Element(LineToCmd, cx = arg1 + cx, cy)) ;
                else
                    elements_.push_back(PathData::Element(LineToCmd, cx = arg1, cy)) ;
            }
            previousCmd = LineToCmd ;
        }
        else if ( cmd == 'v' || cmd == 'V' )
        {
            bool isRel = (cmd == 'v') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i++)
            {
                arg1 = argList[i] ;

                if ( isRel )
                    elements_.push_back(PathData::Element(LineToCmd, cx, cy = arg1 + cy)) ;
                else
                    elements_.push_back(PathData::Element(LineToCmd, cx, cy = arg1)) ;
            }
            previousCmd = LineToCmd ;
        }
        else if ( cmd == 'c' || cmd == 'C' )
        {
            bool isRel = (cmd == 'c') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=6)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;
                arg3 = argList[i+2] ;
                arg4 = argList[i+3] ;
                arg5 = argList[i+4] ;
                arg6 = argList[i+5] ;

                if ( isRel )
                {
                    elements_.push_back(PathData::Element(CurveToCmd,
                        cx + arg1, cy + arg2, rx = cx + arg3, ry = cy + arg4, arg5 + cx, arg6 + cy)) ;
                        cx += arg5 ;
                        cy += arg6 ;

                }
                else
                    elements_.push_back(PathData::Element(CurveToCmd,
                        arg1, arg2, rx = arg3, ry = arg4, cx = arg5, cy = arg6)) ;
            }
            previousCmd = CurveToCmd ;
        }
        else if ( cmd == 's' || cmd == 'S' )
        {
            bool isRel = (cmd == 's') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=4)
            {
                arg3 = argList[i] ;
                arg4 = argList[i+1] ;
                arg5 = argList[i+2] ;
                arg6 = argList[i+3] ;

                if ( elements_.empty() ) return false ;

                Element &preCmd = elements_.back() ;

                if ( previousCmd == CurveToCmd )
                {
                    arg1 = 2 * cx - rx ;
                    arg2 = 2 * cy - ry ;
                }
                else
                {
                    arg1 = cx ; arg2 = cy ;
                }
                if ( isRel ) { arg1 -= cx ; arg2 -= cy ; }

                if ( isRel )
                {
                    elements_.push_back(PathData::Element(CurveToCmd,
                        cx + arg1, cy + arg2, rx = cx + arg3, ry = cy + arg4, arg5 + cx, arg6 + cy)) ;
                        cx += arg5 ;
                        cy += arg6 ;
                }
                else
                    elements_.push_back(PathData::Element(CurveToCmd,
                        arg1, arg2, rx = arg3, ry = arg4, cx = arg5, cy = arg6)) ;
            }
            previousCmd = CurveToCmd ;
        }
        else if ( cmd == 'Q' || cmd == 'q' )
        {
            bool isRel = (cmd == 'q') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=4)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;
                arg3 = argList[i+2] ;
                arg4 = argList[i+3] ;

                if ( isRel )
                {
                    arg1 += cx ; arg3 += cx ;
                    arg2 += cy ; arg4 += cy ;
                }

                rx = arg1 ; ry = arg2 ;

                /* raise quadratic bezier to cubic */
                double x1 = (cx + 2 * arg1) * (1.0 / 3.0);
                double y1 = (cy + 2 * arg2) * (1.0 / 3.0);
                double x3 = arg3 ;
                double y3 = arg4 ;
                double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
                double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

                elements_.push_back(PathData::Element(CurveToCmd, x1, y1, x2, y2, x3, y3)) ;

                cx = arg3 ;
                cy = arg4 ;
            }
            previousCmd = QuadCurveToCmd ;
        }
        else if ( cmd == 'T' || cmd == 't' )
        {
            bool isRel = (cmd == 't') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=2)
            {
                arg3 = argList[i] ;
                arg4 = argList[i+1] ;

                if ( elements_.empty() ) return false ;

                Element &preCmd = elements_.back() ;

                if ( previousCmd == QuadCurveToCmd )
                {
                    arg1 = 2 * cx - rx ;
                    arg2 = 2 * cy - ry ;
                }
                else
                {
                    arg1 = cx ; arg2 = cy ;
                }

                if ( isRel )
                {
                    arg3 += cx ; arg4 += cy ;
                }

                /* raise quadratic bezier to cubic */
                double x1 = (cx + 2 * arg1) * (1.0 / 3.0);
                double y1 = (cy + 2 * arg2) * (1.0 / 3.0);
                double x3 = arg3 ;
                double y3 = arg4 ;
                double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
                double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

                elements_.push_back(PathData::Element(CurveToCmd, x1, y1, x2, y2, x3, y3)) ;

                cx = arg3 ;
                cy = arg4 ;
            }
            previousCmd = QuadCurveToCmd ;
        }
        else if ( cmd == 'A' || cmd == 'a' )
        {
            bool isRel = (cmd == 'a') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int k=0 ; k<argList.size() ; k+=7)
            {
                arg1 = argList[k] ;
                arg2 = argList[k+1] ;
                arg3 = argList[k+2] ;
                arg4 = argList[k+3] ;
                arg5 = argList[k+4] ;
                arg6 = argList[k+5] ;
                arg7 = argList[k+6] ;

                if ( isRel )
                {
                    arg1 += cx ; arg2 += cy ;
                    arg6 += cx ; arg7 += cy ;
                }

                /**
                * rsvg_path_arc: Add an RSVG arc to the path context.
                * @ctx: Path context.
                * @rx: Radius in x direction (before rotation).
                * @ry: Radius in y direction (before rotation).
                * @x_axis_rotation: Rotation angle for axes.
                * @large_arc_flag: 0 for arc length <= 180, 1 for arc >= 180.
                * @sweep: 0 for "negative angle", 1 for "positive angle".
                * @x: New x coordinate.
                * @y: New y coordinate.
                *
                **/

                double rx = arg1 ;
                double ry = arg2 ;
                double x_axis_rotation = arg3 ;
                int large_arc_flag = (arg4 == 0.0 ) ? 0 : 1 ;
                int sweep_flag = (arg5 == 0.0 ) ? 0 : 1 ;
                double x = arg6 ;
                double y = arg7 ;

                double sin_th, cos_th;
                double a00, a01, a10, a11;
                double x0, y0, x1, y1, xc, yc;
                double d, sfactor, sfactor_sq;
                double th0, th1, th_arc;
                int i, n_segs;

                /* Check that neither radius is zero, since its isn't either
                    geometrically or mathematically meaningful and will
                    cause divide by zero and subsequent NaNs.  We should
                    really do some ranged check ie -0.001 < x < 000.1 rather
                    can just a straight check again zero.
                */
                if ((rx == 0.0) || (ry == 0.0))
                    continue ;

                sin_th = sin (x_axis_rotation * (M_PI / 180.0));
                cos_th = cos (x_axis_rotation * (M_PI / 180.0));
                a00 = cos_th / rx;
                a01 = sin_th / rx;
                a10 = -sin_th / ry;
                a11 = cos_th / ry;
                x0 = a00 * cx + a01 * cy;
                y0 = a10 * cx + a11 * cy;
                x1 = a00 * x + a01 * y;
                y1 = a10 * x + a11 * y;
                /* (x0, y0) is current point in transformed coordinate space.
                   (x1, y1) is new point in transformed coordinate space.

                   The arc fits a unit-radius circle in this space.
                */
                d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
                sfactor_sq = 1.0 / d - 0.25;
                if (sfactor_sq < 0) sfactor_sq = 0;
                sfactor = sqrt (sfactor_sq);
                if (sweep_flag == large_arc_flag)  sfactor = -sfactor;
                xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
                yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
                /* (xc, yc) is center of the circle. */

                th0 = atan2 (y0 - yc, x0 - xc);
                th1 = atan2 (y1 - yc, x1 - xc);

                th_arc = th1 - th0;
                if (th_arc < 0 && sweep_flag) th_arc += 2 * M_PI;
                else if (th_arc > 0 && !sweep_flag) th_arc -= 2 * M_PI;

                n_segs = ceil (fabs (th_arc / (M_PI * 0.5 + 0.001)));

                for (i = 0; i < n_segs; i++)
                {
                    double ccc[6] ;
                    svg_path_arc_segment (ccc, xc, yc,
                               th0 + i * th_arc / n_segs,
                               th0 + (i + 1) * th_arc / n_segs, rx, ry, x_axis_rotation);

                    elements_.push_back(PathData::Element(CurveToCmd,
                        ccc[0], ccc[1], ccc[2], ccc[3], ccc[4], ccc[5])) ;
                }

                cx = x ;
                cy = y ;
            }
            previousCmd = EllipticArcToCmd ;
        }

        ++it ;
    }



    return true ;
}


bool PointList::fromString(const std::string &str)
{
    if ( !parse_coordinate_list(str, points_) ) return false ;
    if ( points_.size() %2 ) return false ;
    return true ;
}

/////////////////////////////////////////////////////////////////////////

unsigned int Style::parseOpacity(const std::string &str)
{
   char *end_ptr;
   double opacity;

   opacity = strtod (str.c_str(), &end_ptr);

   if (end_ptr && end_ptr[0] == '%')
        opacity *= 0.01;

   return (unsigned int) floor (opacity * 255. + 0.5);
}


Style::Style()
{
    fill_paint_type_ = SolidColorPaint ;
    fill_paint_.clr_ = 0 ;
    fill_opacity_ = 0xff ;

    fill_rule_ = NonZeroFillRule ;
    stroke_paint_type_ = NoPaint ;

    stroke_width_ = Length(Length::NumberLengthType, 1.0) ;
    line_cap_ = ButtLineCap ;
    line_join_ = MiterLineJoin ;
    miter_limit_ = 4 ;
    dash_offset_ = Length(Length::NumberLengthType, 0.0) ;
    stroke_opacity_ = 0xff ;
    opacity_ = 0xff ;
    solid_stroke_ = true ;
    text_rendering_quality_ = AutoTextQuality ;
    shape_rendering_quality_ = AutoShapeQuality ;
    overflow_ = false ;
 //   fontFamily = "Arial" ;
    font_size_ = Length(Length::PXLengthType, 10.0) ;
}

void Style::resetNonInheritable()
{

}


void Style::parsePaint(const std::string &val, bool fill)
{
    if ( val == "none" )
    {
        if ( fill )
            fill_paint_type_ = Style::NoPaint ;
        else
            stroke_paint_type_ = Style::NoPaint ;
    }
    else if ( val == "currentColor" )
    {
        if ( fill )
            fill_paint_type_ = Style::CurrentColorPaint ;
        else
        stroke_paint_type_ = Style::CurrentColorPaint ;
    }
    else if ( val == "inherit") ;
    else if ( boost::algorithm::starts_with(val, "url") )
    {
        string id = parseUri(val) ;

        if ( !id.empty() )
        {
            if ( fill )
            {
                fill_paint_.paint_server_id_ = strdup(id.c_str()) ;
                fill_paint_type_ = Style::PaintServerPaint ;
            }
            else
            {
                stroke_paint_.paint_server_id_ = strdup(id.c_str()) ;
                stroke_paint_type_ = Style::PaintServerPaint ;
            }
        }
    }
    else
    {
        unsigned int clr ;

        if ( parse_css_color(val, clr) )
        {
            if ( fill )
            {
                fill_paint_.clr_ = clr ;
                fill_paint_type_ = Style::SolidColorPaint ;
            }
            else
            {
                stroke_paint_.clr_ = clr ;
                stroke_paint_type_ = Style::SolidColorPaint ;
            }

        }
    }

}


bool Style::hasFlag(Flag f) const
{
    for( int i=0 ; i<flags_.size() ; i++ )
        if ( flags_[i] == f ) return true ;

    return false ;
}


bool DocumentInstance::load(istream &strm)
{
   pugi::xml_document xml ;

    if ( !xml.load(strm) ) return false ;

    pugi::xml_node pNode = xml.root().child("svg") ;

    if ( !pNode ) return false ;

    root_.reset(new Document()) ;
    return root_->fromXml(pNode) ;
}


}

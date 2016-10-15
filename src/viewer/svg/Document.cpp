#include "Document.h"
#include "Rendering.h"
#include "ParseUtil.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <cerrno>
#include <math.h>

using namespace std ;
namespace svg {

const char *NS_SVG = "http://www.w3.org/2000/svg" ;

#define SAFE_ASSIGN(x) if (!(x)) return false ;

bool Element::parseAttributes(const XmlElement *pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        if ( attr->key() == "id" )
            id = attr->value() ;
    }

    return true ;
}

bool Stylable::parseAttributes(const XmlElement *pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "style" ) {
           if ( !style.fromStyleString(val) ) return false;
        }
        else
           if ( !style.parseNameValue(key, val) ) return false ;
    }

    return true ;
}


bool GradientElement::parseAttributes(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    fallback = 0 ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "gradientUnits" )
        {
            if ( val == "userSpaceOnUse" )
                gradientUnits = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                gradientUnits = ObjectBoundingBox ;

            fallback |= GradientUnitsDefined ;
        }
        else if ( key == "gradientTransform" )
        {
            if ( !trans.fromString(val) ) return false ;

            fallback |= TransformDefined ;
        }
        else if ( key == "spreadMethod" )
        {
            if ( val == "pad" )
                spreadMethod = PadSpreadMethod ;
            else if ( val == "repeat" )
                spreadMethod = RepeatSpreadMethod ;
            else if ( val == "reflect" )
                spreadMethod = ReflectSpreadMethod ;

            fallback |= SpreadMethodDefined ;
        }
        else if ( key == "xlink:href" )
        {
            href = val ;
        }
    }

    return true ;
}


Container::~Container()
{
    for(int i=0 ; i<children.size() ; i++)
        delete children[i] ;
}

bool Pattern::parseAttributes(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !FitToViewBox::parseAttributes(pNode) ) return false ;

    fallback = 0 ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "patternUnits" )
        {
            if ( val == "userSpaceOnUse" )
                patternUnits = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                patternUnits = ObjectBoundingBox ;

            fallback |= PatternUnitsDefined ;
        }
        else if ( key == "patternContentUnits" )
        {
            if ( val == "userSpaceOnUse" )
                patternContentUnits = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                patternContentUnits = ObjectBoundingBox ;

            fallback |= PatternContentUnitsDefined ;
        }
        else if ( key == "patternTransform" )
        {
            if ( !trans.fromString(val) ) return false ;

            fallback |= TransformDefined ;
        }
        else if ( key  == "xlink:href" )
        {
           href = val ;
        }
        else if ( key == "x" ) {
            if ( !x.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width.fromString(val, true) ) return false ;
        }
        else if ( key == "height" ) {
            if (!height.fromString(val, true) ) return false ;
        }
    }

    return true ;
}


bool Transformable::parseAttributes(const XmlElement *pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "transform" )
            if ( !trans.fromString(val) ) return false ;
    }

    return true ;
}

bool FitToViewBox::parseAttributes(const XmlElement *pNode)
{
    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "viewBox" )  {
           if ( !viewBox.fromString(val) ) return false ;
        }
        else if ( key == "preserveAspectRatio" ) {
           if ( !preserveAspectRatio.fromString(val) ) return false ;
        }
    }

    return true ;
}

bool TextPosElement::parseAttributes(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x" ) {
            if ( !x.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y.fromString(val) ) return false ;
        }
        else if ( key == "dx" ) {
            if ( !dx.fromString(val) ) return false ;
        }
        else if ( key == "dy" ) {
            if ( !dy.fromString(val) ) return false ;
        }
        else if ( key == "xml::space" ) {
            if ( val == "default" )
                preserveWhite = false ;
            else if ( val == "preserve" )
                preserveWhite = true ;
        }
    }

    return true ;
}

bool Document::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !FitToViewBox::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x" ) ;
        else if ( key == "y" ) ;
        else if ( key == "width" ) {
           if ( !width.fromString(val, true) ) return false ;
        }
        else if ( key == "height" ) {
           if ( !height.fromString(val, true) ) return false ;
        }
    }

    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        Element *pElem = 0 ;

        if ( qNode->tag() == nsPrefixSVG + "g" )
            pElem = new Group ;
        else if ( qNode->tag() == nsPrefixSVG + "svg" )
            pElem = new Document ;
        else if ( qNode->tag() == nsPrefixSVG + "rect" )
            pElem = new Rect ;
        else if ( qNode->tag() == nsPrefixSVG + "line" )
            pElem = new Line ;
        else if ( qNode->tag() == nsPrefixSVG + "path" )
            pElem = new Path ;
        else if ( qNode->tag() == nsPrefixSVG + "ellipse" )
            pElem = new Ellipse ;
        else if ( qNode->tag() == nsPrefixSVG + "polygon" )
            pElem = new Polygon ;
        else if ( qNode->tag() == nsPrefixSVG + "polyline" )
            pElem = new PolyLine ;
        else if ( qNode->tag() == nsPrefixSVG + "circle" )
            pElem = new Circle ;
        else if ( qNode->tag() == nsPrefixSVG + "text" )
            pElem = new Text ;
        else if ( qNode->tag() == nsPrefixSVG + "defs" )
            pElem = new Defs ;
        else if ( qNode->tag() == nsPrefixSVG + "symbol" )
            pElem = new Symbol ;
        else if ( qNode->tag() == nsPrefixSVG + "linearGradient" )
            pElem = new LinearGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "radialGradient" )
            pElem = new RadialGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "use" )
            pElem = new Use ;
        else if ( qNode->tag() == nsPrefixSVG + "image" )
            pElem = new Image ;
        else if ( qNode->tag() == nsPrefixSVG + "pattern" )
            pElem = new Pattern ;
        else if ( qNode->tag() == nsPrefixSVG + "clipPath" )
            pElem = new ClipPath ;
        else if ( qNode->tag() == nsPrefixSVG + "style" )
            pElem = new StyleDefinition ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children.push_back(pElem) ;
        pElem->parent = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node

    }

    return true ;
}

bool StyleDefinition::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "type" )
            type = val ;
        else if ( key == "media" )
            media = val ;
    }

    if ( type != "text/css" ) return false ;

    string cssStr = pNode->text() ;

    return true ;
}


bool Image::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x" ) {
            if ( !x.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width.fromString(val, true) ) return false ;
        }
        else if ( key == "height" ) {
            if ( !height.fromString(val, true) ) return false ;
        }
        else if ( key == "preserveAspectRatio" ) {
            if ( !preserveAspectRatio.fromString(val) ) return false ;
        }
        else if ( key == "xlink:href" )
            img_path = val ;

    }
    return true ;
}


bool ClipPath::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "clipPathUnits" )
        {
            if ( val == "userSpaceOnUse" )
                clipPathUnits = UserSpaceOnUse ;
            else if ( val == "objectBoundingBox" )
                clipPathUnits = ObjectBoundingBox ;
        }

    }

    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        Element *pElem = 0 ;

        if ( qNode->tag() == nsPrefixSVG + "path" )
            pElem = new Path ;
        else if ( qNode->tag() == nsPrefixSVG + "text" )
            pElem = new Text ;
        else if ( qNode->tag() == nsPrefixSVG + "rect" )
            pElem = new Rect ;
        else if ( qNode->tag() == nsPrefixSVG + "line" )
            pElem = new Line ;
        else if ( qNode->tag() == nsPrefixSVG + "ellipse" )
            pElem = new Ellipse ;
        else if ( qNode->tag() == nsPrefixSVG + "polygon" )
            pElem = new Polygon ;
        else if ( qNode->tag() == nsPrefixSVG + "polyline" )
            pElem = new PolyLine ;
        else if ( qNode->tag() == nsPrefixSVG + "circle" )
            pElem = new Circle ;
        else if ( qNode->tag() == nsPrefixSVG + "use" )
            pElem = new Use ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children.push_back(pElem) ;
        pElem->parent = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node
    }

    return true ;
}


bool Use::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x" ) {
            if ( !x.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width.fromString(val) ) return false ;
        }
        else if ( key == "height" ) {
            if ( !height.fromString(val) ) return false ;
        }
        else if ( key == "xlink:href" )
        {
            refId = val ;
        }
    }

    return true ;
}



bool Text::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !TextPosElement::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "xml::space" ) {
            if ( val == "default" )
                preserveWhite = false ;
            else if ( val == "preserve" )
                preserveWhite = true ;
        }
    }

    XML_FOREACH_CHILD_NODE(pNode, node)
    {
        if ( node->nodeType() == XmlNode::TextNode )
        {
            SpanElement *elem = new SpanElement() ;
            elem->x = x ;
            elem->y = y ;
            elem->text = node->nodeValue(preserveWhite) ;
            children.push_back(elem) ;
            elem->parent = this ;
        }
        else if ( node->nodeType() == XmlNode::ElementNode &&
                  node->nodeName() == "tspan" )
        {
            SpanElement *elem = new SpanElement() ;
            if ( elem->fromXml(node->toElement()) )
            {
                children.push_back(elem) ;
                elem->parent = this ;
            }
            else delete elem ;
        }
    }

    return true ;
}


bool SpanElement::fromXml(const XmlElement *pNode)
{
  // Parse attributes

    if ( !TextPosElement::parseAttributes(pNode) ) return false ;

    bool preserveWhite_ = ((TextPosElement *)parent)->preserveWhite ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "xml::space" ) {
            if ( val == "default" )
                preserveWhite = false ;
            else if ( val == "preserve" )
                preserveWhite = true ;
        }
    }

    XML_FOREACH_CHILD_NODE(pNode, node)
    {
        if ( node->nodeType() == XmlNode::TextNode )
        {
            text = node->nodeValue(preserveWhite);
            if ( text.empty() ) return false ;
        }
        else if ( node->nodeType() == XmlNode::ElementNode &&
                  node->nodeName() == "tspan" )
        {
            SpanElement *elem = new SpanElement() ;
            if ( elem->fromXml(node->toElement()) )
            {
                elements.push_back(elem) ;
                elem->parent = this ;
            }
            else delete elem ;
        }
    }

    return true ;
}

bool Line::fromXml(const XmlElement *pNode)
{
    x1 = y1 = x2  = y2 = Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x1" ) {
            if ( !x1.fromString(val) ) return false ;
        }
        else if ( key == "y1" ) {
            if ( !y1.fromString(val) ) return false ;
        }
        else if ( key == "x2" ) {
            if ( !x2.fromString(val) ) return false ;
        }
        else if ( key == "y2" ) {
            if ( !y2.fromString(val) ) return false ;
        }
    }

    return true ;
}


bool PolyLine::fromXml(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "points" )
            if ( !points.fromString(val) ) return false ;
    }

    return true ;
}

bool Polygon::fromXml(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ;
    if ( !Transformable::parseAttributes(pNode) ) return false ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "points" )
            if ( !points.fromString(val) ) return false ;
    }

    return true ;
}

bool Circle::fromXml(const XmlElement *pNode)
{
    cx = cy = r  = Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "cx" ) {
            if ( !cx.fromString(val) ) return false ;
        }
        else if ( key == "cy" ) {
            if ( !cy.fromString(val) ) return false ;
        }
        else if ( key == "r" ) {
            if ( !r.fromString(val) ) return false ;
        }
    }

    return true ;
}

bool Ellipse::fromXml(const XmlElement *pNode)
{
    cx = cy = rx = ry  = Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "cx" ) {
            if ( !cx.fromString(val) ) return false ;
        }
        else if ( key == "cy" ) {
            if ( !cy.fromString(val) ) return false ;
        }
        else if ( key == "rx" ) {
            if ( !rx.fromString(val) ) return false ;
        }
        else if ( key == "ry" ) {
            if ( !ry.fromString(val) ) return false ;
        }
    }

    return true ;
}

bool Rect::fromXml(const XmlElement *pNode)
{
    x = y = width = height = rx = ry  =
        Length(Length::NumberLengthType, 0.0) ;

    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x" ) {
            if ( !x.fromString(val) ) return false ;
        }
        else if ( key == "y" ) {
            if ( !y.fromString(val) ) return false ;
        }
        else if ( key == "width" ) {
            if ( !width.fromString(val) ) return false ;
        }
        else if ( key == "height" ) {
            if ( !height.fromString(val) ) return false ;
        }
        else if ( key == "rx" ) {
            if ( !rx.fromString(val) ) return false ;
        }
        else if ( key == "ry" ) {
            if ( !ry.fromString(val) ) return false ;
        }
    }

    return true ;
}


bool Path::fromXml(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "d" )
            if ( !data.fromString(val) ) return false ;
    }

    return true ;
}

bool Group::fromXml(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;

    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        Element *pElem = 0 ;

        if ( qNode->tag() == nsPrefixSVG + "g" )
            pElem = new Group ;
        else if ( qNode->tag() == nsPrefixSVG + "svg" )
            pElem = new Document ;
        else if ( qNode->tag() == nsPrefixSVG + "symbol" )
            pElem = new Symbol ;
        else if ( qNode->tag() == nsPrefixSVG + "rect" )
            pElem = new Rect ;
        else if ( qNode->tag() == nsPrefixSVG + "line" )
            pElem = new Line ;
        else if ( qNode->tag() == nsPrefixSVG + "path" )
            pElem = new Path ;
        else if ( qNode->tag() == nsPrefixSVG + "ellipse" )
            pElem = new Ellipse ;
        else if ( qNode->tag() == nsPrefixSVG + "polygon" )
            pElem = new Polygon ;
        else if ( qNode->tag() == nsPrefixSVG + "polyline" )
            pElem = new PolyLine ;
        else if ( qNode->tag() == nsPrefixSVG + "circle" )
            pElem = new Circle ;
        else if ( qNode->tag() == nsPrefixSVG + "text" )
            pElem = new Text ;
        else if ( qNode->tag() == nsPrefixSVG + "defs" )
            pElem = new Defs ;
        else if ( qNode->tag() == nsPrefixSVG + "linearGradient" )
            pElem = new LinearGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "radialGradient" )
            pElem = new RadialGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "use" )
            pElem = new Use ;
        else if ( qNode->tag() == nsPrefixSVG + "image" )
            pElem = new Image ;
        else if ( qNode->tag() == nsPrefixSVG + "pattern" )
            pElem = new Pattern;
        else if ( qNode->tag() == nsPrefixSVG + "clipPath" )
            pElem = new ClipPath ;
        else if ( qNode->tag() == nsPrefixSVG + "style" )
            pElem = new StyleDefinition ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children.push_back(pElem) ;
        pElem->parent = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node


    }

    return true ;
}


bool Defs::fromXml(const XmlElement *pNode)
{
    if ( !Element::parseAttributes(pNode) ) return false ; ;
    if ( !Stylable::parseAttributes(pNode) ) return false ; ;
    if ( !Transformable::parseAttributes(pNode) ) return false ; ;

    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        Element *pElem = 0 ;

        if ( qNode->tag() == nsPrefixSVG + "g" )
            pElem = new Group ;
        else if ( qNode->tag() == nsPrefixSVG + "svg" )
            pElem = new Document ;
        else if ( qNode->tag() == nsPrefixSVG + "symbol" )
            pElem = new Symbol ;
        else if ( qNode->tag() == nsPrefixSVG + "rect" )
            pElem = new Rect ;
        else if ( qNode->tag() == nsPrefixSVG + "line" )
            pElem = new Line ;
        else if ( qNode->tag() == nsPrefixSVG + "path" )
            pElem = new Path ;
        else if ( qNode->tag() == nsPrefixSVG + "ellipse" )
            pElem = new Ellipse ;
        else if ( qNode->tag() == nsPrefixSVG + "polygon" )
            pElem = new Polygon ;
        else if ( qNode->tag() == nsPrefixSVG + "polyline" )
            pElem = new PolyLine ;
        else if ( qNode->tag() == nsPrefixSVG + "circle" )
            pElem = new Circle ;
        else if ( qNode->tag() == nsPrefixSVG + "text" )
            pElem = new Text ;
        else if ( qNode->tag() == nsPrefixSVG + "linearGradient" )
            pElem = new LinearGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "radialGradient" )
            pElem = new RadialGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "use" )
            pElem = new Use ;
        else if ( qNode->tag() == nsPrefixSVG + "image" )
            pElem = new Image ;
        else if ( qNode->tag() == nsPrefixSVG + "pattern" )
            pElem = new Pattern ;
        else if ( qNode->tag() == nsPrefixSVG + "clipPath" )
            pElem = new ClipPath ;
        else if ( qNode->tag() == nsPrefixSVG + "style" )
            pElem = new StyleDefinition ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        children.push_back(pElem) ;
        pElem->parent = this ;

        if ( !pElem->fromXml(qNode) ) return false ; // parse node
    }

    return true ;
}

bool GradientElement::parseStops(const XmlElement *pNode)
{
    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        Element *pElem = 0 ;

        if ( qNode->tag() == nsPrefixSVG + "stop" )
        {
            Stop stop ;

            XML_FOREACH_ATTRIBUTE(qNode, attr)
            {
                const string &key = attr->key(), &val = attr->value() ;

                if ( key == "offset" ) {
                     if ( !stop.offset.fromString(val) ) return false ;
                }
                else if ( key == "stop-color" )
                {
                    unsigned int clr ;
                    if ( parse_css_color(val, clr) ) stop.stopColor = clr ;
                    else return false ;
                 }
                 else if ( key == "stop-opacity" )
                    stop.stopOpacity =  Style::parseOpacity(val.c_str()) ;
                 else if ( key == "style")
                 {
                     Style st ;
                     if ( !st.fromStyleString(val.c_str()) ) return false ;

                     for( int i=0 ; i<st.flags.size() ; i++ )
                     {
                         if ( st.flags[i] == Style::StopColorState )
                             stop.stopColor = st.stopColor ;
                         else if ( st.flags[i] == Style::StopOpacityState )
                             stop.stopOpacity = st.stopOpacity ;
                     }
                 }
             }
             stops.push_back(stop) ;
        }
    }

    return true ;
}

bool LinearGradient::fromXml(const XmlElement *pNode)
{
    if ( !GradientElement::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "x1" ) {
            if( !x1.fromString(val) ) return false ;
        }
        else if ( key == "x2" ) {
            if ( !x2.fromString(val) ) return false ;
        }
        else if ( key == "y1" ) {
            if ( !y1.fromString(val) ) return false ;
        }
        else if ( key == "y2" ) {
            if ( !y2.fromString(val) ) return false  ;
        }
    }

    if ( !GradientElement::parseStops(pNode) ) return false ;

    return true ;
}

bool RadialGradient::fromXml(const XmlElement *pNode)
{
    if ( !GradientElement::parseAttributes(pNode) ) return false ;

    XML_FOREACH_ATTRIBUTE(pNode, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( key == "cx" ) {
            if ( !cx.fromString(val) ) return false ;
        }
        else if ( key == "cy" ) {
            if ( !cy.fromString(val) ) return false ;
        }
        else if ( key == "fx" ) {
            if ( !fx.fromString(val) ) return false ;
        }
        else if ( key == "fy" ) {
            if ( !fy.fromString(val) ) return false ;
        }
        else if ( key == "r" ) {
            if ( !r.fromString(val) ) return false ;
        }
    }

    if ( !GradientElement::parseStops(pNode) ) return false ;

    return true ;
}


bool Pattern::fromXml(const XmlElement *pNode)
{
    if ( !parseAttributes(pNode) ) return false ;

    string nsPrefixSVG = pNode->resolveNamespace(NS_SVG) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        Element *pElem = 0 ;

        if ( qNode->tag() == nsPrefixSVG + "g" )
            pElem = new Group ;
        else if ( qNode->tag() == nsPrefixSVG + "svg" )
            pElem = new Document ;
        else if ( qNode->tag() == nsPrefixSVG + "symbol" )
            pElem = new Symbol ;
        else if ( qNode->tag() == nsPrefixSVG + "rect" )
            pElem = new Rect ;
        else if ( qNode->tag() == nsPrefixSVG + "line" )
            pElem = new Line ;
        else if ( qNode->tag() == nsPrefixSVG + "path" )
            pElem = new Path ;
        else if ( qNode->tag() == nsPrefixSVG + "ellipse" )
            pElem = new Ellipse ;
        else if ( qNode->tag() == nsPrefixSVG + "polygon" )
            pElem = new Polygon ;
        else if ( qNode->tag() == nsPrefixSVG + "polyline" )
            pElem = new PolyLine ;
        else if ( qNode->tag() == nsPrefixSVG + "circle" )
            pElem = new Circle ;
        else if ( qNode->tag() == nsPrefixSVG + "text" )
            pElem = new Text ;
        else if ( qNode->tag() == nsPrefixSVG + "linearGradient" )
            pElem = new LinearGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "radialGradient" )
            pElem = new RadialGradient ;
        else if ( qNode->tag() == nsPrefixSVG + "use" )
            pElem = new Use ;
        else if ( qNode->tag() == nsPrefixSVG + "image" )
            pElem = new Image ;
        else if ( qNode->tag() == nsPrefixSVG + "pattern" )
            pElem = new Pattern ;
        else if ( qNode->tag() == nsPrefixSVG + "clipPath" )
            pElem = new ClipPath ;
        else if ( qNode->tag() == nsPrefixSVG + "style" )
            pElem = new StyleDefinition ;

        if ( !pElem ) continue ; // ignore unsupported nodes

        if ( pElem->fromXml(qNode) )
        {
            children.push_back(pElem) ;
            pElem->parent = this ;
        }

    }

    return true ;
}

bool Style::parseNameValue(const std::string &name, const std::string &val_)
{
    string val = boost::trim_copy(val_) ;

    if ( name == "fill-rule" )
    {
        if ( val == "nonzero" ) {
            fillRule = NonZeroFillRule ;
            flags.push_back(FillRuleState) ;
        }
        else if ( val == "evenodd" ) {
            fillRule = EvenOddFillRule ;
            flags.push_back(FillRuleState) ;
        }
        else if ( val == "inherit" ) ;
    }
    else if ( name == "fill-opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            fillOpacity = parseOpacity(val) ;
            flags.push_back(FillOpacityState) ;
        }
    }
    else if ( name == "opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            opacity = parseOpacity(val) ;
           flags.push_back(OpacityState) ;
        }
    }
    else if ( name == "stoke-opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            strokeOpacity = parseOpacity(val) ;
            flags.push_back(StrokeOpacityState) ;
        }
    }
    else if ( name == "clip-rule" ) flags.push_back(ClipRuleState) ;
    else if ( name == "fill")
    {
        parsePaint(val, true) ;
        flags.push_back(FillState) ;
    }
    else if ( name == "stroke" )
    {
        parsePaint(val, false) ;
        flags.push_back(StrokeState) ;
    }
    else if ( name == "stroke-width" )
    {
        if ( val == "inherit" ) ;
        else {
          flags.push_back(StrokeWidthState) ;
          strokeWidth.fromString(val) ;
        }
    }
    else if ( name == "stroke-miterlimit" )
    {
        if ( val == "inherit" ) ;
        else
        {
            double dval ;
            if ( !parseFloatingPoint(val, dval) ) return false ;

            miterLimit = dval ;
            flags.push_back(StrokeMiterLimitState) ;
        }
    }
    else if ( name == "stroke-dasharray" )
    {
      if ( val == "none" )
      {
          solidStroke = true ;
          flags.push_back(StrokeDashArrayState) ;
      }
      else if ( val == "inherit" ) ;
      else
      {
          if ( !Length::parseList(val, dashArray) ) return false ;

            solidStroke = false ;
            flags.push_back(StrokeDashArrayState) ;
        }
    }
    else if ( name == "stroke-dashoffset" )
    {
        if ( val == "inherit" );
        else {
            flags.push_back(StrokeDashOffsetState) ;
            if ( !dashOffset.fromString(val) ) return false;
        }
    }
    else if ( name == "stroke-linejoin" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "miter" )
        {
            flags.push_back(StrokeLineJoinState) ;
            lineJoin = MiterLineJoin ;
        }
        else if ( val == "round" )
        {
            flags.push_back(StrokeLineJoinState) ;
            lineJoin = RoundLineJoin ;
        }
        else if ( val == "bevel" )
        {
            flags.push_back(StrokeLineJoinState) ;
            lineJoin = BevelLineJoin ;
        }
    }
    else if ( name == "stroke-linecap" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "butt" )
        {
            flags.push_back(StrokeLineCapState) ;
            lineCap = ButtLineCap ;
        }
        else if ( val == "round" )
        {
            flags.push_back(StrokeLineCapState) ;
            lineCap = RoundLineCap ;
        }
        else if ( val == "square" )
        {
            flags.push_back(StrokeLineCapState) ;
            lineCap = SquareLineCap ;
        }
    }
    else if ( name == "font-family" )
    {
        if ( val.empty() ) ;
        else if ( val == "inherit" ) ;
        else {
            fontFamily = val ;
            flags.push_back(FontFamilyState) ;
        }
    }
    else if ( name == "font-style" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "normal" )
        {
            fontStyle = NormalFontStyle ;
            flags.push_back(FontStyleState) ;
        }
        else if ( val == "oblique" )
        {
            fontStyle = ObliqueFontStyle ;
            flags.push_back(FontStyleState) ;
        }
        else if ( val == "italic" )
        {
            fontStyle = ItalicFontStyle ;
            flags.push_back(FontStyleState) ;
        }
    }
    else if ( name == "font-variant" )
    {
        if ( val == "inherit" );
        else if ( val == "normal" )
        {
            fontVariant = NormalFontVariant ;
            flags.push_back(FontVariantState) ;
        }
        else if ( val == "small-caps" )
        {
            fontVariant = SmallCapsFontVariant ;
            flags.push_back(FontVariantState) ;
        }
    }
    else if ( name == "font-weight" )
    {
        if ( val == "normal" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = NormalFontWeight ;
        }
        else if ( val == "bold" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = BoldFontWeight ;
        }
        else if ( val == "bolder" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = BolderFontWeight ;
        }
        else if ( val == "lighter" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = LighterFontWeight ;
        }
        else if ( val == "100" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W100FontWeight ;
        }
        else if ( val == "200" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W200FontWeight ;
        }
        else if ( val == "300" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W300FontWeight ;
        }
        else if ( val == "400" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W400FontWeight ;
        }
        else if ( val == "500" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W500FontWeight ;
        }
        else if ( val == "600" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W600FontWeight ;
        }
        else if ( val == "700" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W700FontWeight ;
        }
        else if ( val == "800" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W800FontWeight ;
        }
        else if ( val == "900" )
        {
            flags.push_back(FontWeightState) ;
            fontWeight = W900FontWeight ;
        }
        else if ( val == "inherit" ) ;
    }
    else if ( name == "font-stretch" )
    {
        if ( val == "ultra-condensed" )
        {
            fontStretch = UltraCondensedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "extra-condensed" )
        {
            fontStretch = ExtraCondensedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "condensed" )
        {
            fontStretch = CondensedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "narrower" )
        {
            fontStretch = NarrowerFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "semi-condensed" )
        {
            fontStretch = SemiCondensedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "semi-expanded" )
        {
            fontStretch = SemiExpandedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "expanded" )
        {
            fontStretch = ExpandedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "wider" )
        {
            fontStretch = WiderFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "extra-expanded" )
        {
            fontStretch = ExtraExpandedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "ultra-expanded" )
        {
            fontStretch = UltraExpandedFontStretch ;
            flags.push_back(FontStretchState) ;
        }
        else if ( val == "inherit" ) ;
    }
    else if ( name == "font-size" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "xx-small" )
        {
            fontSize = Length(Length::PTLengthType,  6.94) ;
            flags.push_back(FontSizeState) ;
        }
        else if ( val == "x-small" )
        {
            fontSize = Length(Length::PTLengthType,  8.33) ;
            flags.push_back(FontSizeState) ;
        }
        else if ( val == "small" )
        {
            fontSize = Length(Length::PTLengthType,  10) ;
            flags.push_back(FontSizeState) ;
        }
        else if ( val == "medium" )
        {
            fontSize = Length(Length::PTLengthType,  12) ;
            flags.push_back(FontSizeState) ;
        }
        else if ( val == "large" )
        {
            fontSize = Length(Length::PTLengthType,  14.4) ;
            flags.push_back(FontSizeState) ;
        }
        else if ( val == "x-large" )
        {
            fontSize = Length(Length::PTLengthType,  17.28) ;
            flags.push_back(FontSizeState) ;
        }
        else if ( val == "xx-large" )
        {
            fontSize = Length(Length::PTLengthType,  20.73) ;
            flags.push_back(FontSizeState) ;
        }
        else
        {
            if ( !fontSize.fromString(val) ) return false ;
            flags.push_back(FontSizeState) ;
        }
    }
    else if ( name == "text-decoration" )
    {
        if ( val == "inherit" ) ;
        else if ( val == "none" ) ;
        else if ( val == "underline" )
        {
            flags.push_back(Style::TextDecorationState) ;
            textDecoration = UnderlineDecoration ;
        }
        else if ( val == "overline" )
        {
            flags.push_back(Style::TextDecorationState) ;
            textDecoration = OverlineDecoration ;
        }
        else if ( val == "strike" )
        {
            flags.push_back(Style::TextDecorationState) ;
            textDecoration = StrikeDecoration ;
        }
        else if ( val == "line-through" )
        {
            flags.push_back(Style::TextDecorationState) ;
            textDecoration = OverlineDecoration ;
        }
    }
    else if ( name == "text-anchor" )
    {
        if ( val == "inherit" );
        else if ( val == "start" )
        {
            flags.push_back(Style::TextAnchorState) ;
            textAnchor = StartTextAnchor ;

        }
        else if ( val == "middle" )
        {
            flags.push_back(Style::TextAnchorState) ;
            textAnchor = MiddleTextAnchor ;
        }
        else if ( val == "end" )
        {
            flags.push_back(Style::TextAnchorState) ;
            textAnchor = EndTextAnchor ;
        }
    }
    else if ( name == "display" )
    {
        if ( val == "none" ) {
            displayMode = NoDisplay ;
            flags.push_back(Style::DisplayState) ;
        }
        else if ( val == "inline" ) 	{
            displayMode = InlineDisplay ;
            flags.push_back(Style::DisplayState) ;
        }

    }
    else if ( name == "visibility" )
    {
        if ( val == "none" ) {
            visibilityMode = NoVisibility ;
            flags.push_back(Style::VisibilityState) ;
        }
        else if ( val == "hidden" ||  val == "collapsed" ) {
            visibilityMode = HiddenVisibility ;
            flags.push_back(Style::VisibilityState) ;
        }
    }
    else if ( name == "shape-rendering" )
    {
        if ( val == "auto" || val == "default" )
        {
            shapeRenderingQuality = AutoShapeQuality ;
            flags.push_back(Style::ShapeRenderingState) ;
        }
        else if ( val == "optimizeSpeed" )
        {
            shapeRenderingQuality = OptimizeSpeedShapeQuality ;
            flags.push_back(Style::ShapeRenderingState) ;
        }
        else if ( val == "crispEdges" )
        {
            shapeRenderingQuality = CrispEdgesShapeQuality ;
            flags.push_back(Style::ShapeRenderingState) ;
        }
        else if ( val == "geometricPrecision" )
        {
            shapeRenderingQuality = GeometricPrecisionShapeQuality ;
            flags.push_back(Style::ShapeRenderingState) ;
        }
    }
    else if ( name == "text-rendering" )
    {
        if ( val == "auto" || val == "default" )
        {
            textRenderingQuality = AutoTextQuality ;
            flags.push_back(Style::TextRenderingState) ;
        }
        else if ( val == "optimizeSpeed" )
        {
            textRenderingQuality = OptimizeSpeedTextQuality ;
            flags.push_back(Style::TextRenderingState) ;
        }
        else if ( val == "optimizeLegibility" )
        {
            textRenderingQuality = OptimizeLegibilityTextQuality ;
            flags.push_back(Style::TextRenderingState) ;
        }
        else if ( val == "geometricPrecision" )
        {
            textRenderingQuality = GeometricPrecisionTextQuality ;
            flags.push_back(Style::TextRenderingState) ;
        }
    }
    else if ( name == "stop-opacity" )
    {
        if ( val == "inherit" ) ;
        else {
            stopOpacity = parseOpacity(val) ;
            flags.push_back(StopOpacityState) ;
        }
    }
    else if ( name == "stop-color" )
    {
        if ( val == "inherit" ) ;
        else {
            unsigned int clr ;
            if ( parse_css_color(val, clr) )
            {
                stopColor = clr ;
                flags.push_back(StopColorState) ;
            }
        }
    }
    else if ( name == "overflow" )
    {
        if ( val == "visible"  || val == "auto" )
        {
            overflow = true ;
            flags.push_back(OverflowState) ;
        }
        else if ( val == "hidden"  || val == "scroll" )
        {
            overflow = false ;
            flags.push_back(OverflowState) ;
        }
    }
    else if ( name == "clip-path" )
    {
        if ( val == "none" )
            flags.push_back(OverflowState) ;
        else
        {
            clipPathId = parseUri(val) ;
            flags.push_back(ClipPathState) ;
        }
    }

    return true ;
}

bool Style::fromStyleString(const std::string &str)
{

    boost::sregex_iterator it(str.begin(), str.end(), boost::regex("([a-zA-Z-]+)[\\s]*:[\\s]*([^:;]+)[\\s]*[;]?")) ;
    boost::sregex_iterator end ;

    while ( it != end )
    {
        if ( !parseNameValue(it->str(1), it->str(2)) ) return false ;

        ++it ;
    }

    return true ;

}


bool DocumentInstance::load(istream &strm)
{
    XmlDocument xml ;

    if ( !xml.load(strm) ) return false ;

    const XmlElement *pNode = xml.root()->toElement() ;

    if ( !pNode ) return false ;

    if ( pNode->nodeName() ==  "svg" )
    {
        root = new Document() ;
        return root->fromXml(pNode->toElement()) ;
    }
    else return false ;

}

DocumentInstance::DocumentInstance(): root(0) {
}

DocumentInstance::~DocumentInstance() {
    if ( root ) delete root ;
}

}

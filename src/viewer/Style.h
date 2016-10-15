#ifndef __SLD_READER_H__
#define __SLD_READER_H__

#include "Feature.h"
#include "MapFile.h"

#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>

class XmlElement ;

namespace sld {

struct ParseContext ;

// abstract ogc Filter
class Filter {
public:
    enum Type { Expression, Else } ;

    virtual Type type() const = 0 ;

    Filter() {}

    virtual ~Filter() {}
};

// sld:ElseFilter
class FilterOtherwise: public Filter {
public:
    Type type() const { return Filter::Else ; }
};

class ExpressionNode ;

// ogc:Filter
class FilterExpression: public Filter {

public:

    Type type() const { return Filter::Expression ; }

    // convert the expression tree into an SQL WHERE conditional expression.

    std::string toSQL() const ;

private:

    friend class Rule ;

    bool fromXml(const XmlElement *, const ParseContext *) ;

    boost::shared_ptr<ExpressionNode> node_ ;
};

typedef boost::shared_ptr<Filter> FilterPtr ;

// Abstract symbolizer
class Symbolizer {
public:
    enum Type { Point, Line, Polygon, Text, Raster } ;

    virtual Type type() const = 0 ;
    virtual bool fromXml(const XmlElement *, const ParseContext *) = 0;
};

typedef boost::shared_ptr<Symbolizer> SymbolizerPtr ;

class ParameterValue {

public:
    ~ParameterValue();

    Value eval(const Feature &f) const ;

private:
    friend class Graphic ;
    friend class TextSymbolizer ;
    friend class LineSymbolizer ;
    friend class RasterSymbolizer ;
    friend class PointPlacement ;
    friend class LinePlacement ;
    friend class Halo ;
    friend class Stroke ;
    friend class Fill ;
    friend class GraphicStroke ;
    friend class GraphicFill ;

    bool fromXml(const XmlElement *pNode, const ParseContext *);

    std::vector<ExpressionNode *> mixedExpression ;
};

class ExternalGraphic {
public:
    std::string href ;
    std::string format ;
private:
    friend class Graphic ;
    bool fromXml(const XmlElement *pNode, const ParseContext *);
};
typedef boost::shared_ptr<ExternalGraphic> ExternalGraphicPtr ;

class Fill ;
typedef boost::shared_ptr<Fill> FillPtr ;

class Stroke ;
typedef boost::shared_ptr<Stroke> StrokePtr ;

class Mark {
public:
    std::string wellKnownName ;
    FillPtr fill ;
    StrokePtr stroke ;

private:
    friend class Graphic ;
    bool fromXml(const XmlElement *, const ParseContext *) ;

};
typedef boost::shared_ptr<Mark> MarkPtr ;

class Graphic
{
public:

    ParameterValue opacity ;
    ParameterValue size ;
    ParameterValue rotation ;
    ParameterValue anchorPointX, anchorPointY ;
    ParameterValue displacementX, displacementY ;

    ExternalGraphicPtr externalGraphic ;
    MarkPtr mark ;

private:
    friend class PointSymbolizer ;
    friend class GraphicFill ;
    friend class GraphicStroke ;

    bool fromXml(const XmlElement *, const ParseContext *) ;
};
typedef boost::shared_ptr<Graphic> GraphicPtr ;


class GraphicFill {
public:
    GraphicPtr graphic ;

private:

    friend class Stroke ;
    friend class Fill ;

    bool fromXml(const XmlElement *pNode, const ParseContext *) ;
};
typedef boost::shared_ptr<GraphicFill> GraphicFillPtr ;

class GraphicStroke {
public:

    std::vector<GraphicPtr> graphics ;

    ParameterValue initialGap ;
    ParameterValue gap ;

private:

    friend class Stroke ;

    bool fromXml(const XmlElement *pNode, const ParseContext *) ;
};
typedef boost::shared_ptr<GraphicStroke> GraphicStrokePtr ;

typedef boost::shared_ptr<GraphicFill> GraphicFillPtr ;

class Fill {
public:
    GraphicFillPtr gfill ;

    ParameterValue fill ;
    ParameterValue fillOpacity ;

private:

    friend class Mark ;
    friend class PolygonSymbolizer ;
    friend class TextSymbolizer ;
    friend class Halo ;

    bool fromXml(const XmlElement *pNode, const ParseContext *) ;
};

class Stroke {
public:
    GraphicFillPtr gfill ;
    GraphicStrokePtr gstroke ;

    ParameterValue stroke ;
    ParameterValue strokeOpacity ;
    ParameterValue strokeWidth ;
    ParameterValue strokeLineJoin ;
    ParameterValue strokeLineCap ;
    ParameterValue strokeDashArray ;
    ParameterValue strokeDashOffset ;
private:

    friend class Mark ;
    friend class LineSymbolizer ;
    friend class PolygonSymbolizer ;

    bool fromXml(const XmlElement *pNode, const ParseContext *) ;
};



class PointSymbolizer: public Symbolizer {

public:

    Type type() const { return Point ; }

public:

    std::vector<GraphicPtr> graphics ;

private:
    friend class Rule ;
    bool fromXml(const XmlElement *, const ParseContext *) ;
};

class LineSymbolizer: public Symbolizer {

    Type type() const { return Line ; }

public:

    StrokePtr stroke ;
    ParameterValue perpendicularOffset ;

private:
    friend class Rule ;
    bool fromXml(const XmlElement *, const ParseContext *) ;

};

class PolygonSymbolizer: public Symbolizer {

public:

    Type type() const { return Polygon; }

public:

    StrokePtr stroke ;
    FillPtr fill ;

private:
    friend class Rule ;
    bool fromXml(const XmlElement *, const ParseContext *) ;

};

class LinePlacement {
public:
    ParameterValue perpendicularOffset ;
    ParameterValue initialGap ;
    ParameterValue gap ;
    ParameterValue isRepeated ;
    ParameterValue isAligned ;

private:

    friend class TextSymbolizer ;
    virtual bool fromXml(const XmlElement *, const ParseContext *) ;
};

typedef boost::shared_ptr<LinePlacement> LinePlacementPtr ;

class PointPlacement {
public:
    ParameterValue anchorPointX ;
    ParameterValue anchorPointY ;
    ParameterValue displacementX ;
    ParameterValue displacementY ;
    ParameterValue rotation ;

private:

    friend class TextSymbolizer ;
    virtual bool fromXml(const XmlElement *, const ParseContext *) ;
};

typedef boost::shared_ptr<PointPlacement> PointPlacementPtr ;

class Halo {
public:
    ParameterValue radius ;
    FillPtr fill ;

private:

    friend class TextSymbolizer ;
    virtual bool fromXml(const XmlElement *, const ParseContext *) ;
};

typedef boost::shared_ptr<Halo> HaloPtr ;


class TextSymbolizer: public Symbolizer {
public:

    Type type() const { return Text ; }

public:
    ParameterValue label ;

    ParameterValue fontFamily ;
    ParameterValue fontSize ;
    ParameterValue fontStyle ;
    ParameterValue fontWeight ;

    LinePlacementPtr linePlacement ;
    PointPlacementPtr pointPlacement ;
    HaloPtr halo ;
    FillPtr fill ;

private:
    friend class Rule ;
    bool fromXml(const XmlElement *, const ParseContext *) ;
};


class RasterSymbolizer: public Symbolizer {

public:
    Type type() const { return Raster ; }

public:

    ParameterValue opacity ;

    float reliefFactor ;
    bool reliefBrightnessOnly ;

private:

    friend class Rule ;

    RasterSymbolizer(): reliefFactor(0.0), reliefBrightnessOnly(false) {}

    bool fromXml(const XmlElement *, const ParseContext *) ;
};


class Rule {

public:

    Rule() ;

    FilterPtr filter ;
    std::vector<SymbolizerPtr> symbolizers ;

    double minScaleDenom, maxScaleDenom ;

private:

    friend class FeatureTypeStyle ;
    bool fromXml(const XmlElement *, const ParseContext *) ;
};

typedef boost::shared_ptr<Rule> RulePtr ;

class FeatureTypeStyle {
public:

    std::vector<RulePtr> rules ;

private:

    friend class Reader ;
    bool fromXml(const XmlElement *, const ParseContext *) ;
};





// read feature type styles from stream

bool fromXml(const MapFile &m, const std::string &fileName, std::vector<FeatureTypeStylePtr> &styles) ;


}


#endif

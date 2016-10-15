#include "Style.h"
#include "XmlDocument.h"
#include "ParseUtil.h"
#include "MapFile.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <float.h>
#include <math.h>
#include <errno.h>

using namespace std ;

namespace sld {

const char *NS_SLD = "http://www.opengis.net/sld"  ;
const char *NS_SE = "http://www.opengis.net/se"  ;
const char *NS_OGC = "http://www.opengis.net/ogc" ;

struct ParseContext {
    string resource_file_name_ ;
};

class Reader {

public:

    Reader(const MapFile &mf): mf_(mf) {}

    bool load(const string &file_name, std::vector<FeatureTypeStylePtr> &styles) ;

    const MapFile &mf_ ;

};

bool Reader::load(const string &file_name, std::vector<sld::FeatureTypeStylePtr> &styles)
{

    string payload = mf_.readResourceFile(file_name) ;
    if ( payload.empty() ) return false ;

    istringstream strm(payload) ;

    XmlDocument doc ;

    if ( !doc.load(strm) ) return false ;

    const XmlElement *pNode = doc.root()->toElement() ;

    if ( !pNode ) return false ;

    string nsPrefixSLD = pNode->resolveNamespace(NS_SLD) ;
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    if ( pNode->tag() != nsPrefixSLD + "StyledLayerDescriptor" ) return false ;

    pNode = pNode->firstChildElement(nsPrefixSLD + "NamedLayer") ;

    if ( !pNode ) return false ;

    pNode = pNode->firstChildElement(nsPrefixSLD + "UserStyle") ;

    if ( !pNode ) return false ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        if ( qNode->tag() == nsPrefixSE + "FeatureTypeStyle" )
        {
            FeatureTypeStylePtr style(new FeatureTypeStyle) ;

            ParseContext ctx ;
            ctx.resource_file_name_ = file_name ;

            if ( style->fromXml(qNode, &ctx) )
                styles.push_back(style) ;
        }
    }

    return ( !styles.empty() )  ;
}

bool FeatureTypeStyle::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE  = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        if ( qNode->tag() == nsPrefixSE + "Rule" )
        {
            RulePtr rule(new Rule) ;

            if ( rule->fromXml(qNode, ctx) ) {
                rules.push_back(rule) ;
            }
        }
    }

    return ( !rules.empty() ) ;
}

Rule::Rule(): minScaleDenom(0), maxScaleDenom(DBL_MAX) {
}

bool Rule::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE),
           nsPrefixOGC = pNode->resolveNamespace(NS_OGC) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        if ( qNode->tag() == nsPrefixSE + "MinScaleDenominator" )  {
            if ( !parseNumber(qNode->text(), minScaleDenom) ) return false ;
        }
        else if ( qNode->tag() == nsPrefixSE + "MaxScaleDenominator" ) {
            if ( !parseNumber(qNode->text(), maxScaleDenom) ) return false ;
        }
        else if ( qNode->tag() == nsPrefixSE + "ElseFilter" ) {
            filter.reset(new FilterOtherwise) ;
        }
        else if ( qNode->tag() == nsPrefixOGC + "Filter" ) {
            FilterExpression *fe = new FilterExpression ;
            filter.reset(fe) ;

            if ( !fe->fromXml(qNode, ctx) ) return false ;
        }
        else if ( qNode->tag() == nsPrefixSE + "PointSymbolizer" ) {
            PointSymbolizer *psym_ = new PointSymbolizer ;

            if ( !psym_->fromXml(qNode, ctx) ) return false ;
            symbolizers.push_back(SymbolizerPtr(psym_)) ;
        }
        else if ( qNode->tag() == nsPrefixSE + "LineSymbolizer" ) {
            LineSymbolizer *psym_ = new LineSymbolizer ;

            if ( !psym_->fromXml(qNode, ctx) ) return false ;
            symbolizers.push_back(SymbolizerPtr(psym_)) ;
        }
        else if ( qNode->tag() == nsPrefixSE + "PolygonSymbolizer" ) {
            PolygonSymbolizer *psym_ = new PolygonSymbolizer ;

            if ( !psym_->fromXml(qNode, ctx) ) return false ;
            symbolizers.push_back(SymbolizerPtr(psym_)) ;
        }
        else if ( qNode->tag() == nsPrefixSE + "TextSymbolizer" ) {
            TextSymbolizer *psym_ = new TextSymbolizer ;

            if ( !psym_->fromXml(qNode, ctx) ) return false ;
            symbolizers.push_back(SymbolizerPtr(psym_)) ;
        }
        else if ( qNode->tag() == nsPrefixSE + "RasterSymbolizer" ) {
            RasterSymbolizer *psym_ = new RasterSymbolizer ;

            if ( !psym_->fromXml(qNode, ctx) ) return false ;
            symbolizers.push_back(SymbolizerPtr(psym_)) ;
        }

    }

    return ( !symbolizers.empty() ) ;
}


bool PointSymbolizer::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        if ( qNode->tag() == nsPrefixSE + "Graphic" )  {

            GraphicPtr g(new Graphic) ;

            if ( !g->fromXml(qNode, ctx) ) return false ;
            else graphics.push_back(g) ;
        }
    }

    return true ;
}

bool Graphic::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "ExternalGraphic" )  {

            ExternalGraphic *g = new ExternalGraphic ;

            if ( !g->fromXml(qNode, ctx) ) {
                delete g ;
                return false ;
            }
            else externalGraphic.reset(g) ;
        }
        else if ( tag == nsPrefixSE + "Mark" )  {
            Mark *g = new Mark ;

            if ( !g->fromXml(qNode, ctx) ) {
                delete g ;
                return false ;
            }
            else mark.reset(g) ;
        }
        else if ( tag == nsPrefixSE + "Opacity" )
        {
            if ( !opacity.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Size" )
        {
            if ( !size.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Rotation" )
        {
            if ( !rotation.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "AnchorPoint" )
        {
            const XmlElement *nx = qNode->firstChildElement(nsPrefixSE + "AnchorPointX") ;

            const XmlElement *ny = qNode->firstChildElement(nsPrefixSE + "AnchorPointY") ;

            if ( nx && !anchorPointX.fromXml(nx, ctx) ) return false ;
            if ( ny && !anchorPointY.fromXml(ny, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Displacement" )
        {
            const XmlElement *nx = qNode->firstChildElement(nsPrefixSE + "DisplacementX") ;

            const XmlElement *ny = qNode->firstChildElement(nsPrefixSE + "DisplacementY") ;

            if ( nx && !displacementX.fromXml(nx, ctx) ) return false ;
            if ( ny && !displacementY.fromXml(ny, ctx) ) return false ;
        }

    }

    return true ;

}

bool ExternalGraphic::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "OnlineResource" )
        {
            if ( qNode->attribute("xlink:type") != "simple" ) return false ;

            href = qNode->attribute("xlink:href") ;

            if ( href.empty() ) return false ;

        }
        else if ( tag == nsPrefixSE + "Format" )
        {
            format = qNode->text() ;
        }
    }

    return true ;

}

bool Mark::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "WellKnownName" )
        {
            wellKnownName = qNode->text() ;
        }
        else if ( tag == nsPrefixSE + "Fill" )
        {
            FillPtr f(new Fill) ;

            if ( !f->fromXml(qNode, ctx) ) return false ;
            else fill = f ;
        }
        else if ( tag == nsPrefixSE + "Stroke" )
        {
            StrokePtr s(new Stroke) ;

            if ( !s->fromXml(qNode, ctx) ) return false ;
            else stroke = s ;
        }
    }

    return true ;

}

bool LineSymbolizer::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        if ( qNode->tag() == nsPrefixSE + "Stroke" )  {
            StrokePtr s(new Stroke) ;

            if ( !s->fromXml(qNode, ctx) ) return false ;
            else stroke = s ;
        }
        else if ( qNode->tag() == nsPrefixSE + "PerpendicularOffset" )  {
             if ( !perpendicularOffset.fromXml(qNode, ctx) ) return false ;
        }
    }

    return true ;

}

bool PolygonSymbolizer::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        if ( qNode->tag() == nsPrefixSE + "Stroke" )  {
            StrokePtr s(new Stroke) ;

            if ( !s->fromXml(qNode, ctx) ) return false ;
            else stroke = s ;
        }
        else if ( qNode->tag() == nsPrefixSE + "Fill" )  {
            FillPtr f(new Fill) ;

            if ( !f->fromXml(qNode, ctx) ) return false ;
            else fill = f ;
        }
    }

    return true ;

}


bool TextSymbolizer::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "Label" )  {
            if ( !label.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Font" )  {

            XML_FOREACH_CHILD_ELEMENT(qNode, cNode)
            {
                if ( cNode->tag() == nsPrefixSE + "CssParameter" || cNode->tag() == nsPrefixSE + "SvgParameter")
                {
                    string name = cNode->attribute("name") ;

                    if ( name == "font-family" ) {
                        if ( !fontFamily.fromXml(cNode, ctx) ) return false ;
                    }
                    else if ( name == "font-style" ) {
                        if ( !fontStyle.fromXml(cNode, ctx) ) return false ;
                    }
                    else if ( name == "font-weight" ) {
                        if ( !fontStyle.fromXml(cNode, ctx) ) return false ;
                    }
                    else if ( name == "font-size" ) {
                        if ( !fontSize.fromXml(cNode, ctx) ) return false ;
                    }
                }

            }
        }
        else if ( tag == nsPrefixSE + "LabelPlacement" )
        {
            XML_FOREACH_CHILD_ELEMENT(qNode, cNode)
            {
                if ( cNode->tag() == nsPrefixSE + "LinePlacement" )
                {
                    LinePlacementPtr lp(new LinePlacement) ;

                    if ( !lp->fromXml(cNode, ctx) ) return false ;
                    else linePlacement = lp ;

                }
                else if ( cNode->tag() == nsPrefixSE + "PointPlacement" )
                {
                    PointPlacementPtr pp(new PointPlacement) ;

                    if ( !pp->fromXml(cNode, ctx) ) return false ;
                    else pointPlacement = pp ;
                }

            }
        }
        else if ( tag == nsPrefixSE + "Halo")
        {
            HaloPtr hp(new Halo) ;
            if ( !hp->fromXml(qNode, ctx) ) return false ;
            else halo = hp ;
        }
        else if ( tag == nsPrefixSE + "Fill")
        {
            FillPtr fp(new Fill) ;
            if ( !fp->fromXml(qNode, ctx) ) return false ;
            else fill = fp ;

        }
    }

    return true ;

}

bool PointPlacement::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "AnchorPoint" )
        {
            const XmlElement *nx = qNode->firstChildElement(nsPrefixSE + "AnchorPointX") ;
            if ( !nx ) return false ;

            const XmlElement *ny = qNode->firstChildElement(nsPrefixSE + "AnchorPointY") ;
            if ( !ny ) return false ;

            if ( !anchorPointX.fromXml(nx, ctx) ) return false ;
            if ( !anchorPointY.fromXml(ny, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Displacement" )
        {
            const XmlElement *nx = qNode->firstChildElement(nsPrefixSE + "DisplacementX") ;
            if ( !nx ) return false ;

            const XmlElement *ny = qNode->firstChildElement(nsPrefixSE + "DisplacementY") ;
            if ( !ny ) return false ;

            if ( !displacementX.fromXml(nx, ctx) ) return false ;
            if ( !displacementY.fromXml(ny, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Rotation" )
        {
            if ( !rotation.fromXml(qNode, ctx) ) return false ;
        }
    }

    return true ;

}

bool LinePlacement::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "PerpendicularOffset" )
        {
            if ( !perpendicularOffset.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "Gap" )
        {
            if ( !gap.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "InitialGap" )
        {
            if ( !initialGap.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "IsRepeated" )
        {
            if ( !isRepeated.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "IsAligned" ) // This has a different interpretation. To follow the line
        {
            if ( !isAligned.fromXml(qNode, ctx) ) return false ;
        }
    }

    return true ;

}

bool Halo::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "Radius" )
        {
            if ( !radius.fromXml(qNode, ctx) ) return false ;

        }
        else if ( tag == nsPrefixSE + "Fill" )
        {
            FillPtr f(new Fill) ;

            if ( !f->fromXml(qNode, ctx) ) return false ;
            else fill = f ;
        }
    }

    return true ;

}

bool Stroke::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "CssParameter" || tag == nsPrefixSE + "SvgParameter")
        {
            string name = qNode->attribute("name") ;

            if ( name == "stroke" ) {
                if ( !stroke.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "stroke-width" ) {
                if ( !strokeWidth.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "stroke-opacity" ) {
                if ( !strokeOpacity.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "stroke-linejoin" ) {
                if ( !strokeLineJoin.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "stroke-linecap" ) {
                if ( !strokeLineCap.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "stroke-dasharray" ) {
                if ( !strokeDashArray.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "stroke-dashoffset" ) {
                if ( !strokeDashOffset.fromXml(qNode, ctx) ) return false ;
            }
        }
        else if ( tag == nsPrefixSE + "GraphicStroke" )
        {
            GraphicStrokePtr gs(new GraphicStroke) ;
            if ( !gs->fromXml(qNode, ctx) ) return false ;
            else gstroke = gs ;
        }
        else if ( tag == nsPrefixSE + "GraphicFill" )
        {
            GraphicFillPtr gf(new GraphicFill) ;
            if ( !gf->fromXml(qNode, ctx) ) return false ;
            else gfill = gf ;
        }

    }
    return true ;

}

bool Fill::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "CssParameter" || tag == nsPrefixSE + "SvgParameter")
        {
            string name = qNode->attribute("name") ;

            if ( name == "fill" ) {
                if ( !fill.fromXml(qNode, ctx) ) return false ;
            }
            else if ( name == "fill-opacity" ) {
                if ( !fillOpacity.fromXml(qNode, ctx) ) return false ;
            }
        }
        else if ( tag == nsPrefixSE + "GraphicFill" )
        {
            GraphicFillPtr gf(new GraphicFill) ;
            if ( !gf->fromXml(qNode, ctx) ) return false ;
            else gfill = gf ;
        }


    }
    return true ;

}

bool GraphicFill::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "Graphic" )
        {
            GraphicPtr g(new Graphic) ;
            if ( !g->fromXml(qNode, ctx) ) return false ;
            else graphic = g ;
        }

    }
    return true ;

}

bool GraphicStroke::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "Graphic" )
        {
            GraphicPtr g(new Graphic) ;
            if ( !g->fromXml(qNode, ctx) ) return false ;
            else graphics.push_back(g) ;
        }
        else if ( tag == nsPrefixSE + "Gap")
        {
            if ( !gap.fromXml(qNode, ctx) ) return false ;
        }
        else if ( tag == nsPrefixSE + "InitialGap" )
        {
            if ( !initialGap.fromXml(qNode, ctx) ) return false ;
        }
    }
    return true ;

}

bool RasterSymbolizer::fromXml(const XmlElement *pNode, const ParseContext *ctx)
{
    string nsPrefixSE = pNode->resolveNamespace(NS_SE) ;

    XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
    {
        string tag = qNode->tag() ;

        if ( tag == nsPrefixSE + "Opacity" )  {
            if ( !opacity.fromXml(qNode, ctx) ) return false ;
        }
    }

    return true ;

}

/////////////////////////////////////////////////////////////////////////////////////////

bool fromXml(const MapFile &mf, const string &file_name, std::vector<FeatureTypeStylePtr> &styles)
{
    return Reader(mf).load(file_name, styles) ;
}



}

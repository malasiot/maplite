#include "ECQLParser.h"

namespace ECQL {

FilterNode *Parser::parse() {
    int res = parser.parse();

    if ( res == 0 ) return node ;
    else {
        delete node ;
        return NULL ;
    }
}

void Parser::error(const ECQL::BisonParser::location_type &loc,
           const std::string& m)
{
    std::stringstream strm ;

    strm << loc << ": " << m ;

    errorString = strm.str() ;
}

///////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

const QString ogcNsUri("http://www.opengis.net/ogc") ;

void NumericLiteral::toSld(QXmlStreamWriter &wr) {
    wr.writeStartElement(ogcNsUri, "Literal") ;
    wr.writeCharacters(QString("%1").arg(val)) ;
    wr.writeEndElement() ;
}

void StringLiteral::toSld(QXmlStreamWriter &wr) {

    wr.writeStartElement(ogcNsUri, "Literal") ;
    wr.writeCharacters(QString::fromStdString(val)) ;
    wr.writeEndElement() ;
}

void BooleanLiteral::toSld(QXmlStreamWriter &wr) {
    std::stringstream s ;
    s << val ;

    wr.writeStartElement(ogcNsUri, "Literal") ;
    if ( val ) wr.writeCharacters("TRUE") ;
    else wr.writeCharacters("FALSE") ;
    wr.writeEndElement() ;
}

void Attribute::toSld(QXmlStreamWriter &wr) {

    wr.writeStartElement(ogcNsUri, "PropertyName") ;
    wr.writeCharacters(QString::fromStdString(val));
    wr.writeEndElement() ;
}

void BinaryOperator::toSld(QXmlStreamWriter &wr)
{
    switch ( op ) {
        case '+':
            wr.writeStartElement(ogcNsUri, "Add") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case '-':
            wr.writeStartElement(ogcNsUri, "Sub") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case '*':
            wr.writeStartElement(ogcNsUri, "Mult") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case '/':
            wr.writeStartElement(ogcNsUri, "Div") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
     }
}

void BooleanOperator::toSld(QXmlStreamWriter &wr)
{
    switch ( op ) {
        case And:
            wr.writeStartElement(ogcNsUri, "And") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Or:
            wr.writeStartElement(ogcNsUri, "Or") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Not:
            wr.writeStartElement(ogcNsUri, "Not") ;
            children[0]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
    }
}

void ComparisonPredicate::toSld(QXmlStreamWriter &wr)
{
    switch ( op ) {
        case Equal:
            wr.writeStartElement(ogcNsUri, "PropertyIsEqualTo") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case NotEqual:
            wr.writeStartElement(ogcNsUri, "PropertyIsNotEqualTo") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Less:
            wr.writeStartElement(ogcNsUri, "PropertyIsLessThan") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Greater:
            wr.writeStartElement(ogcNsUri, "PropertyIsGreaterThan") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case LessOrEqual:
            wr.writeStartElement(ogcNsUri, "PropertyIsLessOrEqual") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case GreaterOrEqual:
            wr.writeStartElement(ogcNsUri, "PropertyIsGreaterOrEqual") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
    }
}

void LikeTextPredicate::toSld(QXmlStreamWriter &wr)
{
    wr.writeStartElement(ogcNsUri, "PropertyIsLike") ;
        children[0]->toSld(wr) ;
        wr.writeStartElement(ogcNsUri, "Literal") ;
            wr.writeCharacters(QString::fromStdString(pattern)) ;
        wr.writeEndElement() ;
    wr.writeEndElement() ;

}

void IsNullPredicate::toSld(QXmlStreamWriter &wr)
{
    if ( isPos )
        wr.writeStartElement(ogcNsUri, "PropertyIsNull") ;
    else
        wr.writeStartElement(ogcNsUri, "PropertyIsNotNull") ;

        children[0]->toSld(wr) ;

    wr.writeEndElement() ;
}

void IncludePredicate::toSld(QXmlStreamWriter &wr)
{
    //??
}

void SpatialPredicate::toSld(QXmlStreamWriter &wr)
{
    switch ( op )
    {
        case Contains:
            wr.writeStartElement(ogcNsUri, "Contains") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Equals:
            wr.writeStartElement(ogcNsUri, "Equals") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Disjoint:
            wr.writeStartElement(ogcNsUri, "Disjoint") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Intersects:
            wr.writeStartElement(ogcNsUri, "Intersects") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Touches:
            wr.writeStartElement(ogcNsUri, "Touches") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Crosses:
            wr.writeStartElement(ogcNsUri, "Crosses") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Within:
            wr.writeStartElement(ogcNsUri, "Within") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Overlaps:
            wr.writeStartElement(ogcNsUri, "Overlaps") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
        case Relate:
            wr.writeStartElement(ogcNsUri, "Relate") ;
            if ( !pattern.empty() ) wr.writeAttribute("pattern", pattern.c_str()) ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
            wr.writeEndElement() ;
            break ;
    }


}

void AttributeExistsPredicate::toSld(QXmlStreamWriter &wr)
{
    if ( isPos )
        wr.writeStartElement(ogcNsUri, "PropertyExists") ;
    else
        wr.writeStartElement(ogcNsUri, "PropertyDoesNotExists") ;
    children[0]->toSld(wr) ;
    wr.writeEndElement() ;
}

void TemporalPredicate::toSld(QXmlStreamWriter &wr)
{
    switch ( op )
    {
        case During:
            wr.writeStartElement(ogcNsUri, "During") ;
            children[0]->toSld(wr) ;
                wr.writeStartElement(ogcNsUri, "Literal") ;
                wr.writeCharacters(date.c_str()) ;
                wr.writeEndElement();
            wr.writeEndElement() ;
            break ;
        case After:
            wr.writeStartElement(ogcNsUri, "After") ;
            children[0]->toSld(wr) ;
                wr.writeStartElement(ogcNsUri, "Literal") ;
                wr.writeCharacters(date.c_str()) ;
                wr.writeEndElement();
            wr.writeEndElement() ;
            break ;
        case Before:
            wr.writeStartElement(ogcNsUri, "Before") ;
            children[0]->toSld(wr) ;
                wr.writeStartElement(ogcNsUri, "Literal") ;
                wr.writeCharacters(date.c_str()) ;
                wr.writeEndElement();
            wr.writeEndElement() ;
            break ;

    }
}

void InPredicate::toSld(QXmlStreamWriter &wr)
{
    wr.writeStartElement(ogcNsUri, "Or") ;

    for(int i=0 ; i<children[1]->children.size() ; i++)
    {
        wr.writeStartElement(ogcNsUri, "PropertyIsEqualTo") ;
        children[0]->toSld(wr) ;

        Literal *lit = (Literal *)children[1]->children[i] ;

        wr.writeStartElement(ogcNsUri, "Literal") ;
        wr.writeCharacters(lit->toString().c_str()) ;
        wr.writeEndElement() ;

        wr.writeEndElement() ;
    }
    wr.writeEndElement() ;
}

void IdPredicate::toSld(QXmlStreamWriter &wr)
{
    for(int i=0 ; i<children[0]->children.size() ; i++)
    {
        wr.writeStartElement(ogcNsUri, "FeatureId") ;

        Literal *lit = (Literal *)children[0]->children[i] ;

        wr.writeAttribute("fid", lit->toString().c_str()) ;
        wr.writeEndElement() ;
    }

}

void RelativeSpatialPredicate::toSld(QXmlStreamWriter &wr)
{
    switch ( op )
    {
        case DWithin:
            wr.writeStartElement(ogcNsUri, "DWithin") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
                wr.writeStartElement(ogcNsUri, "distance") ;
                wr.writeAttribute("units", units.c_str()) ;
                wr.writeCharacters(QString("%1").arg(distance)) ;
                wr.writeEndElement() ;
            wr.writeEndElement() ;
            break ;
        case Beyond:
            wr.writeStartElement(ogcNsUri, "Beyond") ;
            children[0]->toSld(wr) ;
            children[1]->toSld(wr) ;
                wr.writeStartElement(ogcNsUri, "distance") ;
                wr.writeAttribute("units", units.c_str()) ;
                wr.writeCharacters(QString("%1").arg(distance)) ;
                wr.writeEndElement() ;

            wr.writeEndElement() ;
            break ;
    }
}

const QString gmlNsUri = "http://www.opengis.net/gml" ;

void BBox::toSld(QXmlStreamWriter &wr)
{
    wr.writeStartElement(ogcNsUri, "BBOX") ;

    if ( children.size() == 1 )
    {
        children[0]->toSld(wr) ;
        wr.writeStartElement(gmlNsUri, "Box") ;
        if ( !crs.empty() ) wr.writeAttribute("srsName", crs.c_str()) ;
            wr.writeStartElement(gmlNsUri, "coordinates") ;
                wr.writeCharacters(QString("%1,%2,%3,%4").arg(arg1).arg(arg2).arg(arg3).arg(arg4)) ;
            wr.writeEndElement() ;
        wr.writeEndElement() ;
    }
    wr.writeEndElement() ;
}

void Function::toSld(QXmlStreamWriter &wr)
{
    wr.writeStartElement(ogcNsUri, "Function") ;
    wr.writeAttribute("name", name.c_str()) ;
    FilterNode *args = children[0] ;

    std::deque<FilterNode *>::const_iterator it = args->children.begin() ;

    for( ; it != args->children.end() ; ++it )
        (*it)->toSld(wr) ;

    wr.writeEndElement() ;

}

void  PointSequenceNode::toGml(QXmlStreamWriter &wr, GeometryType type)
{

    if ( type == Point )
    {
        wr.writeStartElement(gmlNsUri, "Point");
            wr.writeStartElement(gmlNsUri, "pos") ;
                wr.writeCharacters(QString("%1 %2").arg(pts[0].first).arg(pts[0].second)) ;
            wr.writeEndElement() ;
        wr.writeEndElement() ;
   }
   else if ( type == LineString )
   {
        wr.writeStartElement(gmlNsUri, "LineString");
            wr.writeStartElement(gmlNsUri, "posList") ;
            for(int i=0 ; i<pts.size() ; i++)
            {
                if ( i > 0 ) wr.writeCharacters(" ") ;
                wr.writeCharacters(QString("%1 %2").arg(pts[i].first).arg(pts[i].second)) ;
            }
            wr.writeEndElement() ;
        wr.writeEndElement() ;
   }
   else if ( type == Polygon )
   {
        wr.writeStartElement(gmlNsUri, "Polygon");
            wr.writeStartElement(gmlNsUri, "exterior") ;
                wr.writeStartElement(gmlNsUri, "linearRing") ;
                    wr.writeStartElement(gmlNsUri, "coordinates") ;

                    for(int i=0 ; i<children[0]->pts.size() ; i++)
                    {
                        if ( i > 0 ) wr.writeCharacters(" ") ;
                        wr.writeCharacters(QString("%1,%2").arg(children[0]->pts[i].first).arg(children[0]->pts[i].second)) ;
                    }

                    wr.writeEndElement() ;
                wr.writeEndElement() ;
             wr.writeEndElement() ;

             int n = children.size() ;

             for(int j=1 ; j<n ; j++ )
             {
                 wr.writeStartElement(gmlNsUri, "interior") ;
                     wr.writeStartElement(gmlNsUri, "linearRing") ;
                         wr.writeStartElement(gmlNsUri, "coordinates") ;

                         for(int i=0 ; i<children[j]->pts.size() ; i++)
                         {
                             if ( i > 0 ) wr.writeCharacters(" ") ;
                             wr.writeCharacters(QString("%1,%2").arg(children[j]->pts[i].first).arg(children[j]->pts[i].second)) ;
                         }

                         wr.writeEndElement() ;
                     wr.writeEndElement() ;
                  wr.writeEndElement() ;

             }


         wr.writeEndElement() ;
    }
    else if ( type == Envelope )
    {
         wr.writeStartElement(gmlNsUri, "Envelope");

            wr.writeStartElement(gmlNsUri, "lowerCorner");
            wr.writeCharacters(QString("%1 %2").arg(pts[0].first).arg(pts[0].second)) ;
            wr.writeEndElement() ;

            wr.writeStartElement(gmlNsUri, "upperCorner");
            wr.writeCharacters(QString("%1 %2").arg(pts[1].first).arg(pts[1].second)) ;
            wr.writeEndElement() ;

         wr.writeEndElement() ;

    }
    else if ( type == MultiPoint )
    {
         wr.writeStartElement(gmlNsUri, "MultiPoint");

         for(int i=0 ; i<children.size() ; i++)
         {
             wr.writeStartElement(gmlNsUri, "pointMember") ;
             children[i]->toGml(wr, Point) ;
             wr.writeEndElement() ;
         }

         wr.writeEndElement() ;
    }
    else if ( type == MultiLineString )
    {
         wr.writeStartElement(gmlNsUri, "MultiLineString");

         for(int i=0 ; i<children.size() ; i++)
         {
             wr.writeStartElement(gmlNsUri, "lineStringMember") ;
             children[i]->toGml(wr, LineString) ;
             wr.writeEndElement() ;
         }

          wr.writeEndElement() ;
    }
    else if ( type == MultiPolygon )
    {
         wr.writeStartElement(gmlNsUri, "MultiPolygon");

         for(int i=0 ; i<children.size() ; i++)
         {
             wr.writeStartElement(gmlNsUri, "polygonMember") ;
             children[i]->toGml(wr, Polygon) ;
             wr.writeEndElement() ;
         }

         wr.writeEndElement() ;
    }

}

void GeometryLiteral::toSld(QXmlStreamWriter &wr)
{

    if ( type == GeometryCollection )
    {
        wr.writeStartElement(gmlNsUri, "GeometryCollection");

        for(int i=0 ; i<children.size() ; i++)
        {
            wr.writeStartElement(gmlNsUri, "geometryMember") ;
            children[i]->toSld(wr) ;
            wr.writeEndElement() ;
        }

        wr.writeEndElement() ;
    }
    else  pts->toGml(wr, type) ;



}

void AttributeBetweenPredicate::toSld(QXmlStreamWriter &wr)
{
     wr.writeStartElement(ogcNsUri, "PropertyIsBetween") ;

     children[0]->toSld(wr) ;

        wr.writeStartElement(ogcNsUri, "LowerBoundary") ;
        children[1]->toSld(wr) ;
        wr.writeEndElement() ;

        wr.writeStartElement(ogcNsUri, "UpperBoundary") ;
        children[2]->toSld(wr) ;
        wr.writeEndElement() ;

     wr.writeEndElement() ;

}
} // namespace ECQL

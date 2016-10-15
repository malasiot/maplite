#ifndef __ECQL_PARSER_H__
#define __ECQL_PARSER_H__

#include "ECQLScanner.h"

#include <deque>
#include <string>
#include <vector>
#include <sstream>

#include <QXmlStreamWriter>



namespace ECQL {

class FilterNode ;
class PointSequenceNode ;

class Parser {

public:

    Parser(std::istream &strm) : scanner(strm), parser(*this), node(NULL) {}

    ECQL::FilterNode *parse() ;

    void error(const ECQL::BisonParser::location_type &loc,
                   const std::string& m) ;

    ECQL::FlexScanner scanner;
    ECQL::BisonParser parser;
    ECQL::FilterNode *node ;

    std::string errorString ;
 } ;

enum GeometryType { Point, MultiPoint, LineString, MultiLineString, Polygon, MultiPolygon, GeometryCollection, Envelope } ;

struct PointSequenceNode {
    public:

    PointSequenceNode() {}

    ~PointSequenceNode() {
        std::deque<PointSequenceNode *>::iterator it = children.begin() ;
        for( ; it != children.end() ; ++it ) delete (*it) ;
    }

    void addPoint(float x, float y) {
        pts.push_back(std::pair<float, float>(x, y)) ;
    }

    void appendChild(PointSequenceNode *node) { children.push_back(node) ; }

    void  toGml(QXmlStreamWriter &wr, GeometryType type) ;

    std::vector<std::pair<float, float> > pts ;
    std::deque<PointSequenceNode *> children ;
};



class FilterNode {

    public:

    FilterNode() {}

    FilterNode(FilterNode *child) { appendChild(child) ; }
    FilterNode(FilterNode *a1, FilterNode *a2) {
        appendChild(a1) ;
        appendChild(a2) ;
    }
    FilterNode(FilterNode *a1, FilterNode *a2, FilterNode *a3) {
        appendChild(a1) ;
        appendChild(a2) ;
        appendChild(a3) ;
    }

    virtual ~FilterNode() {
        std::deque<FilterNode *>::iterator it = children.begin() ;
        for( ; it != children.end() ; ++it ) delete (*it) ;
    }

    virtual void toSld(QXmlStreamWriter &) {}

    void appendChild(FilterNode *node) { children.push_back(node) ; }
    void prependChild(FilterNode *node) { children.push_front(node) ; }

    std::deque<FilterNode *> children ;
};

class Literal: public FilterNode {
public:
    Literal() {}

    virtual std::string toString() const = 0;
};


class NumericLiteral: public Literal {
public:

    NumericLiteral(double val_): val(val_) {}

    void toSld(QXmlStreamWriter &) ;

    std::string toString() const {
        std::stringstream strm ;
        strm << val ;
        return strm.str() ;
    }

private:

    double val ;
};

class StringLiteral: public Literal {

public:

    StringLiteral(const std::string val_): val(val_) {}

    void toSld(QXmlStreamWriter &)  ;

    std::string toString() const { return val ; }


private:

    std::string val ;
};

class BooleanLiteral: public Literal {

public:
    BooleanLiteral(bool val_): val(val_) {}

    void toSld(QXmlStreamWriter &) ;

    std::string toString() const {
        return ( val ) ? "TRUE" : "FALSE" ;
    }

private:
    bool val ;
};

class Attribute: public FilterNode {
public:
    Attribute(const std::string val_): val(val_) {}

    void toSld(QXmlStreamWriter &wr) ;

    void prepend(const std::string &p) {
        val = p + '.' + val ;
    }

private:
    std::string val ;
};


class BinaryOperator: public FilterNode {
public:
    BinaryOperator(int op_, FilterNode *op1, FilterNode *op2): op(op_), FilterNode(op1, op2) {}

    void toSld(QXmlStreamWriter &) ;

private:
    int op ;

};

class BooleanOperator: public FilterNode {
public:
    enum Type { And, Or, Not } ;

    BooleanOperator(Type op_, FilterNode *op1, FilterNode *op2): op(op_), FilterNode(op1, op2) {}

    void toSld(QXmlStreamWriter &wr);

private:
    Type op ;

};

class ComparisonPredicate: public FilterNode {
public:
    enum Type { Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } ;

    ComparisonPredicate(Type op_, FilterNode *op1, FilterNode *op2): op(op_), FilterNode(op1, op2) {}

    void toSld(QXmlStreamWriter &wr) ;

private:
    Type op ;

};

class LikeTextPredicate: public FilterNode {
public:

    LikeTextPredicate(FilterNode *arg, const std::string &pattern_, bool is_pos): pattern(pattern_), isPos(is_pos), FilterNode(arg) {}

    void toSld(QXmlStreamWriter &) ;
private:
    std::string pattern ;
    bool isPos ;

};

class IsNullPredicate: public FilterNode {
public:

    IsNullPredicate(FilterNode *arg, bool is_pos):  isPos(is_pos), FilterNode(arg) {}

    void toSld(QXmlStreamWriter &) ;

private:

    bool isPos ;

};

class IncludePredicate: public FilterNode {
public:

    IncludePredicate(bool include_or_exclude):  includeOrExclude(include_or_exclude) { }

    void toSld(QXmlStreamWriter &) ;

private:

    bool includeOrExclude ;

};

class SpatialPredicate: public FilterNode {
public:
    enum Type {  Equals, Disjoint, Intersects, Touches, Crosses, Within, Contains, Overlaps, Relate } ;

    SpatialPredicate(Type op_, FilterNode *arg1, FilterNode *arg2, const std::string &pattern_ = ""):
        op(op_), FilterNode(arg1, arg2), pattern(pattern_) {}

    void toSld(QXmlStreamWriter &) ;

private:

    Type op ;
    std::string pattern ;
};

class AttributeExistsPredicate: public FilterNode {
public:

    AttributeExistsPredicate(FilterNode *arg, bool is_pos):  isPos(is_pos), FilterNode(arg) {}

    void toSld(QXmlStreamWriter &) ;

private:

    bool isPos ;

};

class AttributeBetweenPredicate: public FilterNode {
public:

    AttributeBetweenPredicate(FilterNode *arg1, FilterNode *arg2, FilterNode *arg3, bool is_pos):
        isPos(is_pos), FilterNode(arg1, arg2, arg3) {}

    void toSld(QXmlStreamWriter &) ;

private:

    bool isPos ;

};

class TemporalPredicate: public FilterNode {
public:

    enum Type { Before, BeforeOrDuring, During, DuringOrAfter, After } ;

    TemporalPredicate(Type op_, FilterNode *arg1, const std::string &date_or_duration):
        op(op_), date(date_or_duration), FilterNode(arg1) {}

    void toSld(QXmlStreamWriter &) ;

private:

    Type op ;
    std::string date ;

};

class InPredicate: public FilterNode {
public:

    InPredicate(FilterNode *arg1, FilterNode *arg2, bool is_pos):
        isPos(is_pos), FilterNode(arg1, arg2) {}

    void toSld(QXmlStreamWriter &) ;

private:

  bool isPos ;

};

class IdPredicate: public FilterNode {
public:

    IdPredicate(FilterNode *arg): FilterNode(arg) {}

    void toSld(QXmlStreamWriter &) ;
};

class RelativeSpatialPredicate: public FilterNode {
public:
    enum Type { DWithin, Beyond } ;

    RelativeSpatialPredicate(Type op_, FilterNode *arg1, FilterNode *arg2,
                                   double dist, const std::string &un):
        op(op_), distance(dist), units(un), FilterNode(arg1, arg2) {}

    void toSld(QXmlStreamWriter &) ;
private:

    std::string units ;
    double distance ;
    Type op ;

};

class BBox: public FilterNode {
public:

    BBox(FilterNode *arg, double arg1_, double arg2_,
                                   double arg3_, double arg4_, const std::string crs_ = ""):
            arg1(arg1_), arg2(arg2_), arg3(arg3_), arg4(arg4_), crs(crs_), FilterNode(arg)  {}

    BBox(FilterNode *arg1_, FilterNode *arg2_): FilterNode(arg1_, arg2_) {}

    void toSld(QXmlStreamWriter &) ;

private:
    double arg1, arg2, arg3, arg4 ;
    std::string crs ;

};


class Function: public FilterNode {
public:
    Function(const std::string &name_): name(name_) {}
    Function(const std::string &name_, FilterNode *args): name(name_), FilterNode(args) {}

    void toSld(QXmlStreamWriter &) ;

private:
    std::string name ;

};

class GeometryLiteral: public FilterNode {
public:

    GeometryLiteral( GeometryType type_, PointSequenceNode *pts_): type(type_), pts(pts_) { }

    void toSld(QXmlStreamWriter &) ;

private:
    GeometryType type ;
    PointSequenceNode *pts ;

};

} // namespace ECQL

#endif

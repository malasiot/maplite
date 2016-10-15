#ifndef __FILTER_H__
#define __FILTER_H__

#include <deque>
#include <string>
#include <vector>
#include <sstream>

class PointSequenceNode ;

namespace ECQL {

class Node {

    public:

    Node() {}

    virtual ~Node() {
        std::deque<Node *>::iterator it = children.begin() ;
        for( ; it != children.end() ; ++it ) delete (*it) ;
    }

    virtual std::string toSld() { return "" ; }

    void appendChild(Node *node) { children.push_back(node) ; }
    void prependChild(Node *node) { children.push_front(node) ; }

    std::deque<Node *> children ;
};


class NumericLiteral: public Node {
    public:

    NumericLiteral(double val_) {
        val = val_ ;
    }

    std::string toSld() {
        std::stringstream s ;
        s << val ;
        return "<ogc:Literal>" + s.str() + "</ogc:Literal>" ;
    }

    double val ;
};

class StringLiteral: public Node {

public:

    StringLiteral(const std::string val_): Node() {
        val = val_ ;
    }

    std::string toSld() {
        return "<ogc:Literal>" + val + "</ogc:Literal>" ;
    }

private:
    std::string val ;
};

class BooleanLiteral: public Node {

public:
    BooleanLiteral(bool val_) {
        val = val_ ;
    }

    std::string toSld() {
        return (val) ? "<ogc:Literal>TRUE</ogc:Literal>" : "<ogc:Literal>FALSE</ogc:Literal>" ;
    }
private:
    bool val ;
};

class Attribute: public Node {
public:
    Attribute(const std::string val_) {
        val = val_ ;
    }

    std::string toSld() {
        return "<ogc:PropertyName>" + val + "</ogc:PropertyName>" ;
    }

    void prepend(const std::string &p) {
        val = p + '.' + val ;
    }

private:
    std::string val ;
};


class BinaryOperator: public Node {
public:
    BinaryOperator(int op_, Node *op1, Node *op2): op(op_) {
        children.push_back(op1) ;
        children.push_back(op2) ;
    }

        std::string toSld() {
            switch ( op ) {
                case '+':
                    return "<ogc:Add>" + children[0]->toSld() + children[1]->toSld() + "</ogc:Add>"  ;
                case '-':
                    return "<ogc:Sub>" + children[0]->toSld() + children[1]->toSld() + "</ogc:Sub>"  ;
                case '*':
                    return "<ogc:Mult>" + children[0]->toSld() + children[1]->toSld() + "</ogc:Mult>" ;
                case '/':
                    return "<ogc:Div>" + children[0]->toSld() + children[1]->toSld() + "</ogc:Div>" ;
            }
    }

private:
    int op ;

};

class BooleanOperator: public Node {
public:
    enum Type { And, Or, Not } ;

    BooleanOperator(Type op_, Node *op1, Node *op2): op(op_) {
        children.push_back(op1) ;
        children.push_back(op2) ;
    }

    std::string toSld() {
        switch ( op ) {
            case And:
                return "<ogc:And>" + children[0]->toSld() + children[1]->toSld() + "</ogc:And>"  ;
            case Or:
                return "<ogc:Or>" + children[0]->toSld() + children[1]->toSld() + "</ogc:Or>"  ;
             case Not:
                return "<ogc:Not>" + children[0]->toSld() + "</ogc:Not>" ;
        }


    }

private:
    Type op ;

};

class ComparisonPredicate: public Node {
public:
    enum Type { Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } ;

    ComparisonPredicate(Type op_, Node *op1, Node *op2): op(op_) {
        children.push_back(op1) ;
        children.push_back(op2) ;
    }

    std::string toSld() {
        switch ( op ) {
            case Equal:
                return "<ogc:PropertyIsEqualTo>" + children[0]->toSld() + children[1]->toSld() + "</ogc:PropertyIsEqualTo>"  ;
            case NotEqual:
                return "<ogc:PropertyIsNotEqualTo>" + children[0]->toSld() + children[1]->toSld() + "</ogc:PropertyIsNotEqualTo>"  ;
             case Less:
                return "<ogc:PropertyIsLessThan>" + children[0]->toSld() + children[1]->toSld() + "</ogc:PropertyIsLessThan>" ;
        case Greater:
           return "<ogc:PropertyIsGreaterThan>" + children[0]->toSld() + children[1]->toSld() + "</ogc:PropertyIsGreaterThan>" ;
        }
    }

private:
    Type op ;

};

class LikeTextPredicate: public Node {
public:

    LikeTextPredicate(Node *arg, const std::string &pattern_, bool is_pos): pattern(pattern_), isPos(is_pos) {
        children.push_back(arg) ;

    }

private:
    std::string pattern ;
    bool isPos ;

};

class IsNullPredicate: public Node {
public:

    IsNullPredicate(Node *arg, bool is_pos):  isPos(is_pos) {
        children.push_back(arg) ;

    }

private:

    bool isPos ;

};

class IncludePredicate: public Node {
public:

    IncludePredicate(bool include_or_exclude):  includeOrExclude(include_or_exclude) {
    }

private:

    bool includeOrExclude ;

};

class SpatialPredicate: public Node {
public:
    enum Type {  Equals, Disjoint, Intersects, Touches, Crosses, Within, Contains, Overlaps, Relate } ;

    SpatialPredicate(Type op_, Node *arg1, Node *arg2, const std::string &pattern_ = ""): op(op_) {
        children.push_back(arg1) ;
        children.push_back(arg2) ;
    }

    Type op ;
    std::string pattern_ ;
};

class AttributeExistsPredicate: public Node {
public:

    AttributeExistsPredicate(Node *arg, bool is_pos):  isPos(is_pos) {
        children.push_back(arg) ;

    }

private:

    bool isPos ;

};

class AttributeBetweenPredicate: public Node {
public:

    AttributeBetweenPredicate(Node *arg1, Node *arg2, Node *arg3, bool is_pos):  isPos(is_pos) {
        children.push_back(arg1) ;
        children.push_back(arg2) ;
        children.push_back(arg3) ;

    }

private:

    bool isPos ;

};

class TemporalPredicate: public Node {
public:

    enum Type { Before, BeforeOrDuring, During, DuringOrAfter, After } ;

    TemporalPredicate(Type op_, Node *arg1, const std::string &date_or_duration): op(op_), date(date_or_duration) {
        children.push_back(arg1) ;

    }

private:

    Type op ;
    std::string date ;

};

class InPredicate: public Node {
public:

    InPredicate(Node *arg1, Node *arg2, bool is_pos): isPos(is_pos) {
        children.push_back(arg1) ;
        children.push_back(arg2) ;
    }

private:

  bool isPos ;

};

class IdPredicate: public Node {
public:

    IdPredicate(Node *arg) {
        children.push_back(arg) ;
    }
};

class RelativeSpatialPredicate: public Node {
public:
    enum Type { DWithin, Beyond } ;

    RelativeSpatialPredicate(Type op_, Node *arg, Node *arg1, Node *arg2,
                                   double dist, const std::string &un):
        op(op_), distance(dist), units(un)
    {
        children.push_back(arg) ;
        children.push_back(arg1) ;
        children.push_back(arg2) ;
    }

private:

    std::string units ;
    double distance ;
    Type op ;

};

class BBox: public Node {
public:

    BBox(Node *arg, double arg1_, double arg2_,
                                   double arg3_, double arg4_, const std::string crs_ = ""):
            arg1(arg1_), arg2(arg2_), arg3(arg3_), arg4(arg4_), crs(crs_)  {
        children.push_back(arg) ;
    }

    BBox(Node *arg1_, Node *arg2_) {
        children.push_back(arg1_) ;
        children.push_back(arg2_) ;
    }



private:
    double arg1, arg2, arg3, arg4 ;
    std::string crs ;

};


class Function: public Node {
public:
    Function(const std::string &name_): name(name_) {}
    Function(const std::string &name_, Node *args): name(name_) {
        children.push_back(args) ;

    }

    std::string name ;

};

class GeometryLiteral: public Node {
public:
    enum Type { Point, MultiPoint, LineString, MultiLineString, Polygon, MultiPolygon, GeometryCollection, Envelope } ;

    GeometryLiteral( Type type_, PointSequenceNode *pts_): type(type_), pts(pts_) {

    }

    Type type ;
    PointSequenceNode *pts ;

};

}

#endif

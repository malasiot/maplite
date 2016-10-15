#include "Style.h"

#include "XmlDocument.h"

#include <sstream>

using namespace std ;

namespace sld {

class ExpressionNode ;

class ExpressionNode {

    public:

    ExpressionNode() {}

    ExpressionNode(ExpressionNode *child) { appendChild(child) ; }
    ExpressionNode(ExpressionNode *a1, ExpressionNode *a2) {
        appendChild(a1) ;
        appendChild(a2) ;
    }
    ExpressionNode(ExpressionNode *a1, ExpressionNode *a2, ExpressionNode *a3) {
        appendChild(a1) ;
        appendChild(a2) ;
        appendChild(a3) ;
    }

    ExpressionNode(const vector<ExpressionNode *> &op) {

        std::copy(op.begin(), op.end(), std::back_inserter(children)) ;
    }

    virtual ~ExpressionNode() {
        std::deque<ExpressionNode *>::iterator it = children.begin() ;
        for( ; it != children.end() ; ++it ) delete (*it) ;
    }

    virtual string toSQL() const = 0 ;
    virtual Value eval(const Feature &) const = 0 ;

    void appendChild(ExpressionNode *node) { children.push_back(node) ; }
    void prependChild(ExpressionNode *node) { children.push_front(node) ; }

    std::deque<ExpressionNode *> children ;
};

class Literal: public ExpressionNode {
public:
    Literal() {}

    virtual std::string toString() const = 0;
};


class NumericLiteral: public Literal {
public:

    NumericLiteral(double val_): val(val_) {}

    string toSQL() ;

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

    string toSQL() const {
        return '\'' + val + '\'' ; //? needs escaping
    }

    std::string toString() const { return val ; }

    Value eval(const Feature &) const {
        return val ;
    }

private:

    std::string val ;
};

class BooleanLiteral: public Literal {

public:
    BooleanLiteral(bool val_): val(val_) {}

    string toSQL() const { return toString() ; }

    std::string toString() const {
        return ( val ) ? "TRUE" : "FALSE" ;
    }

private:
    bool val ;
};

class Attribute: public ExpressionNode {
public:
    Attribute(const std::string val_): val(val_) {}

    string toSQL() const { return '"' + val + "_\""; }
    Value eval(const Feature &f) const {
        return f.attribute(val) ;
    }

private:
    std::string val ;
};


class BinaryOperator: public ExpressionNode {
public:
    BinaryOperator(int op_, ExpressionNode *op1, ExpressionNode *op2): op(op_), ExpressionNode(op1, op2) {}

    string toSQL() const {
        switch( op )
        {
        case '+':
            return '(' + children[0]->toSQL() + '+' + children[1]->toSQL() + ')' ;
        case '-':
            return '(' + children[0]->toSQL() + '-' + children[1]->toSQL() + ')' ;
        case '*':
            return '(' + children[0]->toSQL() + '*' + children[1]->toSQL() + ')' ;
        case '%':
            return '(' + children[0]->toSQL() + '/' + children[1]->toSQL() + ')' ;
        }
    }

    Value eval(const Feature &f) const {
        Value v1 = children[0]->eval(f) ;
        Value v2 = children[1]->eval(f) ;

        double n1, n2 ;
        if ( !v1.toNumber(n1) ) return Value() ;
        if ( !v2.toNumber(n2) ) return Value() ;

        switch( op )
        {
        case '+':
            return n1 + n2 ;
        case '-':
            return n1 - n2 ;
        case '*':
            return n1 * n2 ;
        case '%':
            return n1 / n2 ;
        }


    }

private:
    int op ;

};

class BooleanOperator: public ExpressionNode {
public:
    enum Type { And, Or, Not } ;

    BooleanOperator(Type op_, ExpressionNode *op1, ExpressionNode *op2): op(op_), ExpressionNode(op1, op2) {}
    BooleanOperator(Type op_, const vector<ExpressionNode *> &operants): op(op_), ExpressionNode(operants) {}

    string toSQL () const {

        if ( op == Not )
            return "NOT (" + children[0]->toSQL() + ")" ;
        else {
            string res = "(" ;

            for(int i=0 ; i<children.size() ; i++ ) {
                if ( i>0 ) {
                    if ( op == And ) res += " AND " ;
                    else res += " OR " ;
                }
                res += children[i]->toSQL() ;
            }
            res += ')' ;

            return res ;
        }
    }

    Value eval(const Feature &f) const {
        Value v1 = children[0]->eval(f) ;
        Value v2 = children[1]->eval(f) ;

        bool b1, b2 ;

        if ( !v1.toBoolean(b1)) return Value() ;
        if ( !v2.toBoolean(b2)) return Value() ;

        switch (op)
        {
        case And:
        {
            Value v1 = children[0]->eval(f) ;
            Value v2 = children[1]->eval(f) ;

            if ( !v1.toBoolean(b1)) return Value() ;
            if ( !v2.toBoolean(b2)) return Value() ;

            return b1 && b2 ;
        }
        case Or:
        {
            Value v1 = children[0]->eval(f) ;
            Value v2 = children[1]->eval(f) ;

            if ( !v1.toBoolean(b1)) return Value() ;
            if ( !v2.toBoolean(b2)) return Value() ;

            return b1 || b2 ;
        }
        case Not:
        {
            Value v1 = children[0]->eval(f) ;

            if ( !v1.toBoolean(b1)) return Value() ;

            return !b1 ;
        }
        }
    }

private:
    Type op ;

};

class ComparisonPredicate: public ExpressionNode {
public:
    enum Type { Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } ;

    ComparisonPredicate(Type op_, ExpressionNode *op1, ExpressionNode *op2): op(op_), ExpressionNode(op1, op2) {}

    string toSQL() const {
        switch ( op )
        {
        case Equal:
            return '(' + children[0]->toSQL() + '=' + children[1]->toSQL() + ')' ;
        case NotEqual:
            return '(' + children[0]->toSQL() + "!=" + children[1]->toSQL() + ')' ;
        case Less:
            return '(' + children[0]->toSQL() + '<' + children[1]->toSQL() + ')' ;
        case Greater:
            return '(' + children[0]->toSQL() + '>' + children[1]->toSQL() + ')' ;
        case LessOrEqual:
            return '(' + children[0]->toSQL() + "<=" + children[1]->toSQL() + ')' ;
        case GreaterOrEqual:
            return '(' + children[0]->toSQL() + ">=" + children[1]->toSQL() + ')' ;
        }
    }

    Value eval(const Feature &) const {
        return Value() ; //? TODO




    }

private:
    Type op ;

};

class LikeTextPredicate: public ExpressionNode {
public:

    LikeTextPredicate(ExpressionNode *arg, const std::string &pattern_, const string &wildcard_, const string &single_char_, const string &escape_char_):
        pattern(pattern_), wildcard(wildcard_), singleChar(single_char_), escapeChar(escape_char_), ExpressionNode(arg) {}

    //?
    string toSQL() const {
        return children[0]->toSQL() + " LIKE '" + pattern + "'" ;
    }

    Value eval(const Feature &) const {
        return Value() ; //? TODO
    }

private:
    std::string pattern, wildcard, singleChar, escapeChar ;

};

class IsNullPredicate: public ExpressionNode {
public:

    IsNullPredicate(ExpressionNode *arg): ExpressionNode(arg) {}

    string toSQL() const { return children[0]->toSQL() + " ISNULL" ; }
    Value eval(const Feature &v) const {
        return children[0]->eval(v).isNull() ;
    }
};

class IncludePredicate: public ExpressionNode {
public:

    IncludePredicate(bool include_or_exclude):  includeOrExclude(include_or_exclude) { }

    string toSQL(vector<string> &) const {
        return string();
    }

private:

    bool includeOrExclude ;

};

class SpatialPredicate: public ExpressionNode {
public:
    enum Type {  Equals, Disjoint, Intersects, Touches, Crosses, Within, Contains, Overlaps, Relate } ;

    SpatialPredicate(Type op_, ExpressionNode *arg1, ExpressionNode *arg2, const std::string &pattern_ = ""):
        op(op_), ExpressionNode(arg1, arg2), pattern(pattern_) {}

    string toSQL(vector<string> &) const {
        return string();
    }

private:

    Type op ;
    std::string pattern ;
};

class AttributeExistsPredicate: public ExpressionNode {
public:

    AttributeExistsPredicate(ExpressionNode *arg): ExpressionNode(arg) {}

    string toSQL(vector<string> &) const {
        return string() ;
    }

private:

};

class AttributeBetweenPredicate: public ExpressionNode {
public:

    AttributeBetweenPredicate(ExpressionNode *arg1, ExpressionNode *arg2, ExpressionNode *arg3, bool is_pos):
        isPos(is_pos), ExpressionNode(arg1, arg2, arg3) {}

    string toSQL(vector<string> &) const ;

private:

    bool isPos ;

};

class TemporalPredicate: public ExpressionNode {
public:

    enum Type { Before, BeforeOrDuring, During, DuringOrAfter, After } ;

    TemporalPredicate(Type op_, ExpressionNode *arg1, const std::string &date_or_duration):
        op(op_), date(date_or_duration), ExpressionNode(arg1) {}

    string toSQL(vector<string> &) const { return "" ; }

private:

    Type op ;
    std::string date ;

};

class InPredicate: public ExpressionNode {
public:

    InPredicate(ExpressionNode *arg1, ExpressionNode *arg2, bool is_pos):
        isPos(is_pos), ExpressionNode(arg1, arg2) {}

    string toSQL(vector<string> &) const ;

private:

  bool isPos ;

};

class IdPredicate: public ExpressionNode {
public:

    IdPredicate(ExpressionNode *arg): ExpressionNode(arg) {}

    string toSQL(vector<string> &) const ;
};

class RelativeSpatialPredicate: public ExpressionNode {
public:
    enum Type { DWithin, Beyond } ;

    RelativeSpatialPredicate(Type op_, ExpressionNode *arg1, ExpressionNode *arg2,
                                   double dist, const std::string &un):
        op(op_), distance(dist), units(un), ExpressionNode(arg1, arg2) {}

    string toSQL() const ;
private:

    std::string units ;
    double distance ;
    Type op ;

};

class BBox: public ExpressionNode {
public:

    BBox(ExpressionNode *arg, double arg1_, double arg2_,
                                   double arg3_, double arg4_, const std::string crs_ = ""):
            arg1(arg1_), arg2(arg2_), arg3(arg3_), arg4(arg4_), crs(crs_), ExpressionNode(arg)  {}

    BBox(ExpressionNode *arg1_, ExpressionNode *arg2_): ExpressionNode(arg1_, arg2_) {}

    string toSQL() const ;

private:
    double arg1, arg2, arg3, arg4 ;
    std::string crs ;

};


class Function: public ExpressionNode {
public:
    Function(const std::string &name_): name(name_) {}
    Function(const std::string &name_, ExpressionNode *args): name(name_), ExpressionNode(args) {}

    string toSQL() const {
        string res = name + "(" ;

        for( int i=0 ; i<children.size() ; i++ )
        {
            if ( i > 0 ) res += ',' ;
            res += children[i]->toSQL() ;
        }

        res += ')' ;

        return res ;
    }

    Value eval(const Feature &) const {
        return Value() ; //? TODO




    }

private:
    std::string name ;

};

class GeometryLiteral: public ExpressionNode {
public:

    GeometryLiteral( const string &geom_str): geom(geom_str) { }

    string toSQL(vector<string> &) const { return "" ; }

private:
    string type ;
    string geom ;

};

class ExpressionParser {

public:


    ExpressionNode *parseAttribute(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        if ( pNode->tag() == ns_ogc + "PropertyName" )
            return new Attribute(pNode->text()) ;
        else return 0 ;
    }

    ExpressionNode *parseLiteral(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        if ( pNode->tag() == ns_ogc + "Literal" )
            return new StringLiteral(pNode->text()) ;
        else return 0 ;
    }

    ExpressionNode *parseFunction(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        if ( pNode->tag() == ns_ogc + "Function" )
        {
            string name = pNode->attribute("name") ;
            if ( name.empty() ) return 0 ;

            Function *node = new Function(name) ;

            XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
            {
                ExpressionNode *arg ;
                if ( arg = parseExpression(qNode) ) {
                    node->appendChild(arg);
                }
                else {
                    delete node ;
                    return 0 ;
                }
            }

            return node ;
        }
        else return 0 ;
    }


    ExpressionNode *parseFactor(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        ExpressionNode *n ;
        if ( n = parseLiteral(pNode) ) return n ;
        else if ( n = parseAttribute(pNode) ) return n ;
        else if ( n = parseFunction(pNode) ) return n ;
        else return 0 ;
    }

    ExpressionNode *parseTerm(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        int op ;

        if ( pNode->tag() == ns_ogc + "Mul" )
            op = '*' ;
        else if ( pNode->tag() == ns_ogc + "Div" )
            op = '%' ;
        else
            return parseFactor(pNode) ;

        const XmlElement *lhs = pNode->firstChildElement() ;
        const XmlElement *rhs = lhs->nextSiblingElement() ;

        ExpressionNode *n1 = parseFactor(lhs) ;
        ExpressionNode *n2 = parseTerm(rhs) ;

        return ( n1 && n2 ) ? new BinaryOperator(op, n1, n2) : 0 ;
    }

    ExpressionNode *parseExpression(const XmlElement *pNode)
    {
        ns_ogc = pNode->resolveNamespace("http://www.opengis.net/ogc") ;

        if ( !pNode ) return 0 ;

        int op ;

        if ( pNode->tag() == ns_ogc + "Add" )
            op = '+' ;
        else if ( pNode->tag() == ns_ogc + "Sub" )
            op = '-' ;
        else
            return parseTerm(pNode) ;

        const XmlElement *lhs = pNode->firstChildElement() ;
        const XmlElement *rhs = lhs->nextSiblingElement() ;

        ExpressionNode *n1 = parseTerm(lhs) ;
        ExpressionNode *n2 = parseExpression(rhs) ;

        return ( n1 && n2 ) ? new BinaryOperator(op, n1, n2) : 0 ;
    }


    ExpressionNode *parseComparisonPredicate(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        ComparisonPredicate::Type op ;

        if ( pNode->tag() == ns_ogc + "PropertyIsEqualTo" )
            op = ComparisonPredicate::Equal ;
        else if ( pNode->tag() == ns_ogc + "PropertyIsNotEqualTo" )
            op = ComparisonPredicate::NotEqual ;
        else if ( pNode->tag() == ns_ogc + "PropertyIsLessThan" )
            op = ComparisonPredicate::Less ;
        else if ( pNode->tag() == ns_ogc + "PropertyIsGreaterThan" )
            op = ComparisonPredicate::Greater ;
        else if ( pNode->tag() == ns_ogc + "PropertyIsLessThanOrEqualTo" )
            op = ComparisonPredicate::LessOrEqual ;
        else if ( pNode->tag() == ns_ogc + "PropertyIsGreaterThanOrEqualTo" )
            op = ComparisonPredicate::GreaterOrEqual ;
        else return 0 ;

        const XmlElement *lhs = pNode->firstChildElement() ;
        const XmlElement *rhs = lhs->nextSiblingElement() ;

        ExpressionNode *n1 = parseExpression(lhs) ;
        ExpressionNode *n2 = parseExpression(rhs) ;

        return (n1 && n2 ) ? new ComparisonPredicate(op, n1, n2) : 0;

    }


    ExpressionNode *parseLikeTextPredicate(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        if ( pNode->tag() == ns_ogc + "PropertyIsLike" )
        {
            const XmlElement *lhs = pNode->firstChildElement(ns_ogc + "PropertyName") ;
            const XmlElement *rhs = lhs->nextSiblingElement(ns_ogc + "Literal") ;

            if ( !rhs ) return 0 ;

            ExpressionNode *c = parseAttribute(lhs) ;

            return (c) ? new LikeTextPredicate(c, rhs->text(),
                                         pNode->attribute("wildcard"),
                                         pNode->attribute("singleChar"),
                                         pNode->attribute("escapeChar")
                                         ) : 0;
        }
        else return 0 ;
    }

    ExpressionNode *parseIsNullPredicate(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        if ( pNode->tag() == ns_ogc + "PropertyIsNull" )
        {
            const XmlElement *lhs = pNode->firstChildElement(ns_ogc + "PropertyName") ;
            ExpressionNode *c = parseAttribute(lhs) ;
            return (c) ? new IsNullPredicate(c) : 0;
        }
        else return 0 ;
     }

    ExpressionNode *parsePredicate(const XmlElement *pNode)
    {
        if ( !pNode ) return 0 ;

        ExpressionNode *n ;

        if ( n = parseComparisonPredicate(pNode) ) return n ;
        else if ( n = parseLikeTextPredicate(pNode) ) return n ;
        else if ( n = parseIsNullPredicate(pNode) ) return n ;
        else return 0 ;


    }

    ExpressionNode *parseBooleanValueExpression(const XmlElement *pNode)
    {
        ns_ogc = pNode->resolveNamespace("http://www.opengis.net/ogc") ;

        if ( !pNode ) return 0 ;

        if ( pNode->tag() == ns_ogc + "Or" ||
             pNode->tag() == ns_ogc + "And" )
        {
            vector<ExpressionNode *> nodes ;

            XML_FOREACH_CHILD_ELEMENT(pNode, qNode)
            {
                ExpressionNode *n1 = parsePredicate(qNode) ;

                if ( n1 ) nodes.push_back(n1) ;
                else {
                    n1 = parseBooleanValueExpression(qNode) ;
                    if ( n1 ) nodes.push_back(n1) ;
                    else {
                        for(int i=0 ; i<nodes.size() ; i++ ) {
                            delete nodes[i] ;
                            return 0 ;
                        }
                    }
                }
            }

            BooleanOperator::Type op ;

            if ( pNode->tag() == ns_ogc + "Or" ) op = BooleanOperator::Or ;
            else op = BooleanOperator::And ;

            return ( nodes.size() >= 2 ) ? new BooleanOperator(op, nodes) : 0 ;
        }
        else if ( pNode->tag() == ns_ogc + "Not" )
        {
            const XmlElement *child = pNode->firstChildElement() ;

            ExpressionNode *op = parsePredicate(child) ;

            return ( op ) ? new BooleanOperator(BooleanOperator::Not, op, 0) : op ;
        }
        else
            return parsePredicate(pNode) ;
    }

     string ns_ogc, ns_gml ;
};

/////////////////////////////////////////////////////////////////////////////

bool FilterExpression::fromXml(const XmlElement *pNode, const ParseContext *)
{
    ExpressionParser parser ;

    ExpressionNode *n = parser.parseBooleanValueExpression(pNode->firstChildElement()) ;

    if ( !n ) return false ;

    node_.reset(n) ;
    return true ;
}


string FilterExpression::toSQL() const {
    return node_->toSQL() ;
}


//////////////////////////////////////////////////////////////////////

extern const char *NS_SLD  ;
extern const char *NS_OGC  ;

bool ParameterValue::fromXml(const XmlElement *pNode, const ParseContext *)
{
    string nsPrefixSLD = pNode->resolveNamespace(NS_SLD) ;

    XML_FOREACH_CHILD_NODE(pNode, qNode)
    {
        if ( qNode->nodeType() == XmlNode::TextNode )
        {
            StringLiteral *n = new StringLiteral(qNode->nodeValue()) ;
            mixedExpression.push_back(n) ;
        }
        else if ( qNode->nodeType() == XmlNode::ElementNode )
        {
            ExpressionParser parser ;

            ExpressionNode *node = parser.parseExpression(qNode->toElement()) ;

            if ( !node ) return false ;

            mixedExpression.push_back(node) ;
        }

    }

    return true ;
}

ParameterValue::~ParameterValue()
{
    for( int i=0 ; i<mixedExpression.size() ; i++ )
        delete mixedExpression[i] ;
}

Value ParameterValue::eval(const Feature &f) const
{
    if ( mixedExpression.empty() ) return Value() ;
    else if ( mixedExpression.size() == 1 )
        return mixedExpression[0]->eval(f) ;
    else
    {
        string res ;

        for(int i=0 ; i<mixedExpression.size() ; i++ )
        {
            if ( i > 0 ) res += " " ;

            Value v = mixedExpression[i]->eval(f) ;
            if ( v.isNull() )
                res += "[NULL]" ;
            else {
                string s ;
                if ( v.toString(s) ) res += s ;
                else res += "[NULL]" ;
            }
        }

        return Value(res) ;
    }
}

}

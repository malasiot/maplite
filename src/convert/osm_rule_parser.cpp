#include "osm_rule_scanner.hpp"
#include "osm_rule_parser.hpp"

#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <errno.h>
using namespace std;

namespace OSM {
namespace Filter {

Parser::Parser(std::istream &strm) :
    scanner_(strm),
    parser_(*this, loc_)
{}

bool Parser::parse() {
 //      parser_.set_debug_level(2);

    loc_.initialize() ;
    int res = parser_.parse();

    return ( res == 0 ) ;
}

void Parser::error(const OSM::BisonParser::location_type &loc,
                   const std::string& m)
{
    std::stringstream strm ;


    strm << m << loc ;
    error_string_ = strm.str() ;
}
//////////////////////////////////////////////////////////////////



Literal::Literal(const std::string &val, bool auto_conv)
{
    if ( auto_conv ) {
        char * e;
        double x = std::strtod(val.c_str(), &e);

        if (*e != 0 ||  errno != 0 )  {
            type_ = String ;
            string_val_ = val ;
        }
        else
        {
            type_ = Number ;
            number_val_ = x ;
        }
    }
    else {
        type_ = String ;
        string_val_ = val ;
    }

}

bool Literal::toBoolean() const {

    switch ( type_ ) {
    case Null: return false ;
    case Boolean: return boolean_val_ ;
    case Number: return number_val_ != 0.0 ;
    case String: return ( string_val_ != "0" && string_val_.empty()) ;
    }
}

double Literal::toNumber() const {

    switch ( type_ ) {
    case Null: return 0 ;
    case Boolean: return (double)boolean_val_ ;
    case Number: return number_val_ ;
    case String: return atof(string_val_.c_str()) ;
    }
}

string Literal::toString() const {
    switch ( type_ ) {
    case Null: return "" ;
    case Boolean: ( boolean_val_ ) ? "TRUE" : "FALSE" ;
    case Number: return str(boost::format("%f") % number_val_) ;
    case String: return string_val_ ;
    }
}


Literal BooleanOperator::eval(Context &ctx)
{
    switch ( op ) {
    case And:
        return ( children_[0]->eval(ctx).toBoolean() && children_[1]->eval(ctx).toBoolean() ) ;
    case Or:
        return ( children_[0]->eval(ctx).toBoolean() || children_[1]->eval(ctx).toBoolean() ) ;
    case Not:
        return !( children_[0]->eval(ctx).toBoolean() ) ;
    }
}

Literal ComparisonPredicate::eval(Context &ctx)
{

    Literal lhs = children_[0]->eval(ctx) ;
    Literal rhs = children_[1]->eval(ctx) ;

    if ( lhs.isNull() || rhs.isNull() ) return false ;

    switch ( op_ ) {
    case Equal:
    {
        if ( lhs.type_ == Literal::String && lhs.type_ == Literal::String )
            return Literal(lhs.string_val_ == rhs.string_val_) ;
        else return Literal(lhs.toNumber() == rhs.toNumber()) ;
    }
    case NotEqual:
        if ( lhs.type_ == Literal::String && lhs.type_ == Literal::String )
            return Literal(lhs.string_val_ != rhs.string_val_) ;
        else return Literal(lhs.toNumber() != rhs.toNumber()) ;
    case Less:
        return lhs.toNumber() < rhs.toNumber() ;
    case Greater:
        return lhs.toNumber() > rhs.toNumber() ;
    case LessOrEqual:
        return lhs.toNumber() <= rhs.toNumber() ;
    case GreaterOrEqual:
        return lhs.toNumber() >= rhs.toNumber() ;
    }

    return Literal() ;
}
static string globToRegex(const char *pat)
{
    // Convert pattern
    string rx = "(?i)^", be ;

    int i = 0;
    const char *pc = pat ;
    int clen = strlen(pat) ;
    bool inCharClass = false ;

    while (i < clen)
    {
        char c = pc[i++];

        switch (c)
        {
        case '*':
            rx += "[^\\\\/]*" ;
            break;
        case '?':
            rx += "[^\\\\/]" ;
            break;
        case '$':  //Regex special characters
        case '(':
        case ')':
        case '+':
        case '.':
        case '|':
            rx += '\\';
            rx += c;
            break;
        case '\\':
            if ( pc[i] == '*' ) rx += "\\*" ;
            else if ( pc[i] == '?' )  rx += "\\?" ;
            ++i ;
            break ;
        case '[':
        {
            if ( inCharClass )  rx += "\\[";
            else {
                inCharClass = true ;
                be += c ;
            }
            break ;
        }
        case ']':
        {
            if ( inCharClass ) {
                inCharClass = false ;
                rx += be ;
                rx += c ;
                rx += "{1}" ;
                be.clear() ;
            }
            else rx += "\\]" ;

            break ;
        }
        case '%':
        {
            boost::regex rd("(0\\d)?d") ;
            boost::smatch what;

            if ( boost::regex_match(std::string(pat + i), what, rd,  boost::match_extra) )
            {

                rx += "[[:digit:]]" ;

                if ( what.size() == 2 )
                {
                    rx +=  "{" ;
                    rx += what[1] ;
                    rx += "}" ;
                }
                else
                    rx += "+" ;

                i += what.size() ;
            }
            else
            {
                if ( inCharClass ) be += c ;
                else rx += c;
            }
            break ;

        }
        default:
            if ( inCharClass ) be += c ;
            else rx += c;
        }
    }

    rx += "$" ;
    return rx ;
}

LikeTextPredicate::LikeTextPredicate(ExpressionNodePtr op, const std::string &pattern, bool is_pos):
    ExpressionNode(op), is_pos_(is_pos)
{
    if ( !pattern.empty() )
        pattern_ = std::regex(globToRegex(pattern.c_str())) ;

}


Literal LikeTextPredicate::eval(Context &ctx)
{
    Literal op = children_[0]->eval(ctx) ;

    return std::regex_match(op.toString(), pattern_) ;

}

ListPredicate::ListPredicate(const string &id, ExpressionNodePtr op, bool is_pos):
    id_(id), ExpressionNode(op), is_pos_(is_pos)
{
    Context ctx ;

    for(int i=0 ; i<children_[0]->children_.size() ; i++)
    {
        string lval = children_[0]->children_[i]->eval(ctx).toString() ;
        lvals_.push_back(lval) ;
    }

}


Literal ListPredicate::eval(Context &ctx)
{
    if ( !ctx.has_tag(id_) ) return Literal() ;

    string val = ctx.value(id_) ;

    for(int i=0 ; i<lvals_.size() ; i++)
        if ( val == lvals_[i] ) return is_pos_ ;

    return !is_pos_ ;
}


Literal IsTypePredicate::eval(Context &ctx)
{
    if ( keyword_ == "node")
    {
        return ctx.type() == Context::Node ;
    }
    else if ( keyword_ == "way" )
    {
        return ctx.type() == Context::Way ;
    }
    else if ( keyword_ == "relation" )
    {
        return ctx.type() == Context::Relation ;
    }

}

Literal Attribute::eval(Context &ctx)
{
    if ( !ctx.has_tag(name_) ) return Literal() ;

    return ctx.value(name_) ;
}



Literal BinaryOperator::eval(Context &ctx)
{
    Literal op1 = children_[0]->eval(ctx) ;
    Literal op2 = children_[1]->eval(ctx) ;

    if ( op == '+' )
    {
        if ( op1.type_ == Literal::String && op2.type_ == Literal::String )
            return op1.toString() + op2.toString() ;
        else return op1.toNumber() + op2.toNumber() ;
    }
    else if ( op == '-' )
    {
        return op1.toNumber() + op2.toNumber() ;
    }
    else if ( op == '.' )
    {
        return op1.toString() + op2.toString() ;
    }
    else if ( op == '*' )
    {
        return op1.toNumber() + op2.toNumber() ;
    }
    else if ( op == '/' ) {
        if ( op2.toNumber() == 0.0 ) return Literal() ;
        else return op1.toNumber()/op2.toNumber() ;
    }

}

Literal ExistsPredicate::eval(Context &ctx)
{
    return ctx.has_tag(tag_) ;

}

} // namespace Filter
} // namespace OSM

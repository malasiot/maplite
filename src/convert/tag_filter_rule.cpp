#include "tag_filter_rule.hpp"
#include "tag_filter_context.hpp"
#include "lua_context.hpp"

#include <boost/format.hpp>

using namespace std ;

namespace tag_filter {

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
    case String: return ( !string_val_.empty()) ;
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


Literal BooleanOperator::eval(TagFilterContext &ctx)
{
    switch ( op_ ) {
    case And:
        return ( lhs_->eval(ctx).toBoolean() && rhs_->eval(ctx).toBoolean() ) ;
    case Or:
        return ( lhs_->eval(ctx).toBoolean() || rhs_->eval(ctx).toBoolean() ) ;
    case Not: //?
        return !( lhs_->eval(ctx).toBoolean() ) ;
    }
}

Literal ComparisonPredicate::eval(TagFilterContext &ctx)
{
    Literal lhs = lhs_->eval(ctx) ;
    Literal rhs = rhs_->eval(ctx) ;

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
    exp_(op), is_pos_(is_pos)
{
    if ( !pattern.empty() )
        pattern_ = boost::regex(globToRegex(pattern.c_str())) ;

}


Literal LikeTextPredicate::eval(TagFilterContext &ctx)
{
    Literal op = exp_->eval(ctx) ;

    return boost::regex_match(op.toString(), pattern_) ;

}

ListPredicate::ListPredicate(const string &id, const std::deque<ExpressionNodePtr> &op, bool is_pos):
    id_(id), children_(op.begin(), op.end()), is_pos_(is_pos)
{
    TagFilterContext ctx ;

    for(int i=0 ; i<children_.size() ; i++)
    {
        string lval = children_[i]->eval(ctx).toString() ;
        lvals_.push_back(lval) ;
    }

}


Literal ListPredicate::eval(TagFilterContext &ctx)
{
    if ( !ctx.has_tag(id_) ) return Literal() ;

    string val = ctx.value(id_) ;

    for(int i=0 ; i<lvals_.size() ; i++)
        if ( val == lvals_[i] ) return is_pos_ ;

    return !is_pos_ ;
}

/*
Literal IsTypePredicate::eval(TagFilterContext &ctx)
{
    if ( keyword_ == "node")
    {
        return ctx.type() == TagFilterContext::Node ;
    }
    else if ( keyword_ == "way" )
    {
        return ctx.type() == TagFilterContext::Way ;
    }

}
*/

Literal Attribute::eval(TagFilterContext &ctx)
{
    if ( !ctx.has_tag(name_) ) return Literal() ;

    return ctx.value(name_) ;
}



Literal BinaryOperator::eval(TagFilterContext &ctx)
{
    Literal op1 = lhs_->eval(ctx) ;
    Literal op2 = rhs_->eval(ctx) ;

    if ( op_ == '+' )
    {
        if ( op1.type_ == Literal::String && op2.type_ == Literal::String )
            return op1.toString() + op2.toString() ;
        else return op1.toNumber() + op2.toNumber() ;
    }
    else if ( op_ == '-' )
    {
        return op1.toNumber() + op2.toNumber() ;
    }
    else if ( op_ == '.' )
    {
        return op1.toString() + op2.toString() ;
    }
    else if ( op_ == '*' )
    {
        return op1.toNumber() + op2.toNumber() ;
    }
    else if ( op_ == '/' ) {
        if ( op2.toNumber() == 0.0 ) return Literal() ;
        else return op1.toNumber()/op2.toNumber() ;
    }

}

Literal UnaryPredicate::eval(TagFilterContext &ctx)
{
    return exp_->eval(ctx).toBoolean() ;
}



Literal Function::eval(TagFilterContext &ctx)
{
    if ( name_ == "is_poi" ) {
        return ( ctx.type() == TagFilterContext::Node )  ;
    }
    else if ( name_ == "is_way" ) {
        return ctx.type() == TagFilterContext::Way ;
    }
    else if ( name_ == "is_relation" ) {
        return ctx.type() == TagFilterContext::Relation ;
    }
    else {
        vector<Literal> vals ;
        for(uint i=0 ; i<args_.size() ; i++)
            vals.push_back(args_[i]->eval(ctx)) ;
        return lua_->call(name_, vals) ;
    }

}
}


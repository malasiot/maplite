#include "Value.h"
#include "ParseUtil.h"

#include <boost/lexical_cast.hpp>

bool Value::toNumber(double &v) const {
    if ( type_ == Null ) return false ;
    else if ( type_ == Number ) {
        v = number_ ;
        return true ;
    }
    else if ( type_ == String )
    {
        try
        {
            v = boost::lexical_cast<double>(*string_);
            return true ;
        }
        catch ( const boost::bad_lexical_cast & )
        {
            return false ;
        }
    }
    else if ( type_ == Boolean )
    {
        return false ;
    }

}

bool Value::toColor(unsigned int &color) {
    if ( type_ == String )
        return parse_css_color(*string_, color)  ;
    else return false ;

}

bool Value::toString(std::string &s) const {
    if ( type_ == Null ) return false ;
    else if ( type_ == Boolean ) {
        s = ( boolean_ ) ? "TRUE" : "FALSE"  ;
        return true ;
    }
    else if ( type_ == Number ) {
        s = boost::lexical_cast<std::string>(number_) ;
        return true ;
    }
    else if ( type_ == String ) {
        s = *string_ ;
        return true ;
    }
}

bool Value::toBoolean(bool &v) const {
    if ( type_ == Null ) return false ;
    else if ( type_ == Number ) {
        v = number_ != 0 ;
        return true ;
    }
    else if ( type_ == String ) {
        if ( *string_ == "TRUE" || *string_ == "true" ) {
            v = true ;
            return true ;
        }
        else if ( *string_ == "FALSE" || *string_ == "false") {
            v = false ;
            return true ;
        }
        else return false ;
    }


}

bool Value::toNumberList(std::vector<double> &l) const
{
    if ( type_ == Number ) {
        l.push_back(number_) ;
        return true ;
    }
    else if ( type_ == String ) {
        return parse_number_list(*string_, l) ;
    }
    else return false ;
}

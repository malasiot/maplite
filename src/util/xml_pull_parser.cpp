#include <util/xml_pull_parser.hpp>
#include <stdexcept>

#include <util/dictionary.hpp>

using namespace std ;

static bool is_valid_name_char(char c) {
    return ( isalnum(c) || c == '-' || c == '_' || c == '.' || c == ':' ) ;
}

bool XmlPullParser::next(char &c) {

    if ( nla_ > 0 )
        c = look_ahead_[--nla_] ;
    else {
        c = (char)strm_.get() ;
        if ( c == '\r' ) c = strm_.get() ;
    }

    chars_ ++ ;
    column_ ++ ;

    if ( c == '\n' ) {
        line_++ ; column_ = 1 ;
    }

    return !strm_.eof() ;
}

bool XmlPullParser::parseBOM() {
    const char c[4] = {  (char)0xef, (char)0xbb, (char)0xbf, (char)0 } ;
    return expect(c) ;

}

void XmlPullParser::putback(char c) {
    look_ahead_[nla_++] = c ;
    chars_ -- ;
    column_ -- ;
    if ( c == '\n' ) {
        line_-- ; column_ = 1 ;
    }
}

bool XmlPullParser::expect(const char *seq) {
    char buf[256] ;
    bool match = true ;
    const char *p = seq ;

    uint k = 0 ;
    char c ;
    while ( *p && next(c) ) {

        buf[k++] = c ;
        if ( *p != c ) {
            match = false ;
            break ;
        }

        ++p ;
    }

    if ( !match )
        for ( int i=k-1 ; i>=0 ; i-- )
            putback(buf[i]) ;

    return match ;
}

bool XmlPullParser::skipWhite() {
    char c ;
    uint chars = 0 ;
    do {
        if ( !next(c) ) return false ;
        chars ++ ;
    } while ( strm_ && ( c == ' ' || c == '\t' || c == '\r' || c == '\n' ) ) ;
    putback(c) ;
    return chars > 1 ;
}


bool XmlPullParser::escapeString(string &value) {

    uint k = 0 ;
    string quot ;
    char c ;
    next(c) ;
    do {
        quot += c ;
        next(c) ;
        ++k ;
    } while ( strm_.good() && c != ';' && k < 5 ) ;

    if ( quot == "amp" ) value += '&' ;
    else if ( quot == "quot" ) value += '\"' ;
    else if ( quot == "lt" ) value += '<' ;
    else if ( quot == "gt" ) value += '>' ;
    else if ( quot == "apos") value += '\'' ;
    else return false ;

    return true ;
}

char XmlPullParser::peek() {
    if ( nla_ > 0 )
        return look_ahead_[nla_ - 1] ;
    else
        return strm_.peek() ;
}



XmlPullParser::XmlPullParser(istream &strm): strm_(strm) {
    line_ = 1 ; chars_ = 0 ; column_ = 1 ;
    nla_ = 0 ;
}


bool XmlPullParser::parseXmlDecl() {
    if ( expect("<?xml") ) {
        skipWhite() ;
        Dictionary attrs ;
        if ( !parseAttributeList(attrs) ) return fatal() ;
        skipWhite() ;
        if ( !expect("?>") ) return fatal() ;
        return true ;
    }

    return false ;
}

bool XmlPullParser::parseAttributeValue(std::string &val) {

    char c ;
    if ( !next(c) || ( c != '"' && c != '\'') ) return false ;
    char oc = c ;

    // eat characters (no backtracking here)

    while ( next(c) ) {

        if ( c == '&' ) {
           if ( !escapeString(val) ) return false ;
        }
        else if ( c == '<' ) return false ;
        else if ( c == oc ) {
            break ;
        }
        else {
            val += c ;

        }
    }  ;

    // closing quote

    return c == oc ;
}

bool XmlPullParser::parseName(std::string &name) {
    name.clear() ;

    char c ;
    while ( next(c) ) {
        if ( !is_valid_name_char(c) ) {
            putback(c) ;
            break ;
        }
        else name += c ;
    }

    return !name.empty() ;
}


bool XmlPullParser::parseAttributeList(Dictionary &attrs)
{
    do {
        string attrName, attrValue ;
        if ( !parseName(attrName) ) return false ;
        skipWhite() ;
        if ( !expect("=") ) fatal() ;
        skipWhite() ;
        if ( !parseAttributeValue(attrValue) ) fatal() ;
        attrs.add(attrName, attrValue) ;
        skipWhite() ;
        char c = peek() ;
        if ( c == '/' || c == '>' || c == '?' ) break ;
    } while ( strm_ ) ;
    return true ;
}

bool XmlPullParser::parseStartElement()
{
    if ( expect("<") ) {

        char c = peek() ;

        if ( c == '/' ) {
            putback('<') ;
            return false ;
        }

        if ( !parseName(name_) ) return fatal() ;
        skipWhite() ;

        attributes_.clear() ;
        parseAttributeList(attributes_) ;

        if ( expect("/>") ) {
            is_empty_element_tag_ = true ;
            return true ;
        }
        else if ( expect(">") ) {
            is_empty_element_tag_ = false ;
            return true ;
        }
    }

    return false ;
}

bool XmlPullParser::parseEndElement()
{
    if ( expect("</") ) {

        if ( !parseName(name_) ) return fatal() ;
        skipWhite() ;

        if ( expect(">") ) return true ;
    }

    return false ;
}


bool XmlPullParser::parseCharacters()
{
    text_.clear() ;

    char c ;
    while ( next(c) ) {

        if ( c == '<' ) {
            putback(c) ;
            break ;
        }
        else if ( c == '&' ) {
           if ( !escapeString(text_) )
               fatal() ;
        }
        else {
           text_ += c ;
        }
    }

    return !text_.empty() ;
}

bool XmlPullParser::fatal() {
    // throw XMLSAXException(code, cursor_.line_, cursor_.column_) ;
    return false ;
}


XmlPullParser::TokenType XmlPullParser::nextToken() {

    if ( token_ == START_DOCUMENT ) {
        parseBOM() ;
        skipWhite() ;
        if ( !parseXmlDecl() ) fatal() ;
        skipWhite() ;
    }

    if ( token_ == START_DOCUMENT ) {
        if ( parseStartElement() ) token_ = XmlPullParser::START_TAG ;
        else fatal() ;
    } else if ( token_ == START_TAG ) {
        if ( parseCharacters() ) token_ = TEXT ;
        else if ( parseStartElement() ) token_ = START_TAG ;
        else if ( parseEndElement() ) token_ = END_TAG ;
        else fatal() ;
    } else if ( token_ == TEXT ) {
        if ( parseStartElement() ) token_ = START_TAG ;
        else if ( parseEndElement() ) token_ = END_TAG ;
        else if ( !strm_ ) token_ = END_DOCUMENT ;
    } else if ( token_ == END_TAG ) {
        if ( parseCharacters() ) token_ = TEXT ;
        else if ( parseStartElement() ) token_ = START_TAG ;
        else if ( parseEndElement() ) token_ = END_TAG ;
        else if ( !strm_ ) token_ = END_DOCUMENT ;
    }

    return token_ ;


}

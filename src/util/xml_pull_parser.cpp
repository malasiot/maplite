#include <util/xml_pull_parser.hpp>
#include <stdexcept>
#include <cstdlib>

#include <util/dictionary.hpp>

using namespace std ;

static bool is_valid_name_char(char c, int idx) {
    if ( idx == 0 )
        return ( isalpha(c) || c == '_' || c == ':' ) ;
    else
        return ( isalnum(c) || c == '-' || c == '_' || c == '.' || c == ':' ) ;
}

bool XmlPullParser::parseBOM() {
    static const char c[4] = {  char(0xef), char(0xbb), char(0xbf), char(0) } ;
    return cursor_.expect(c) ;
}

static bool utf8_encode(string &out, uint32_t utf)
{
    if ( utf <= 0x7F ) {
        out.push_back((char) utf);
    }
    else if ( utf <= 0x07FF ) {
        // 2-byte unicode
        out.push_back((char) (((utf >> 6) & 0x1F) | 0xC0));
        out.push_back((char) (((utf >> 0) & 0x3F) | 0x80));
    }
    else if ( utf <= 0xFFFF ) {
        // 3-byte unicode
        out.push_back((char) (((utf >> 12) & 0x0F) | 0xE0));
        out.push_back((char) (((utf >>  6) & 0x3F) | 0x80));
        out.push_back((char) (((utf >>  0) & 0x3F) | 0x80));
    }
    else if ( utf <= 0x10FFFF ) {
        // 4-byte unicode
        out.push_back((char) (((utf >> 18) & 0x07) | 0xF0));
        out.push_back((char) (((utf >> 12) & 0x3F) | 0x80));
        out.push_back((char) (((utf >>  6) & 0x3F) | 0x80));
        out.push_back((char) (((utf >>  0) & 0x3F) | 0x80));
    } else return false ;

    return true ;
}

bool XmlPullParser::escapeString(string &value) {

    uint k = 0 ;
    string quot ;
    char c ;

    while ( cursor_.next(c) && c != ';' && k < 5 )  {
        quot += char(c) ;
        ++k ;
    };

    if ( quot == "amp" ) value += '&' ;
    else if ( quot == "quot" ) value += '\"' ;
    else if ( quot == "lt" ) value += '<' ;
    else if ( quot == "gt" ) value += '>' ;
    else if ( quot == "apos") value += '\'' ;
    else if ( quot.size() > 2 && quot[0] == '#' ) {
        uint32_t c ;
        try {
            if ( quot[1] == 'x' )
                c = (uint32_t)std::stoul(quot.substr(2), nullptr, 16) ;
            else
                c = (uint32_t)std::stoul(quot.substr(1), nullptr, 10) ;

            if ( !utf8_encode(value, c) ) return false ;
        } catch ( invalid_argument & ) {
            return false ;
        }
    }
    else return false ;

    return true ;
}


XmlPullParser::XmlPullParser(istream &strm, bool ns): cursor_(strm), process_ns_(ns) {
}

string XmlPullParser::getAttribute(const string &name, const string &val) const {
    return attributes_.get(name, val) ;
}


string XmlPullParser::getNamespace(const string &prefix) const
{
    if ( prefix.empty() )
        return resolveUri("xmlns") ;
    else
        return resolveUri(prefix) ;
}

static const char *parse_attributes_error = "Error parsing attribute list" ;

bool XmlPullParser::parseXmlDecl() {
    if ( cursor_.expect("<?xml") ) {
        cursor_.skipWhite() ;
        Dictionary attrs ;
        if ( !parseAttributeList(attrs) ) return fatal(parse_attributes_error) ;
        cursor_.skipWhite() ;
        if ( !cursor_.expect("?>") ) return fatal(parse_attributes_error) ;
        return true ;
    }

    return false ;
}

bool XmlPullParser::parseAttributeValue(std::string &val) {

    char c ;
    if ( !cursor_.next(c)|| ( c != '"' && c != '\'') ) return false ;
    char oc = c ;

    // eat characters (no backtracking here)

    while ( cursor_.next(c) ) {

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


static void breakDownName(const string &name, string &prefix, string &local) {
    size_t idx = name.find_first_of(':') ;
    if ( idx == string::npos ) local = name ;
    else {
        prefix = name.substr(0, idx) ;
        local = name.substr(idx+1) ;
    }
}

bool XmlPullParser::parseName(std::string &name) {
    name.clear() ;

    char c ;
    int count = 0 ;
    while ( cursor_.next(c) ) {
        if ( !is_valid_name_char(c, count) ) {
            cursor_.putback(c) ;
            break ;
        }

        name += c ;
        count ++ ;
    }

    if ( name.empty() ) return false ;

    return true ;
}


bool XmlPullParser::parseAttributeList(Dictionary &attrs) {
    attributes_.clear() ;

    do {
        string attrName, attrValue ;
        if ( !parseName(attrName) ) return false ;
        cursor_.skipWhite() ;
        if ( !cursor_.expect("=") ) fatal(parse_attributes_error) ;
        cursor_.skipWhite() ;
        if ( !parseAttributeValue(attrValue) ) fatal(parse_attributes_error) ;
        attrs.add(attrName, attrValue) ;
        cursor_.skipWhite() ;
        char c = cursor_.peek() ;
        if ( c == '/' || c == '>' || c == '?' ) break ;
    } while ( cursor_ ) ;

    return true ;
}

bool XmlPullParser::parseStartElement()
{
    int c = cursor_.peek() ;

    if ( c < 0 || c == '/' ) return false ;

    cursor_.skipWhite() ;
    if ( !parseName(name_) ) return fatal("Error parsing start element name") ;
    cursor_.skipWhite() ;

    parseAttributeList(attributes_) ;

    if ( process_ns_ ) {
        parseNameSpaceAttributes() ;
        breakDownName(name_, prefix_, local_name_) ;
        ns_ = getNamespace(prefix_) ;
    }
    else local_name_ = name_ ;

    element_stack_.emplace_back(local_name_, prefix_, ns_) ;

    if ( cursor_.expect("/>") ) {
        is_empty_element_tag_ = true ;
        return true ;
    }
    else if ( cursor_.expect(">") ) {
        is_empty_element_tag_ = false ;
        return true ;
    }

    return false ;
}

bool XmlPullParser::parseEndElement()
{
    int c = cursor_.peek() ;

    if ( c < 0 ) return false ;
    if ( c == '/' ) {
        cursor_.read() ;
        cursor_.skipWhite() ;
        if ( !parseName(name_) ) return fatal("Error parsing end element name") ;
        cursor_.skipWhite() ;
        if ( cursor_.expect(">") ) return true ;
        fatal("Invalid end element tag") ;
    }

    return false ;
}


bool XmlPullParser::parseCharacters()
{
    text_.clear() ;
    is_whitespace_ = true ;

    char c ;
    while ( cursor_.next(c) ) {
        if ( c == '\r') {
            continue ;
        }

        if ( c == '<' ) {
            cursor_.putback(c) ;
            break ;
        }
        else if ( c == '&' ) {
            is_whitespace_ = false ;
            if ( !escapeString(text_) )
                fatal("Escape string parsing error") ;
        }
        else {
            if ( c != ' ' && c != '\n' )
                is_whitespace_ = false ;
            text_ += c ;
        }
    }

    return !text_.empty() ;
}


void XmlPullParser::parseNameSpaceAttributes()
{
    Dictionary ns_attrs ;

    for( const auto &dp: attributes_ ) {
        const string &key = dp.first ;
        const string &val = dp.second ;

        string prefix, localName, nsPrefix ;
        breakDownName(key, prefix, localName) ;

        if ( prefix.empty() ) continue ;
        if ( prefix == "xmlns" )
            nsPrefix = localName ;
        else if ( localName == "xmlns" )
            nsPrefix = "default" ;

        if ( nsPrefix.empty() ) continue ;

        ns_attrs.add(nsPrefix, val) ;
    }

    ns_stack_.push_back(ns_attrs) ;
}

string XmlPullParser::resolveUri(const string ns_prefix) const
{
    auto it = ns_stack_.rbegin() ;
    for( ; it != ns_stack_.rend() ; ++it ) {
        const Dictionary &ns = *it ;
        string uri = ns.get(ns_prefix) ;
        if ( !uri.empty() ) return uri ;
    }

    return string() ;
}

bool XmlPullParser::fatal(const string &msg) {
    throw XmlPullParserException(msg, cursor_.line(), cursor_.column()) ;
    return false ;
}

bool XmlPullParser::parseDocType() {
    if ( cursor_.expect("!DOCTYPE") ) {
        // ignore section
        char c ;
        while ( cursor_.next(c) ) {
            if ( c == '>' ) break ;
            else if ( c == '[') { // skip until ending bracket (maybe nested)
                uint depth = 1 ;
                while ( cursor_.next(c) )  {
                    if ( c == '[' ) ++depth ;
                    else if ( c == ']' ) --depth ;

                    if ( depth == 0 ) break ;
                }
            }
        }

        return true ;
    }

    return false ;
}

bool XmlPullParser::parseCData()
{
    if ( cursor_.expect("![CDATA[") ) {
        text_.clear() ;
        char c ;
        while ( cursor_.next(c) ) {
            if ( c != ']' ) text_ += c ;
            else {
                if ( cursor_.expect("]>")) {
                    return true ;
                }
            }
        } ;
    }

    return false ;
}

bool XmlPullParser::parseComment()
{
    if ( cursor_.expect("!--") ) {
        text_.clear() ;
        char c ;
        while ( cursor_.next(c) && !cursor_.expect("-->") ) text_ += c ;
        if ( !cursor_ ) fatal("End of input reached while parsing comment") ;
        else return true ;
    }
    return false ;
}

bool XmlPullParser::parsePI()
{
    if ( cursor_.expect("?") ) {
        char c ;
        text_.clear() ;
        while ( cursor_.next(c) && !cursor_.expect("?>")) text_ += c ;
        return true ;
    }
    return false ;
}

XmlPullParser::TokenType XmlPullParser::nextToken() {

    if ( token_ == START_DOCUMENT ) {
        parseBOM() ;
        cursor_.skipWhite() ;
        parseXmlDecl() ;
        cursor_.skipWhite() ;
    }

    if ( token_ == START_TAG && is_empty_element_tag_ ) {
        is_empty_element_tag_ = false ;
        depth_ -- ;
        if ( process_ns_ ) ns_stack_.pop_back() ;
        element_stack_.pop_back() ;
        token_ = END_TAG ;
        return token_ ;
    }

    char c ;

    if ( !cursor_.next(c) ) {
        token_ = END_DOCUMENT ;
        return token_ ;
    }

    if ( c == '<' ) {
        if ( parseStartElement() ) {
            token_ = START_TAG ;
            depth_ ++ ;
        }
        else if ( parseEndElement() ) {
            if ( process_ns_ ) breakDownName(name_, prefix_, local_name_) ;
            else local_name_ = name_ ;

            if ( !element_stack_.empty() ) {
                const Element &e = element_stack_.back() ;
                if ( local_name_ == e.name_ ) {
                    ns_ = e.ns_ ;
                    prefix_ = e.prefix_ ;

                    token_ = END_TAG ;
                    depth_ -- ;
                    if ( process_ns_ ) ns_stack_.pop_back() ;
                    element_stack_.pop_back() ;
                    return token_ ;
                }
            }

            fatal("Unmatched end tag '" + local_name_ + "'") ;
        }
        else if ( parseDocType() ) token_ = DOCDECL ;
        else if ( parseCData() ) token_ = CDSECT ;
        else if ( parseComment() ) token_ = COMMENT ;
        else if ( parsePI() ) token_ = PROCESSING_INSTRUCTION ;
    }
    else {
        cursor_.putback(c) ;
        if ( parseCharacters() ) token_ = TEXT ;
    }

    return token_ ;
}

XmlPullParser::TokenType XmlPullParser::next() {

    do {
        nextToken() ;
    } while ( token_ == COMMENT || token_ == DOCDECL || token_ == PROCESSING_INSTRUCTION ) ;

    if ( token_ == START_TAG )
        event_ = START_TAG ;
    else if ( token_ == END_TAG )
        event_ = END_TAG ;
    else if ( token_ == CDSECT || token_ == TEXT ) {
        event_ = TEXT ;
    } else if ( token_ == END_DOCUMENT ) {
        event_ = END_DOCUMENT ;
    }

    return event_ ;
}

string XmlPullParserException::make_message(const string &msg, int line, int col) {
    stringstream msg_strm ;
    msg_strm << msg << " (near " << line << ':' << col ;
    return msg_strm.str() ;
}

#include <util/xml_pull_parser.hpp>
#include <stdexcept>
using namespace std ;

class XmlStreamWrapper {
public:
    XmlStreamWrapper(istream &strm): strm_(strm) {
        line_ = 1 ; chars_ = 0 ; column_ = 1 ;
        nla_ = 0 ;
    }

    char next() {
        int c ;
        if ( nla_ > 0 )
            c = look_ahead_[--nla_] ;
        else {
            c = strm_.get() ;
            if ( c == '\r' ) c = strm_.get() ;
        }

        chars_ ++ ;
        column_ ++ ;

        if ( c == '\n' ) {
            line_++ ; column_ = 1 ;
        }

        return char(c) ;
    }

    void putback(char c) {
        look_ahead_[nla_++] = c ;
        chars_ -- ;
        column_ -- ;
        if ( c == '\n' ) {
            line_-- ; column_ = 1 ;
        }
    }

    bool expect(const char *seq) {
        char buf[256] ;
        bool match = true ;
        const char *p = seq ;

        uint k = 0 ;
        while ( *p ) {
            char c = next() ;
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

    bool eatWhite() {
        char c ;
        uint chars = 0 ;
        do {
            c = next() ;
            chars ++ ;
        } while ( strm_ && ( c == ' ' || c == '\t' || c == '\r' || c == '\n' ) ) ;
        putback(c) ;
        return chars > 1 ;
    }

    bool good() {
        return strm_.good() ;
    }

    bool escapeString(string &value) {

        uint k = 0 ;
        string quot ;
        char c = next() ;
        do {
            quot += c ;
            c = next() ;
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

    char peek() {
        if ( nla_ > 0 )
            return look_ahead_[nla_ - 1] ;
        else
            return strm_.peek() ;
    }

    istream &strm_ ;
    size_t line_, chars_, column_ ;
    char look_ahead_[8] ;
    size_t nla_ ;
};

XmlPullParser::XmlPullParser(istream &strm) {
    stream_.reset(new XmlStreamWrapper(strm)) ;


}

XmlPullParser::TokenType XmlPullParser::nextToken() {
    stream_->eatWhite() ;
    if ( !stream_->good() ) throw runtime_error("") ;
    char c = stream_->next() ;
    if ( c == '<' ) {
        char c = stream_->next() ;
        if ( c == '?' ) { // parse processing instruction


        }
        else if ( c == '!' ) {
            char c = stream_->next() ;
        }
    }

}

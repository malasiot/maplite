#ifndef __READ_AHEAD_STREAM_ADAPTER_HPP__
#define __READ_AHEAD_STREAM_ADAPTER_HPP__

#include <iostream>
#include <memory>

template<size_t max_read_ahead = 4>
class ReadAheadStreamAdapter {
public:
    ReadAheadStreamAdapter(std::istream &strm): strm_(strm)  {
        line_ = 1 ; chars_ = 0 ; column_ = 1 ;
        nra_ = 0 ;
    }

    bool next(char &c) {
        int rc = read() ;
        if ( rc > 0 ) c = char(rc) ;
        return rc > 0 ;
    }

     int read() {
        int c ;
        if ( nra_ > 0 )
            c = read_ahead_[--nra_] ;
        else {
            c = (char)strm_.get() ;
            if ( c == '\r' ) c = strm_.get() ;
        }

        chars_ ++ ;
        column_ ++ ;

        if ( c == '\n' ) {
            line_++ ; column_ = 1 ;
        }

        if ( strm_.eof() ) return -1 ;
        else return c ;
    }


    void putback(char c) {
        read_ahead_[nra_++] = c ;
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
            int c = read() ;
            if ( c < 0 ) return false ;

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

    void skipWhite() {
        uint chars = 0 ;
        int c ;
        do {
            c = read() ;
            if ( c < 0 ) return ;
            chars ++ ;
        } while ( strm_ && ( c == ' ' || c == '\t' || c == '\r' || c == '\n' ) ) ;
        putback(c) ;
    }

    char peek() {
        if ( nra_ > 0 )
            return read_ahead_[nra_ - 1] ;
        else
            return strm_.peek() ;
    }

     operator bool () const { return (bool)strm_ ; }

private:
    std::istream &strm_ ;
    size_t line_, chars_, column_ ;
    size_t nra_ ;
    char read_ahead_[max_read_ahead];
};












#endif

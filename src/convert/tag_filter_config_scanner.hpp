#ifndef __OSM_FILTER_SCANNER_H__
#define __OSM_FILTER_SCANNER_H__

#if ! defined(yyFlexLexerOnce)
#define yyFlexLexer TagFilterConfigFlexLexer
#include <FlexLexer.h>
#endif

#include "parser/tag_filter_parser.hpp"

#undef YY_DECL
#define YY_DECL                                         \
     tag_filter::Parser::symbol_type                         \
     tag_filter::Scanner::lex(                              \
     tag_filter::Parser::location_type* yylloc          \
)

namespace tag_filter {
    class Scanner : public yyFlexLexer{

    public:
        Scanner(std::istream &strm): yyFlexLexer(&strm)  {}

        virtual tag_filter::Parser::symbol_type lex(tag_filter::Parser::location_type* yylloc);

    };

}


#endif

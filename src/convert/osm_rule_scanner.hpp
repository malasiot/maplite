#ifndef __OSM_FILTER_SCANNER_H__
#define __OSM_FILTER_SCANNER_H__

#if ! defined(yyFlexLexerOnce)
#define yyFlexLexer osmFlexLexer
#include <FlexLexer.h>
#endif

#include "parser/osm_parser.hpp"

#undef YY_DECL
#define YY_DECL                                         \
     OSM::BisonParser::symbol_type                         \
     OSM::FlexScanner::lex(                              \
     OSM::BisonParser::location_type* yylloc          \
)

namespace OSM {

    class FlexScanner : public yyFlexLexer{

    public:
        FlexScanner(std::istream &strm): yyFlexLexer(&strm)  {}

        virtual BisonParser::symbol_type lex(BisonParser::location_type* yylloc);

    };

}

#endif

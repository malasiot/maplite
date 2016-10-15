#ifndef __ECQL_SCANNER_H__
#define __ECQL_SCANNER_H__

#if ! defined(yyFlexLexerOnce)
#define yyFlexLexer ecqlFlexLexer
#include <FlexLexer.h>
#endif

#include "ecql/parser.hpp"

#undef YY_DECL
#define YY_DECL                                         \
     ECQL::BisonParser::token_type                         \
     ECQL::FlexScanner::lex(                              \
     ECQL::BisonParser::semantic_type* yylval,         \
     ECQL::BisonParser::location_type* yylloc          \
)

namespace ECQL {

    class FlexScanner : public yyFlexLexer{

    public:
        FlexScanner(std::istream &strm): yyFlexLexer(&strm)  {}

        virtual ECQL::BisonParser::token_type lex(ECQL::BisonParser::semantic_type* yylval,
                                                  ECQL::BisonParser::location_type* yylloc);

        ECQL::BisonParser::semantic_type * yylval;
    };

}

#endif

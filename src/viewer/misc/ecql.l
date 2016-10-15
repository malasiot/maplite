%{
/*
 * AT&T lex can't handle this lexer due to lex bugs.  It works with flex
 * 2.3.7, pclex 2.0.5, and MKS lex 3.1a.
 */
#include "ECQLScanner.h"
#include "ECQLParser.h"

#include "ecql/parser.hpp"
#include <string.h>

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

typedef ECQL::BisonParser::token token ;

#define yyterminate() return token::END

%}

%option debug noyywrap
%option yyclass="FlexScanner"
%option c++
%option prefix="ecql"


%{
# define YY_USER_ACTION  yylloc->columns (yyleng);
%}
%%

%{
   yylloc->step();
%}

NOT                 { return token::NOT; }
AND                 { return token::AND; }
OR                  { return token::OR; }
LIKE                { return token::LIKE; }
IS                  { return token::IS; }
NULL                { return token::NULLX; }
EXISTS              { return token::EXISTS; }
DOES-NOT-EXIST      { return token::DOES_NOT_EXIST; }
DURING              { return token::DURING; }
AFTER               { return token::AFTER; }
BEFORE              { return token::BEFORE; }
IN                  { return token::IN; }
INCLUDE             { return token::INCLUDE; }
EXCLUDE             { return token::EXCLUDE; }
TRUE                { return token::TRUEX; }
FALSE               { return token::FALSEX; }
EQUALS              { return token::EQUALS; }
DISJOINT            { return token::DISJOINT; }
INTERSECTS          { return token::INTERSECTS; }
TOUCHES             { return token::TOUCHES; }
CROSSES             { return token::CROSSES; }
WITHIN              { return token::WITHIN; }
CONTAINS            { return token::CONTAINS; }
OVERLAPS            { return token::OVERLAPS; }
RELATE              { return token::RELATE; }
DWITHIN             { return token::DWITHIN; }
BEYOND              { return token::BEYOND; }
POINT               { return token::POINT; }
LINESTRING          { return token::LINESTRING; }
POLYGON             { return token::POLYGON; }
MULTIPOINT          { return token::MULTIPOINT; }
MULTILINESTRING     { return token::MULTILINESTRING; }
MULTIPOLYGON        { return token::MULTIPOLYGON; }
GEOMETRYCOLLECTION  { return token::GEOMETRYCOLLECTION; }
BBOX                { return token::BBOX; }
ENVELOPE {return token::ENVELOPE; }

	/* punctuation */

"="     { return token::EQUAL ; }
"<>"    { return token::NOT_EQUAL ; }
"<"     { return token::LESS_THAN ; }
">"     { return token::GREATER_THAN ; }
"<="	{ return token::LESS_THAN_OR_EQUAL ; }
">="	{ return token::GREATER_THAN_OR_EQUAL ; }

"feet" |
"meters" |
"statute" |
"miles" |
"nautical miles" |
"kilometers" {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::UNITS;
}

[-+*/:(),.;\[\]]	{ return static_cast<ECQL::BisonParser::token_type>(*yytext);; }

	/* names */

[a-zA-Z]:[a-zA-Z0-9_]+ |
[a-zA-Z][a-zA-Z0-9_]*    {   yylval->stringVal = new std::string(yytext, yyleng);
                            return token::IDENTIFIER;
}
	/* numbers */

[0-9]+	|
[0-9]+"."[0-9]* |
"."[0-9]*	{

    yylval->numericVal = atof(yytext) ;
    return token::INTNUM;
}

[0-9]+[eE][+-]?[0-9]+	|
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+	{

        yylval->numericVal = atof(yytext) ;
        return token::APPROXNUM;
}

(19|20)\d\d[- /.](0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])T[0-2]\\d{1}:[0-5]\\d{1}:[0-5]\\d{1}Z {

        yylval->stringVal = new std::string(yytext, yyleng);
        return token::DATE_TIME;
}

P(?:([0-9]+)Y)?(?:([0-9]+)M)?(?:([0-9]+)D)?(?:T(?:([0-9]+)H)?(?:([0-9]+)M)?(?:([0-9]+(?:\.[0-9]+)?)?S)?) {

        yylval->stringVal = new std::string(yytext, yyleng);
        return token::DURATION;
}

	/* strings */

'([^\\\']|\\.)*' {
    yylval->stringVal = new std::string(yytext+1, yyleng-2);
    return token::STRING;
}

[\n]+

[ \t\r]+

"--".*$		;	/* comment */

%%



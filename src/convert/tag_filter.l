%{
#include "tag_filter_config_scanner.hpp"
#include "tag_filter_config_parser.hpp"

#include "parser/tag_filter_parser.hpp"
#include <string.h>

void comment() ;
%}

%option debug noyywrap
%option yyclass="tag_filter::Scanner"
%option c++
%option prefix="TagFilterConfig"
%option debug
%option case-insensitive

%x COMMENT LUA

%{
# define YY_USER_ACTION  yylloc->columns (yyleng);
%}
%%

%{
   yylloc->step();
%}

"AND"                 { return tag_filter::Parser::make_AND(*yylloc); }
"&&"                { return tag_filter::Parser::make_AND(*yylloc) ; }
"OR"                  { return tag_filter::Parser::make_OR(*yylloc); }
"||"                { return tag_filter::Parser::make_OR(*yylloc); }
"IN"				{ return tag_filter::Parser::make_IN(*yylloc); }
"NOT"				{ return tag_filter::Parser::make_NOT(*yylloc); }

        /* punctuation */

"="    { return tag_filter::Parser::make_ASSIGN(*yylloc) ; }
"=="    { return tag_filter::Parser::make_EQUAL(*yylloc) ; }
"<>"    { return tag_filter::Parser::make_NOT_EQUAL(*yylloc) ; }
"!="    { return tag_filter::Parser::make_NOT_EQUAL(*yylloc) ; }
"<"     { return tag_filter::Parser::make_LESS_THAN(*yylloc) ; }
">"     { return tag_filter::Parser::make_GREATER_THAN(*yylloc) ; }
"<="	{ return tag_filter::Parser::make_LESS_THAN_OR_EQUAL(*yylloc) ; }
">="	{ return tag_filter::Parser::make_GREATER_THAN_OR_EQUAL(*yylloc) ; }
"~"     { return tag_filter::Parser::make_MATCHES(*yylloc) ; }
"!~"    { return tag_filter::Parser::make_NOT_MATCHES(*yylloc) ; }
"!"     { return tag_filter::Parser::make_NOT(*yylloc) ; }
"^"		{ return tag_filter::Parser::make_EXISTS(*yylloc) ; }
"("     { return tag_filter::Parser::make_LPAR(*yylloc) ; }
")"     { return tag_filter::Parser::make_RPAR(*yylloc) ; }
"+"     { return tag_filter::Parser::make_PLUS(*yylloc) ; }
"-"     { return tag_filter::Parser::make_MINUS(*yylloc) ; }
"*"     { return tag_filter::Parser::make_STAR(*yylloc) ; }
"/"     { return tag_filter::Parser::make_DIV(*yylloc) ; }
","     { return tag_filter::Parser::make_COMMA(*yylloc) ; }
"."     { return tag_filter::Parser::make_DOT(*yylloc) ; }
"{"		{ return tag_filter::Parser::make_LEFT_BRACE(*yylloc) ; }
"}"		{ return tag_filter::Parser::make_RIGHT_BRACE(*yylloc) ; }
";"		{ return tag_filter::Parser::make_COLON(*yylloc) ; }
"]"		{ return tag_filter::Parser::make_RIGHT_BRACKET(*yylloc) ; }
"["	    { return tag_filter::Parser::make_LEFT_BRACKET(*yylloc) ; }

add([[:space:]]+(tag))? { return tag_filter::Parser::make_ADD_CMD(*yylloc) ; }
set([[:space:]]+(tag))? { return tag_filter::Parser::make_SET_CMD(*yylloc) ; }
delete[[:space:]]+(tag)? { return tag_filter::Parser::make_DELETE_CMD(*yylloc) ; }
"write all" { return tag_filter::Parser::make_WRITE_ALL_CMD(*yylloc) ; }
"write" { return tag_filter::Parser::make_WRITE_CMD(*yylloc) ; }
"exclude" { return tag_filter::Parser::make_EXCLUDE_CMD(*yylloc) ; }
"continue" { return tag_filter::Parser::make_CONTINUE_CMD(*yylloc) ; }
"attach" { return tag_filter::Parser::make_ATTACH_CMD(*yylloc) ; }

z[0-9][0-9]?	{ return tag_filter::Parser::make_ZOOM_SPEC(atoi(yytext+1), *yylloc) ; }

[a-zA-Z][a-zA-Z0-9_]*:[a-zA-Z0-9_]+ |
[a-zA-Z][a-zA-Z0-9_]* {
        return tag_filter::Parser::make_IDENTIFIER(yytext, *yylloc);
}

[0-9]+	|
[0-9]+"."[0-9]* |
"."[0-9]*	|
[0-9]+[eE][+-]?[0-9]+	|
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+	{
    return tag_filter::Parser::make_NUMBER(atof(yytext), *yylloc) ;
}

['\"]([^\\\']|\\.)*['\"] {
    return tag_filter::Parser::make_STRING(std::string(yytext+1, yyleng-2), *yylloc) ;
}

[\n]+   yylloc->lines (yyleng); yylloc->step ();

[ \t]+   yylloc->step ();

<<EOF>>    return tag_filter::Parser::make_END(*yylloc);

"/*" { BEGIN(COMMENT); }
<COMMENT>\n { yylloc->lines (); yylloc->step ();}
<COMMENT>"*/" { BEGIN(INITIAL) ; }
<COMMENT>.  { yylloc->step (); }

"#"[^\n]*\n { yylloc->step();}

"<?" { BEGIN(LUA); }
<LUA>\n { yylloc->lines (); yylloc->step (); yymore() ;}
<LUA>"?>" { BEGIN(INITIAL) ; return tag_filter::Parser::make_LUA_SCRIPT(std::string(yytext, yyleng-2), *yylloc) ; }
<LUA>.  { yylloc->step (); yymore(); }

%%



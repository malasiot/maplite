%require "3.0.2"
%skeleton "lalr1.cc"

%defines
%locations

%define parser_class_name {Parser}
%define api.token.constructor
%define api.namespace {tag_filter}
%define api.value.type variant
%define api.token.prefix {TOK_}

%param { TagFilterConfigParser &driver }
%param { tag_filter::Parser::location_type &loc }

%define parse.trace
%define parse.error verbose

%code requires {
#include "tag_filter_rule.hpp"
class TagFilterConfigParser ;
}

%code {
#include "tag_filter_config_parser.hpp"
static tag_filter::Parser::symbol_type yylex(TagFilterConfigParser &driver, tag_filter::Parser::location_type &loc);
}

/* literal keyword tokens */
%token NOT "!"
%token AND "&&"
%token OR "||"
%token MATCHES "~"
%token  NOT_MATCHES "!~"
%token EQUAL "=="
%token NOT_EQUAL "!="
%token LESS_THAN "<"
%token GREATER_THAN ">"
%token LESS_THAN_OR_EQUAL "<="
%token GREATER_THAN_OR_EQUAL ">="
%token TRUEX "true"
%token FALSEX "false"
%token EXISTS "^"
%token PLUS "+"
%token MINUS "-"
%token STAR "*"
%token DIV "/"
%token LPAR "("
%token RPAR ")"
%token COMMA ","
%token DOT "."
%token LEFT_BRACE "{"
%token RIGHT_BRACE "}"
%token LEFT_BRACKET "["
%token RIGHT_BRACKET "]"
%token COLON ";"
%token ADD_CMD "add tag"
%token SET_CMD "set tag"
%token DELETE_CMD "delete tag"
%token WRITE_CMD "write"
%token CONTINUE_CMD "continue"
%token ASSIGN "="
%token IN "in"
%token LAYER "@layer"
%token EXCLUDE_CMD "exclude"
%token WRITE_ALL_CMD "write all"
%token ATTACH_CMD "attach"


%token <std::string> IDENTIFIER "identifier";
%token <double> NUMBER "number";
%token <std::string> STRING "string literal";
%token <std::string> LUA_SCRIPT "LUA script";
%token <uint8_t> ZOOM_SPEC "zoom specifier"
%token END  0  "end of file";

%type <tag_filter::ExpressionNodePtr> boolean_value_expression boolean_term boolean_factor boolean_primary predicate comparison_predicate like_text_predicate
%type <tag_filter::ExpressionNodePtr> expression term factor numeric_literal boolean_literal general_literal literal function function_argument attribute
%type <tag_filter::ExpressionNodePtr> list_predicate unary_predicate
%type <tag_filter::ExpressionListPtr> literal_list function_argument_list
%type <tag_filter::CommandPtr> command
%type <tag_filter::CommandListPtr> command_list action_block
%type <tag_filter::RulePtr> rule
%type <tag_filter::RuleListPtr> rule_list
%type <tag_filter::ZoomRangePtr> zoom_range
%type <tag_filter::TagListPtr> tag_list
%type <tag_filter::TagDeclarationPtr> tag_decl
%type <tag_filter::TagDeclarationListPtr> tag_decl_list

/*operators */

%left OR
%left AND
%left LESS_THAN GREATER_THAN LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL EQUAL NOT_EQUAL
%left PLUS MINUS DOT
%left STAR DIV
%nonassoc UMINUS EXISTS

%start filter

%%

filter:
	rule_list |
	LUA_SCRIPT rule_list { driver.script_ = $1 ; }

rule_list: rule { driver.rules_.push_back($1) ; }
		  | rule rule_list { driver.rules_.push_front($1) ; }
;

rule: LEFT_BRACKET boolean_value_expression RIGHT_BRACKET action_block { $$ = std::make_shared<tag_filter::Rule>($2, $4->commands_) ; }
          | action_block { $$ = std::make_shared<tag_filter::Rule>(nullptr, $1->commands_) ; }
;


action_block:
	LEFT_BRACE command_list RIGHT_BRACE { $$ = $2 ; }
	;


command_list:
        command { $$ = std::make_shared<tag_filter::CommandList>() ; $$->commands_.push_back($1) ;  }
	| command command_list { $$ = $2 ; $$->commands_.push_front($1) ; }
	;

zoom_range:
        LEFT_BRACKET ZOOM_SPEC RIGHT_BRACKET { $$ = std::make_shared<tag_filter::ZoomRange>($2, 255) ; }
        | LEFT_BRACKET ZOOM_SPEC MINUS ZOOM_SPEC RIGHT_BRACKET { $$ = std::make_shared<tag_filter::ZoomRange>($2, $4); }
        | LEFT_BRACKET ZOOM_SPEC MINUS RIGHT_BRACKET { $$ = std::make_shared<tag_filter::ZoomRange>($2, 255); }
        | LEFT_BRACKET MINUS ZOOM_SPEC RIGHT_BRACKET { $$ = std::make_shared<tag_filter::ZoomRange>(0, $3); }

tag_decl:
        IDENTIFIER { $$ = std::make_shared<tag_filter::TagDeclaration>($1, nullptr); }
        | IDENTIFIER ASSIGN expression { $$ = std::make_shared<tag_filter::TagDeclaration>($1, $3); }

tag_list:
          IDENTIFIER { $$ = std::make_shared<tag_filter::TagList>() ; $$->tags_.push_back($1) ; }
	| tag_list COMMA IDENTIFIER { $$ = $1 ; $$->tags_.push_back($3) ; }

tag_decl_list:
          tag_decl { $$ = std::make_shared<tag_filter::TagDeclarationList>() ; $$->tags_.push_back($1) ; }
	| tag_decl_list COMMA tag_decl { $$ = $1 ; $$->tags_.push_back($3) ; }


command:
                ADD_CMD IDENTIFIER ASSIGN expression COLON { $$ = std::make_shared<tag_filter::SimpleCommand>(tag_filter::Command::Add, $2, $4) ; }
		/*The add command adds a tag if it does not already exist.*/
        |	SET_CMD IDENTIFIER ASSIGN expression COLON { $$ = std::make_shared<tag_filter::SimpleCommand>(tag_filter::Command::Set, $2, $4) ;}
		/*The 'set' command is just like the 'add' command, except that it sets the tag, even if the tag already exists*/
        |   DELETE_CMD IDENTIFIER COLON { $$ = std::make_shared<tag_filter::SimpleCommand>(tag_filter::Command::Delete, $2) ; }
		/*Delete tag from node*/
        |   WRITE_CMD zoom_range tag_decl_list COLON { $$ = std::make_shared<tag_filter::WriteCommand>(*$2, *$3) ; }
        |   ATTACH_CMD tag_list COLON { $$ = std::make_shared<tag_filter::AttachCommand>(*$2) ; }
        |   WRITE_CMD tag_decl_list COLON { $$ = std::make_shared<tag_filter::WriteCommand>( tag_filter::ZoomRange(0, 255), *$2) ; }
		/*write tag(s) to file for the given zoom range*/
        |   WRITE_ALL_CMD zoom_range COLON { $$ = std::make_shared<tag_filter::WriteAllCommand>(*$2) ; }
        |   WRITE_ALL_CMD COLON { $$ = std::make_shared<tag_filter::WriteAllCommand>( tag_filter::ZoomRange(0, 255)) ; }
		/* write all tags to file for the given zoom range */
        |   EXCLUDE_CMD zoom_range tag_list COLON { $$ = std::make_shared<tag_filter::ExcludeCommand>(*$2, *$3) ; }
        |   EXCLUDE_CMD tag_list COLON { $$ = std::make_shared<tag_filter::ExcludeCommand>( tag_filter::ZoomRange(0, 255), *$2) ; }
		/* exclude listed tags from being writen for the given zoom range */
        |   CONTINUE_CMD COLON { $$ = std::make_shared<tag_filter::SimpleCommand>( tag_filter::SimpleCommand::Continue) ;}
		/* continue with the next to level rule */
        |   rule { $$ = std::make_shared<tag_filter::RuleCommand>( $1) ;}
        |   function COLON { $$ = std::make_shared<tag_filter::FunctionCommand>($1) ; }
	;


boolean_value_expression:
	boolean_term								{ $$ = $1 ; }
        | boolean_value_expression OR boolean_term 	{ $$ = std::make_shared<tag_filter::BooleanOperator>( tag_filter::BooleanOperator::Or, $1, $3) ; }
	;

boolean_term:
	boolean_factor						{ $$ = $1 ; }
        | boolean_term AND boolean_factor	{ $$ = std::make_shared<tag_filter::BooleanOperator>( tag_filter::BooleanOperator::And, $1, $3) ; }
	;

boolean_factor:
	boolean_primary			{ $$ = $1 ; }
        | NOT boolean_primary	{ $$ = std::make_shared<tag_filter::BooleanOperator>( tag_filter::BooleanOperator::Not, $2, nullptr) ; }
	;

boolean_primary:
	predicate							{ $$ = $1 ; }
	| LPAR boolean_value_expression RPAR	{ $$ = $2 ; }
	;

predicate:
	unary_predicate { $$ = $1 ; }
	|  comparison_predicate	{ $$ = $1 ; }
	| like_text_predicate	{ $$ = $1 ; }
	| list_predicate        { $$ = $1 ; }
	;

unary_predicate:
        expression { $$ = std::make_shared<tag_filter::UnaryPredicate>( $1 ) ;}

comparison_predicate:
        expression EQUAL expression					{ $$ = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::Equal, $1, $3 ) ; }
        | expression NOT_EQUAL expression				{ $$ = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::NotEqual, $1, $3 ) ; }
        | expression LESS_THAN expression				{ $$ = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::Less, $1, $3 ) ; }
        | expression GREATER_THAN expression			{ $$ = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::Greater, $1, $3 ) ; }
        | expression LESS_THAN_OR_EQUAL expression		{ $$ = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::LessOrEqual, $1, $3 ) ; }
        | expression GREATER_THAN_OR_EQUAL expression	{ $$ = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::GreaterOrEqual, $1, $3 ) ; }
	 ;

like_text_predicate:
        expression MATCHES STRING						{ $$ = std::make_shared<tag_filter::LikeTextPredicate>($1, $3, true) ; }
        | expression NOT_MATCHES STRING					{ $$ = std::make_shared<tag_filter::LikeTextPredicate>($1, $3, false) ; }
	;

list_predicate:
        IDENTIFIER IN LPAR literal_list RPAR		{ $$ = std::make_shared<tag_filter::ListPredicate>($1, $4->children(), true) ; }
        | IDENTIFIER NOT IN LPAR literal_list RPAR	{ $$ = std::make_shared<tag_filter::ListPredicate>($1, $5->children(), false) ; }
	;

literal_list:
        literal		{ $$ = std::make_shared<tag_filter::ExpressionList>() ;  $$->append($1) ;  }
	| literal COMMA literal_list { $$ = $3 ; $3->prepend($1) ; }
	;

expression:
		  term					{ $$ = $1 ; }
                | term PLUS expression	{ $$ = std::make_shared<tag_filter::BinaryOperator>('+',$1, $3) ; }
                | term DOT expression	{ $$ = std::make_shared<tag_filter::BinaryOperator>('.',$1, $3) ; }
                | term MINUS expression	{ $$ = std::make_shared<tag_filter::BinaryOperator>('-', $1, $3) ; }
	  ;

term:
		factor					{ $$ = $1 ; }
                | factor STAR term		{ $$ = std::make_shared<tag_filter::BinaryOperator>('*', $1, $3) ; }
                | factor DIV term		{ $$ = std::make_shared<tag_filter::BinaryOperator>('/', $1, $3) ; }
		;

factor:
		  function				{ $$ = $1 ; }
		| literal				{ $$ = $1 ; }
		| attribute			{ $$ = $1 ; }
		| LPAR expression RPAR	{ $$ = $2 ; }
		;

function:
                IDENTIFIER LPAR RPAR		{ $$ = std::make_shared<tag_filter::Function>($1, &driver.lua_) ; }
		 | IDENTIFIER LPAR function_argument_list RPAR {
                        $$ = std::make_shared<tag_filter::Function>($1, $3->children(), &driver.lua_) ;
		 }
	;

function_argument_list:
		  function_argument		{
                                $$ = std::make_shared<tag_filter::ExpressionList>() ;
				$$->append($1) ;
			}
		| function_argument COMMA function_argument_list { $$ = $3 ; $3->prepend($1) ; }
		;

function_argument :
		expression			{ $$ = $1 ; }
		;

literal:
		numeric_literal		{ $$ = $1 ; }
		| general_literal	{ $$ = $1 ; }
		;

general_literal :
                STRING				{ $$ = std::make_shared<tag_filter::LiteralExpressionNode>($1) ; }
		| boolean_literal	{ $$ = $1 ; }

		;

boolean_literal:
                TRUEX	{ $$ = std::make_shared<tag_filter::LiteralExpressionNode>(true) ; }
                | FALSEX { $$ =  std::make_shared<tag_filter::LiteralExpressionNode>(false) ; }
	;

numeric_literal:
	NUMBER {
                $$ = std::make_shared<tag_filter::LiteralExpressionNode>((double)$1) ;
	}
	;

attribute:
	IDENTIFIER {
                $$ = std::make_shared<tag_filter::Attribute>($1) ;
	}
	;

%%
#define YYDEBUG 1

#include "tag_filter_config_scanner.hpp"

// We have to implement the error function
void tag_filter::Parser::error(const tag_filter::Parser::location_type &loc, const std::string &msg) {
	driver.error(loc, msg) ;
}

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

static tag_filter::Parser::symbol_type yylex(TagFilterConfigParser &driver, tag_filter::Parser::location_type &loc) {
	return  driver.scanner_.lex(&loc);
}



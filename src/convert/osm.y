%require "3.0.2"
%skeleton "lalr1.cc"

%defines
%locations
%define api.namespace {OSM}
%define parser_class_name {BisonParser}
%define api.token.constructor
%define api.value.type variant
%define api.token.prefix {TOK_}

%param { OSM::Filter::Parser &driver }
%param { OSM::BisonParser::location_type &loc }

%define parse.trace
%define parse.error verbose

%code requires {
	namespace OSM {
		namespace Filter {
			class Parser ;
			class ExpressionNode ;
			class Command ;
			class SimpleCommand ;
			class RuleCommand ;
			class LayerDefinition ;
			class Rule ;
		}
	}

}

%code {

#include <osm_rule_parser.hpp>

	// Prototype for the yylex function
static OSM::BisonParser::symbol_type yylex(OSM::Filter::Parser &driver, OSM::BisonParser::location_type &loc);
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
%token STORE_CMD "store"
%token CONTINUE_CMD "continue"
%token ASSIGN "="
%token IN "in"
%token LAYER "@layer"

%token <std::string> IDENTIFIER "identifier";
%token <double> NUMBER "number";
%token <std::string> STRING "string literal";
%token END  0  "end of file";

%type <OSM::Filter::ExpressionNode *> boolean_value_expression boolean_term boolean_factor boolean_primary predicate comparison_predicate like_text_predicate exists_predicate
%type <OSM::Filter::ExpressionNode *> expression term factor numeric_literal boolean_literal general_literal literal function function_argument function_argument_list attribute
%type <OSM::Filter::ExpressionNode *> complex_expression list_predicate literal_list
%type <OSM::Filter::Command *> action_block command_list command
%type <OSM::Filter::Rule *> rule rule_list
%type <OSM::Filter::LayerDefinition *> layer layer_list

/*%destructor { delete $$; } STRING IDENTIFIER*/

/*operators */

%left OR
%left AND
%left LESS_THAN GREATER_THAN LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL EQUAL NOT_EQUAL
%left PLUS MINUS DOT
%left STAR DIV
%nonassoc UMINUS EXISTS

%start rule_list

%%

rule_list: rule { driver.rules_ = $$ = $1 ; }
		  | rule rule_list { driver.rules_ = $$ = $1 ; $$->next_ = $2 ; }
;

rule: LEFT_BRACKET boolean_value_expression RIGHT_BRACKET action_block { $$ = new OSM::Filter::Rule{$2, $4} ; }
	  | action_block { $$ = new OSM::Filter::Rule{nullptr, $1} ; }
;


action_block:
	LEFT_BRACE command_list RIGHT_BRACE { $$ = $2 ; }
	;


command_list:
	command { $$ = $1 ;  }
	| command command_list { $$ = $1 ; $1->next_ = $2 ;}
	;

command:
		ADD_CMD IDENTIFIER ASSIGN expression COLON { $$ = new OSM::Filter::SimpleCommand( OSM::Filter::Command::Add, $2, $4) ; }
	|	SET_CMD IDENTIFIER ASSIGN expression COLON { $$ = new OSM::Filter::SimpleCommand( OSM::Filter::Command::Set, $2, $4) ;}
	|   DELETE_CMD IDENTIFIER COLON { $$ = new OSM::Filter::SimpleCommand( OSM::Filter::Command::Delete, $2) ; }
	|   STORE_CMD IDENTIFIER expression COLON { $$ = new OSM::Filter::SimpleCommand( OSM::Filter::Command::Store, $2, $3) ; }
	|   CONTINUE_CMD COLON { $$ = new OSM::Filter::SimpleCommand( OSM::Filter::SimpleCommand::Continue) ;}
	|   rule { $$ = new OSM::Filter::RuleCommand( $1) ;}

	;

complex_expression:
	boolean_value_expression	{ $$ = $1 ; }
	| expression { $$ = $1 ; }
	;

boolean_value_expression:
	boolean_term								{ $$ = $1 ; }
	| boolean_term OR boolean_value_expression	{ $$ = new OSM::Filter::BooleanOperator( OSM::Filter::BooleanOperator::Or, $1, $3) ; }
	;

boolean_term:
	boolean_factor						{ $$ = $1 ; }
	| boolean_factor AND boolean_term	{ $$ = new OSM::Filter::BooleanOperator( OSM::Filter::BooleanOperator::And, $1, $3) ; }
	;

boolean_factor:
	boolean_primary			{ $$ = $1 ; }
	| NOT boolean_primary	{ $$ = new OSM::Filter::BooleanOperator( OSM::Filter::BooleanOperator::Not, $2, NULL) ; }
	;

boolean_primary:
	predicate							{ $$ = $1 ; }
	| LPAR boolean_value_expression RPAR	{ $$ = $2 ; }
	;

predicate:
	comparison_predicate	{ $$ = $1 ; }
	| like_text_predicate	{ $$ = $1 ; }
	| exists_predicate	    { $$ = $1 ; }
	| list_predicate        { $$ = $1 ; }
	;


comparison_predicate:
	 expression EQUAL expression					{ $$ = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::Equal, $1, $3 ) ; }
	| expression NOT_EQUAL expression				{ $$ = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::NotEqual, $1, $3 ) ; }
	| expression LESS_THAN expression				{ $$ = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::Less, $1, $3 ) ; }
	| expression GREATER_THAN expression			{ $$ = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::Greater, $1, $3 ) ; }
	| expression LESS_THAN_OR_EQUAL expression		{ $$ = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::LessOrEqual, $1, $3 ) ; }
	| expression GREATER_THAN_OR_EQUAL expression	{ $$ = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::GreaterOrEqual, $1, $3 ) ; }
	 ;

like_text_predicate:
	expression MATCHES STRING							{ $$ = new OSM::Filter::LikeTextPredicate($1, $3, true) ; }
	| expression NOT_MATCHES STRING					{ $$ = new OSM::Filter::LikeTextPredicate($1, $3, false) ; }
	;

exists_predicate:
	EXISTS IDENTIFIER								{ $$ = new OSM::Filter::ExistsPredicate($2) ; }
	;

list_predicate:
		IDENTIFIER IN LPAR literal_list RPAR	{ $$ = new OSM::Filter::ListPredicate($1, $4, true) ; }
	  | IDENTIFIER NOT IN LPAR literal_list RPAR	{ $$ = new OSM::Filter::ListPredicate($1, $5, false) ; }
		;

literal_list:
				  literal		{ $$ = new OSM::Filter::ExpressionNode() ; $$->appendChild($1) ; }
				| literal COMMA literal_list { $$ = $3 ; $3->prependChild($1) ; }
				;

expression:
		  term					{ $$ = $1 ; }
		| term PLUS expression	{ $$ = new OSM::Filter::BinaryOperator('+',$1, $3) ; }
		| term DOT expression	{ $$ = new OSM::Filter::BinaryOperator('.',$1, $3) ; }
		| term MINUS expression	{ $$ = new OSM::Filter::BinaryOperator('-', $1, $3) ; }
	  ;

term:
		factor					{ $$ = $1 ; }
		| factor STAR term		{ $$ = new OSM::Filter::BinaryOperator('*', $1, $3) ; }
		| factor DIV term		{ $$ = new OSM::Filter::BinaryOperator('/', $1, $3) ; }
		;

factor:
		  function				{ $$ = $1 ; }
		| literal				{ $$ = $1 ; }
		| attribute			{ $$ = $1 ; }
		| LPAR expression RPAR	{ $$ = $2 ; }
		;

function:
		IDENTIFIER LPAR RPAR		{ $$ = new OSM::Filter::Function($1) ; }
		 | IDENTIFIER LPAR function_argument_list RPAR {
			$$ = new OSM::Filter::Function($1, $3) ;
		 }
	;

function_argument_list:
		  function_argument		{ $$ = new OSM::Filter::ExpressionNode() ; $$->appendChild($1) ; }
		| function_argument COMMA function_argument_list { $$ = $3 ; $3->prependChild($1) ; }
		;

function_argument :
		expression			{ $$ = $1 ; }
		;

literal:
		numeric_literal		{ $$ = $1 ; }
		| general_literal	{ $$ = $1 ; }
		;

general_literal :
		STRING				{ $$ = new OSM::Filter::LiteralExpressionNode($1) ; }
		| boolean_literal	{ $$ = $1 ; }

		;

boolean_literal:
		TRUEX	{ $$ = new OSM::Filter::LiteralExpressionNode(true) ; }
		| FALSEX { $$ =  new OSM::Filter::LiteralExpressionNode(false) ; }
	;

numeric_literal:
	NUMBER {
		$$ = new OSM::Filter::LiteralExpressionNode((double)$1) ;
	}
	;

attribute:
	IDENTIFIER {
		$$ = new OSM::Filter::Attribute($1) ;
	}
	;



%%
#define YYDEBUG 1
#include <osm_rule_scanner.hpp>

// We have to implement the error function
void OSM::BisonParser::error(const OSM::BisonParser::location_type &loc, const std::string &msg) {

	driver.error(loc, msg) ;
}

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

static OSM::BisonParser::symbol_type yylex(OSM::Filter::Parser &driver, OSM::BisonParser::location_type &loc) {
	return  driver.scanner_.lex(&loc);
}



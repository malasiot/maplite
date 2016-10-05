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

#include <memory>

	namespace OSM {
		namespace Filter {
			class Parser ;
			class ExpressionNode ;
			typedef std::shared_ptr<ExpressionNode> ExpressionNodePtr ;
			class Command ;
			typedef std::shared_ptr<Command> CommandPtr ;
			class CommandList ;
			typedef std::shared_ptr<CommandList> CommandListPtr ;
			class Rule ;
			typedef std::shared_ptr<Rule> RulePtr ;
			class RuleList ;
			typedef std::shared_ptr<RuleList> RuleListPtr ;
			class ZoomRange ;
			typedef std::shared_ptr<ZoomRange> ZoomRangePtr ;
			class TagList ;
			typedef std::shared_ptr<TagList> TagListPtr ;
		}
	}

}

%code {


#include "osm_rule_parser.hpp"

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
%token WRITE_CMD "write"
%token CONTINUE_CMD "continue"
%token ASSIGN "="
%token IN "in"
%token LAYER "@layer"
%token EXCLUDE_CMD "exclude"
%token WRITE_ALL_CMD "write all"


%token <std::string> IDENTIFIER "identifier";
%token <double> NUMBER "number";
%token <std::string> STRING "string literal";
%token <uint8_t> ZOOM_SPEC "zoom specifier"
%token END  0  "end of file";

%type <OSM::Filter::ExpressionNodePtr> boolean_value_expression boolean_term boolean_factor boolean_primary predicate comparison_predicate like_text_predicate exists_predicate
%type <OSM::Filter::ExpressionNodePtr> expression term factor numeric_literal boolean_literal general_literal literal function function_argument function_argument_list attribute
%type <OSM::Filter::ExpressionNodePtr> complex_expression list_predicate literal_list
%type <OSM::Filter::CommandPtr> command
%type <OSM::Filter::CommandListPtr> command_list action_block
%type <OSM::Filter::RulePtr> rule
%type <OSM::Filter::RuleListPtr> rule_list
%type <OSM::Filter::ZoomRangePtr> zoom_range
%type <OSM::Filter::TagListPtr> tag_list
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

rule_list: rule { driver.rules_.push_back($1) ; }
		  | rule rule_list { driver.rules_.push_front($1) ; }
;

rule: LEFT_BRACKET boolean_value_expression RIGHT_BRACKET action_block { $$ = std::make_shared<OSM::Filter::Rule>($2, $4) ; }
	  | action_block { $$ = std::make_shared<OSM::Filter::Rule>(nullptr, $1) ; }
;


action_block:
	LEFT_BRACE command_list RIGHT_BRACE { $$ = $2 ; }
	;


command_list:
	command { $$ = std::make_shared<OSM::Filter::CommandList>() ; $$->commands_.push_back($1) ;  }
	| command command_list { $$ = $2 ; $$->commands_.push_front($1) ; }
	;

zoom_range:
	LEFT_BRACKET ZOOM_SPEC MINUS ZOOM_SPEC RIGHT_BRACKET { $$ = std::make_shared<OSM::Filter::ZoomRange>($2, $4); }
	| LEFT_BRACKET ZOOM_SPEC MINUS RIGHT_BRACKET { $$ = std::make_shared<OSM::Filter::ZoomRange>($2, 255); }
	| LEFT_BRACKET MINUS ZOOM_SPEC RIGHT_BRACKET { $$ = std::make_shared<OSM::Filter::ZoomRange>(0, $3); }

tag_list:
	  IDENTIFIER { $$ = std::make_shared<OSM::Filter::TagList>() ; $$->tags_.push_back($1) ; }
	| tag_list COMMA IDENTIFIER { $$ = $1 ; $$->tags_.push_back($3) ; }

command:
		ADD_CMD IDENTIFIER ASSIGN expression COLON { $$ = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Add, $2, $4) ; }
		/*The add command adds a tag if it does not already exist.*/
	|	SET_CMD IDENTIFIER ASSIGN expression COLON { $$ = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Set, $2, $4) ;}
		/*The 'set' command is just like the 'add' command, except that it sets the tag, even if the tag already exists*/
	|   DELETE_CMD IDENTIFIER COLON { $$ = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Delete, $2) ; }
		/*Delete tag from node*/
	|   WRITE_CMD zoom_range tag_list COLON { $$ = std::make_shared<OSM::Filter::WriteCommand>(*$2, *$3) ; }
	|   WRITE_CMD tag_list COLON { $$ = std::make_shared<OSM::Filter::WriteCommand>( OSM::Filter::ZoomRange(0, 255), *$2) ; }
		/*write tag(s) to file for the given zoom range*/
	|   WRITE_ALL_CMD zoom_range COLON { $$ = std::make_shared<OSM::Filter::WriteAllCommand>(*$2) ; }
	|   WRITE_ALL_CMD COLON { $$ = std::make_shared<OSM::Filter::WriteAllCommand>( OSM::Filter::ZoomRange(0, 255)) ; }
		/* write all tags to file for the given zoom range */
	|   EXCLUDE_CMD zoom_range tag_list COLON { $$ = std::make_shared<OSM::Filter::ExcludeCommand>(*$2, *$3) ; }
	|   EXCLUDE_CMD tag_list COLON { $$ = std::make_shared<OSM::Filter::ExcludeCommand>( OSM::Filter::ZoomRange(0, 255), *$2) ; }
		/* exclude listed tags from being writen for the given zoom range */
	|   CONTINUE_CMD COLON { $$ = std::make_shared<OSM::Filter::SimpleCommand>( OSM::Filter::SimpleCommand::Continue) ;}
		/* continue with the next to level rule */
	|   rule { $$ = std::make_shared<OSM::Filter::RuleCommand>( $1) ;}


	;

complex_expression:
	boolean_value_expression	{ $$ = $1 ; }
	| expression { $$ = $1 ; }
	;

boolean_value_expression:
	boolean_term								{ $$ = $1 ; }
	| boolean_term OR boolean_value_expression	{ $$ = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Or, $1, $3) ; }
	;

boolean_term:
	boolean_factor						{ $$ = $1 ; }
	| boolean_factor AND boolean_term	{ $$ = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::And, $1, $3) ; }
	;

boolean_factor:
	boolean_primary			{ $$ = $1 ; }
	| NOT boolean_primary	{ $$ = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Not, $2, nullptr) ; }
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
	 expression EQUAL expression					{ $$ = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Equal, $1, $3 ) ; }
	| expression NOT_EQUAL expression				{ $$ = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::NotEqual, $1, $3 ) ; }
	| expression LESS_THAN expression				{ $$ = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Less, $1, $3 ) ; }
	| expression GREATER_THAN expression			{ $$ = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Greater, $1, $3 ) ; }
	| expression LESS_THAN_OR_EQUAL expression		{ $$ = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::LessOrEqual, $1, $3 ) ; }
	| expression GREATER_THAN_OR_EQUAL expression	{ $$ = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::GreaterOrEqual, $1, $3 ) ; }
	 ;

like_text_predicate:
	expression MATCHES STRING						{ $$ = std::make_shared<OSM::Filter::LikeTextPredicate>($1, $3, true) ; }
	| expression NOT_MATCHES STRING					{ $$ = std::make_shared<OSM::Filter::LikeTextPredicate>($1, $3, false) ; }
	;

exists_predicate:
	EXISTS IDENTIFIER								{ $$ = std::make_shared<OSM::Filter::ExistsPredicate>($2) ; }
	;

list_predicate:
		IDENTIFIER IN LPAR literal_list RPAR		{ $$ = std::make_shared<OSM::Filter::ListPredicate>($1, $4, true) ; }
	  | IDENTIFIER NOT IN LPAR literal_list RPAR	{ $$ = std::make_shared<OSM::Filter::ListPredicate>($1, $5, false) ; }
		;

literal_list:
				  literal		{ $$ = std::make_shared<OSM::Filter::ExpressionNode>() ; $$->appendChild($1) ; }
				| literal COMMA literal_list { $$ = $3 ; $3->prependChild($1) ; }
				;

expression:
		  term					{ $$ = $1 ; }
		| term PLUS expression	{ $$ = std::make_shared<OSM::Filter::BinaryOperator>('+',$1, $3) ; }
		| term DOT expression	{ $$ = std::make_shared<OSM::Filter::BinaryOperator>('.',$1, $3) ; }
		| term MINUS expression	{ $$ = std::make_shared<OSM::Filter::BinaryOperator>('-', $1, $3) ; }
	  ;

term:
		factor					{ $$ = $1 ; }
		| factor STAR term		{ $$ = std::make_shared<OSM::Filter::BinaryOperator>('*', $1, $3) ; }
		| factor DIV term		{ $$ = std::make_shared<OSM::Filter::BinaryOperator>('/', $1, $3) ; }
		;

factor:
		  function				{ $$ = $1 ; }
		| literal				{ $$ = $1 ; }
		| attribute			{ $$ = $1 ; }
		| LPAR expression RPAR	{ $$ = $2 ; }
		;

function:
		IDENTIFIER LPAR RPAR		{ $$ = std::make_shared<OSM::Filter::Function>($1) ; }
		 | IDENTIFIER LPAR function_argument_list RPAR {
			$$ = std::make_shared<OSM::Filter::Function>($1, $3) ;
		 }
	;

function_argument_list:
		  function_argument		{ $$ = std::make_shared<OSM::Filter::ExpressionNode>() ; $$->appendChild($1) ; }
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
		STRING				{ $$ = std::make_shared<OSM::Filter::LiteralExpressionNode>($1) ; }
		| boolean_literal	{ $$ = $1 ; }

		;

boolean_literal:
		TRUEX	{ $$ = std::make_shared<OSM::Filter::LiteralExpressionNode>(true) ; }
		| FALSEX { $$ =  std::make_shared<OSM::Filter::LiteralExpressionNode>(false) ; }
	;

numeric_literal:
	NUMBER {
		$$ = std::make_shared<OSM::Filter::LiteralExpressionNode>((double)$1) ;
	}
	;

attribute:
	IDENTIFIER {
		$$ = std::make_shared<OSM::Filter::Attribute>($1) ;
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



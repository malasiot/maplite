%require "2.4.1"
%skeleton "lalr1.cc"

%defines
%locations

%define namespace ECQL
%define parser_class_name "BisonParser"
%parse-param { ECQL::Parser &driver }
%lex-param { ECQL::Parser &driver }
%name-prefix "ecql"

%error-verbose

%code requires {
	namespace ECQL {
		class Parser ;
		class FilterNode ;
		class PointSequenceNode ;
	}

}

%code {
#include <ECQLParser.h>
#include <vector>

	// Prototype for the yylex function
static int yylex(ECQL::BisonParser::semantic_type * yylval, ECQL::BisonParser::location_type *loc, ECQL::Parser &driver);
}

	/* symbolic tokens */

%union {
	std::string *stringVal;
	double numericVal ;
	ECQL::FilterNode *nodeType ;
	PointSequenceNode *pointSeqType ;
	struct Point { float x, y ; } pointType ;
}

	
%token <stringVal> STRING	"string"
%token <numericVal> INTNUM	"integer"
%token <numericVal> APPROXNUM "numeric value"
%token <stringVal>  DATE_TIME "date"
%token <stringVal>  DURATION  "duration"
%token <stringVal> IDENTIFIER "identifier"
%token <stringVal> UNITS "units"
%token END  0  "end of file"

%type <numericVal> number ;
%type <nodeType> expression term factor numeric_literal boolean_literal general_literal attribute literal function function_argument function_argument_list string_literal
%type <nodeType> geometry_literal geometrycollection_text geometry_tagged_sequence id id_sequence
%type <nodeType> boolean_value_expression boolean_term boolean_factor boolean_primary predicate comparison_predicate like_text_predicate null_predicate existence_predicate
%type <nodeType> between_predicate temporal_predicate in_predicate include_exclude_predicate in_predicate_value in_value_list spatial_predicate id_predicate bbox_argument_list
%type <pointSeqType> point_text point_sequence_text linestring_text linestring_sequence_text polygon_text polygon_sequence_text multipoint_text multilinestring_text multipolygon_text envelope_text
%type <pointType> point
%type <nodeType> program
%type <stringVal> date_time_expression period


%destructor { delete $$; } STRING DATE_TIME DURATION IDENTIFIER

/*operators */

%left OR
%left AND
%left LESS_THAN GREATER_THAN LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL EQUAL NOT_EQUAL
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

	/* literal keyword tokens */
%token	NOT AND OR LIKE IS NULLX EXISTS DOES_NOT_EXIST DURING AFTER BEFORE BETWEEN
%token  IN INCLUDE EXCLUDE TRUEX FALSEX EQUALS DISJOINT INTERSECTS TOUCHES CROSSES
%token  WITHIN CONTAINS OVERLAPS RELATE DWITHIN BEYOND POINT LINESTRING POLYGON
%token  MULTIPOINT MULTILINESTRING MULTIPOLYGON GEOMETRYCOLLECTION ENVELOPE BBOX
%token  EQUAL NOT_EQUAL LESS_THAN GREATER_THAN LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL


%start program

%%

program: boolean_value_expression		{ $$ = $1 ; driver.node = $$ ; }


geometry_literal:
	POINT point_text								{ $$ = new ECQL::GeometryLiteral(ECQL::Point, $2) ; }
	| LINESTRING linestring_text					{ $$ = new ECQL::GeometryLiteral(ECQL::LineString, $2) ;  }
	| POLYGON polygon_text							{ $$ = new ECQL::GeometryLiteral(ECQL::Polygon, $2) ;  }
	| MULTIPOINT multipoint_text					{ $$ = new ECQL::GeometryLiteral(ECQL::MultiPoint, $2) ;  }
	| MULTILINESTRING multilinestring_text			{ $$ = new ECQL::GeometryLiteral(ECQL::MultiLineString, $2) ;  }
	| MULTIPOLYGON multipolygon_text				{ $$ = new ECQL::GeometryLiteral(ECQL::MultiPolygon, $2) ;  ; }
	| GEOMETRYCOLLECTION geometrycollection_text	{ $$ = $2 ;  }
	| ENVELOPE envelope_text						{ $$ = new ECQL::GeometryLiteral(ECQL::Envelope, $2); }
	;

geometry_tagged_sequence:
	geometry_literal {
		ECQL::GeometryLiteral *node = new ECQL::GeometryLiteral(ECQL::GeometryCollection, NULL) ;
		node->appendChild($1) ;
		$$ = node ;
	}
	| geometry_tagged_sequence ',' geometry_literal {
		ECQL::GeometryLiteral *node = (ECQL::GeometryLiteral *)$1 ;
		node->appendChild($3) ;
		$$ = node ;
	}
	;

point_text:
	 '(' point ')'	{
		 PointSequenceNode *node = new PointSequenceNode() ;
		 node->addPoint($2.x, $2.y) ;
		 $$ = node ;
	 }
	 ;

point: number number	{
	$$.x = $1 ;
	$$.y = $2 ;
}
;

point_sequence_text:
	point	{
		PointSequenceNode *node = new PointSequenceNode( ) ;
		node->addPoint($1.x, $1.y) ;
		$$ = node ;
	}
	| point_sequence_text ',' point {
		PointSequenceNode *node = (PointSequenceNode *)$1 ;
		node->addPoint($3.x, $3.y) ;
		$$ = node ;
	}
;

linestring_text:
	'(' point_sequence_text ')' { $$ = $2 ; }
	;

linestring_sequence_text:
	linestring_text	{
		PointSequenceNode *node = new PointSequenceNode(  ) ;
		node->appendChild($1) ;
		$$ = node ;
	}
	| linestring_sequence_text ',' linestring_text {
		PointSequenceNode *node = (PointSequenceNode *)$1 ;
		node->appendChild($3) ;
		$$ = node ;
	}
	;

polygon_text :
	 '(' linestring_sequence_text ')' { $$ = $2 ; }
	 ;

polygon_sequence_text:
	polygon_text {
		PointSequenceNode *node = new PointSequenceNode(  ) ;
		node->appendChild($1) ;
		$$ = node ;
	}
	| polygon_sequence_text ',' polygon_text {
		PointSequenceNode *node = (PointSequenceNode *)$1 ;
		node->appendChild($3) ;
	}
	;

multipoint_text:
	'(' point_sequence_text ')' { $$ = $2 ; }
	;

multilinestring_text:
	  '(' linestring_sequence_text ')' { $$ = $2 ; }
	 ;

multipolygon_text:
	'(' polygon_sequence_text ')' { $$ = $2 ; }
	;

geometrycollection_text:
	'(' geometry_tagged_sequence ')' { $$ = $2 ; }
	;

envelope_text:
	'(' number ','number ',' number ',' number ')' {
		PointSequenceNode *node = new PointSequenceNode() ;
		node->addPoint($2, $4) ;
		node->addPoint($6, $8) ;
		$$ = node ;
	}
	;

numeric_literal:
	number {
		$$ = new ECQL::NumericLiteral($1) ;
	}
	;

string_literal:
	STRING {
		$$ = new ECQL::StringLiteral(*$1) ;
	}

number:
	INTNUM		{ $$ = $1 ; }
	| APPROXNUM { $$ = $1 ; }
	;

spatial_predicate:
	EQUALS '(' expression ',' expression ')'				{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Equals, $3, $5) ; }
	| DISJOINT '(' expression ',' expression ')'			{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Disjoint, $3, $5) ; }
	| INTERSECTS '(' expression ',' expression ')'			{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Intersects, $3, $5) ; }
	| TOUCHES '(' expression ',' expression ')'				{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Touches, $3, $5) ; }
	| CROSSES '(' expression ',' expression ')'				{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Crosses, $3, $5) ; }
	| WITHIN '(' expression ',' expression ')'				{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Within, $3, $5) ; }
	| CONTAINS '(' expression ',' expression ')'			{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Contains, $3, $5) ; }
	| OVERLAPS '(' expression ',' expression ')'			{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Overlaps, $3, $5) ; }
	| RELATE '(' expression ',' expression ',' STRING ')'				{ $$ = new ECQL::SpatialPredicate(ECQL::SpatialPredicate::Relate, $3, $5, *$7) ; }
	| DWITHIN '(' expression ',' expression ',' number ',' UNITS ')' {
		$$ = new ECQL::RelativeSpatialPredicate(ECQL::RelativeSpatialPredicate::DWithin,
		$3, $5, $7,*$9) ;

	}
	| BEYOND '(' expression ',' expression ',' number ',' UNITS ')' {
		$$ = new ECQL::RelativeSpatialPredicate(ECQL::RelativeSpatialPredicate::Beyond,
		$3, $5, $7, *$9) ;
	}
	| BBOX bbox_argument_list { $$ = $2; }
	;

bbox_argument_list:
	  '(' expression ',' number ',' number ',' number ',' number ')' {
	  $$ = new ECQL::BBox($2, $4, $6, $8, $10) ;
	  }
	| '(' expression ',' number ',' number ',' number ',' number ',' STRING ')' {
		$$ = new ECQL::BBox($2, $4, $6, $8, $10, *$12																						) ;
	}
	| '(' expression ',' expression ')' {
		$$ = new ECQL::BBox($2, $4) ;
	}
	;


attribute:
	IDENTIFIER					{	$$ = new ECQL::Attribute(*$1) ; }
	| IDENTIFIER '.' attribute	{   $$ = $3 ; ((ECQL::Attribute *)$3)->prepend(*$1) ; }
;

expression:
	  term					{ $$ = $1 ; }
	| term '+' expression	{ $$ = new ECQL::BinaryOperator('+',$1, $3) ; }
	| term '-' expression	{ $$ = new ECQL::BinaryOperator('-', $1, $3) ; }
  ;

term:
	factor					{ $$ = $1 ; }
	| factor '*' term		{ $$ = new ECQL::BinaryOperator('*', $1, $3) ; }
	| factor '/' term		{ $$ = new ECQL::BinaryOperator('/', $1, $3) ; }
	;

factor:
	  function				{ $$ = $1 ; }
	| literal				{ $$ = $1 ; }
	| attribute				{ $$ = $1 ; }
	| '(' expression ')'	{ $$ = $2 ; }
	| '[' expression ']'	{ $$ = $2 ; }
	;

function:
	IDENTIFIER '(' ')'		{ $$ = new ECQL::Function(*$1) ; }
	 | IDENTIFIER '(' function_argument_list ')' {
		$$ = new ECQL::Function(*$1, $3) ;
	 }
;

function_argument_list:
	  function_argument		{ $$ = new ECQL::FilterNode() ; $$->appendChild($1) ; }
	| function_argument ',' function_argument_list { $$ = $3 ; $3->prependChild($1) ; }
	;

function_argument :
	expression			{ $$ = $1 ; }
	;

literal:
	numeric_literal		{ $$ = $1 ; }
	| general_literal	{ $$ = $1 ; }
	;

general_literal :
	STRING				{ $$ = new ECQL::StringLiteral(*$1) ; }
	| boolean_literal	{ $$ = $1 ; }
	| geometry_literal  { $$ = $1 ; }
	;

boolean_literal:
	TRUEX	{ $$ = new ECQL::BooleanLiteral(true) ; }
	| FALSEX { $$ =  new ECQL::BooleanLiteral(false) ; }
;

boolean_value_expression:
	boolean_term								{ $$ = $1 ; }
	| boolean_term OR boolean_value_expression	{ $$ = new ECQL::BooleanOperator( ECQL::BooleanOperator::Or, $1, $3) ; }
	;

boolean_term:
	boolean_factor						{ $$ = $1 ; }
	| boolean_factor AND boolean_term	{ $$ = new ECQL::BooleanOperator( ECQL::BooleanOperator::And, $1, $3) ; }
	;

boolean_factor:
	boolean_primary			{ $$ = $1 ; }
	| NOT boolean_primary	{ $$ = new ECQL::BooleanOperator( ECQL::BooleanOperator::Not, $2, NULL) ; }
	;

boolean_primary:
	predicate							{ $$ = $1 ; }
	| '(' boolean_value_expression ')'	{ $$ = $2 ; }
	| '[' boolean_value_expression ']'	{ $$ = $2 ; }
	;

predicate:
	comparison_predicate	{ $$ = $1 ; }
	| like_text_predicate	{ $$ = $1 ; }
	| null_predicate		{ $$ = $1 ; }
	| temporal_predicate	{ $$ = $1 ; }
	| existence_predicate	{ $$ = $1 ; }
	| between_predicate		{ $$ = $1 ; }
	| spatial_predicate		{ $$ = $1 ; }
	| include_exclude_predicate	{ $$ = $1 ; }
	| id_predicate				{ $$ = $1 ; }
	| in_predicate				{ $$ = $1 ; }

	;

comparison_predicate:
	 expression EQUAL expression					{ $$ = new ECQL::ComparisonPredicate( ECQL::ComparisonPredicate::Equal, $1, $3 ) ; }
	 | expression NOT_EQUAL expression				{ $$ = new ECQL::ComparisonPredicate( ECQL::ComparisonPredicate::NotEqual, $1, $3 ) ; }
	 | expression LESS_THAN expression				{ $$ = new ECQL::ComparisonPredicate( ECQL::ComparisonPredicate::Less, $1, $3 ) ; }
	 | expression GREATER_THAN expression			{ $$ = new ECQL::ComparisonPredicate( ECQL::ComparisonPredicate::Greater, $1, $3 ) ; }
	 | expression LESS_THAN_OR_EQUAL expression		{ $$ = new ECQL::ComparisonPredicate( ECQL::ComparisonPredicate::LessOrEqual, $1, $3 ) ; }
	 | expression GREATER_THAN_OR_EQUAL expression	{ $$ = new ECQL::ComparisonPredicate( ECQL::ComparisonPredicate::GreaterOrEqual, $1, $3 ) ; }
	 ;

like_text_predicate:
	expression LIKE STRING							{ $$ = new ECQL::LikeTextPredicate($1, *$3, true) ; }
	| expression NOT LIKE STRING					{ $$ = new ECQL::LikeTextPredicate($1, *$4, false) ; }
	;

null_predicate:
	expression IS NULLX								{ $$ = new ECQL::IsNullPredicate($1, true) ; }
	| expression IS NOT NULLX						{ $$ = new ECQL::IsNullPredicate($1, false) ; }
	;

existence_predicate:
	  attribute EXISTS								{ $$ = new ECQL::AttributeExistsPredicate($1, true) ; }
	| attribute DOES_NOT_EXIST						{ $$ = new ECQL::AttributeExistsPredicate($1, false) ; }
	;

between_predicate:
	expression BETWEEN expression AND expression		{ $$ = new ECQL::AttributeBetweenPredicate($1, $3, $5, true) ; }
	| expression NOT BETWEEN expression AND expression  { $$ = new ECQL::AttributeBetweenPredicate($1, $4, $6, false) ; }
	;

temporal_predicate:
	  expression BEFORE date_time_expression		{ $$ = new ECQL::TemporalPredicate(ECQL::TemporalPredicate::Before, $1, *$3) ; }
	| expression BEFORE OR DURING period			{ $$ = new ECQL::TemporalPredicate(ECQL::TemporalPredicate::BeforeOrDuring, $1, *$5) ; }
	| expression DURING period						{ $$ = new ECQL::TemporalPredicate(ECQL::TemporalPredicate::During, $1, *$3) ; }
	| expression DURING OR AFTER period				{ $$ = new ECQL::TemporalPredicate(ECQL::TemporalPredicate::DuringOrAfter, $1, *$5) ; }
	| expression AFTER date_time_expression			{ $$ = new ECQL::TemporalPredicate(ECQL::TemporalPredicate::After, $1, *$3) ; }
	;

date_time_expression:
	DATE_TIME			{ $$ = $1 ; }
	| period			{ $$ = $1 ; }
	;

id_predicate:
	IN '(' id_sequence ')' { $$ = new ECQL::IdPredicate($3) ; }
	;

id_sequence:
	id	{
		ECQL::FilterNode *node = new ECQL::FilterNode() ;
		node->appendChild($1) ;
		$$ = node ;
	}
	| id_sequence ',' id {
		ECQL::FilterNode *node = (ECQL::FilterNode *)$1 ;
		node->appendChild($3) ;
		$$ = node ;
	}
	;

id:
	string_literal		{ $$ = $1 ; }
	| numeric_literal	{ $$ = $1 ; }
	;

in_predicate:
	expression IN in_predicate_value			{ $$ = new ECQL::InPredicate($1, $3, true) ; }
	| expression NOT IN in_predicate_value		{ $$ = new ECQL::InPredicate($1, $4, false) ; }
	;

in_predicate_value:
'(' in_value_list ')'	{ $$ = $2 ; }
;


in_value_list:
	expression	{
		ECQL::FilterNode *node = new ECQL::FilterNode() ;
		node->appendChild($1) ;
		$$ = node ;
	}
	| in_value_list ',' expression {
		ECQL::FilterNode *node = ( ECQL::FilterNode *)$1 ;
		node->appendChild($3) ;
		$$ = node ;
	}
	;

include_exclude_predicate:
	INCLUDE					{ $$ = new ECQL::IncludePredicate(true) ; }
	| EXCLUDE				{ $$ = new ECQL::IncludePredicate(false) ; }
;



period:
	DATE_TIME '/' DATE_TIME { $$ = new std::string(*$1 + '/' + *$3) ; }
	| DATE_TIME '/' DURATION	{ $$ = new std::string(*$1 + '/' + *$3) ; }
	| DURATION '/' DATE_TIME  { $$ = new std::string(*$1 + '/' + *$3) ; }


%%

#include "ECQLParser.h"

// We have to implement the error function
void ECQL::BisonParser::error(const ECQL::BisonParser::location_type &loc, const std::string &msg) {

	driver.error(loc, msg) ; //std::cerr << "Error: " << msg << " at " << loc << std::endl;
}

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

static int yylex(ECQL::BisonParser::semantic_type * yylval, ECQL::BisonParser::location_type *loc, ECQL::Parser &driver) {

	ECQL::BisonParser::token_type tok = driver.scanner.lex(yylval, loc);

	return tok ;
}

// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "osm_parser.hpp"

// User implementation prologue.

#line 51 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:407
// Unqualified %code blocks.
#line 49 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:408



#include "osm_rule_parser.hpp"

	// Prototype for the yylex function
static OSM::BisonParser::symbol_type yylex(OSM::Filter::Parser &driver, OSM::BisonParser::location_type &loc);

#line 62 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:408


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 6 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:474
namespace OSM {
#line 148 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:474

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  BisonParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  BisonParser::BisonParser (OSM::Filter::Parser &driver_yyarg, OSM::BisonParser::location_type &loc_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg),
      loc (loc_yyarg)
  {}

  BisonParser::~BisonParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  BisonParser::by_state::by_state ()
    : state (empty)
  {}

  inline
  BisonParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  BisonParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  BisonParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  BisonParser::symbol_number_type
  BisonParser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  BisonParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  BisonParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 49: // action_block
      case 50: // command_list
        value.move< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 55: // command
        value.move< OSM::Filter::CommandPtr > (that.value);
        break;

      case 56: // boolean_value_expression
      case 57: // boolean_term
      case 58: // boolean_factor
      case 59: // boolean_primary
      case 60: // predicate
      case 61: // unary_predicate
      case 62: // comparison_predicate
      case 63: // like_text_predicate
      case 64: // exists_predicate
      case 65: // list_predicate
      case 66: // literal_list
      case 67: // expression
      case 68: // term
      case 69: // factor
      case 70: // function
      case 71: // function_argument_list
      case 72: // function_argument
      case 73: // literal
      case 74: // general_literal
      case 75: // boolean_literal
      case 76: // numeric_literal
      case 77: // attribute
        value.move< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 47: // rule_list
        value.move< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 48: // rule
        value.move< OSM::Filter::RulePtr > (that.value);
        break;

      case 54: // tag_decl_list
        value.move< OSM::Filter::TagDeclarationListPtr > (that.value);
        break;

      case 52: // tag_decl
        value.move< OSM::Filter::TagDeclarationPtr > (that.value);
        break;

      case 53: // tag_list
        value.move< OSM::Filter::TagListPtr > (that.value);
        break;

      case 51: // zoom_range
        value.move< OSM::Filter::ZoomRangePtr > (that.value);
        break;

      case 42: // "number"
        value.move< double > (that.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
        value.move< std::string > (that.value);
        break;

      case 44: // "zoom specifier"
        value.move< uint8_t > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty;
  }

  inline
  BisonParser::stack_symbol_type&
  BisonParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 49: // action_block
      case 50: // command_list
        value.copy< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 55: // command
        value.copy< OSM::Filter::CommandPtr > (that.value);
        break;

      case 56: // boolean_value_expression
      case 57: // boolean_term
      case 58: // boolean_factor
      case 59: // boolean_primary
      case 60: // predicate
      case 61: // unary_predicate
      case 62: // comparison_predicate
      case 63: // like_text_predicate
      case 64: // exists_predicate
      case 65: // list_predicate
      case 66: // literal_list
      case 67: // expression
      case 68: // term
      case 69: // factor
      case 70: // function
      case 71: // function_argument_list
      case 72: // function_argument
      case 73: // literal
      case 74: // general_literal
      case 75: // boolean_literal
      case 76: // numeric_literal
      case 77: // attribute
        value.copy< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 47: // rule_list
        value.copy< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 48: // rule
        value.copy< OSM::Filter::RulePtr > (that.value);
        break;

      case 54: // tag_decl_list
        value.copy< OSM::Filter::TagDeclarationListPtr > (that.value);
        break;

      case 52: // tag_decl
        value.copy< OSM::Filter::TagDeclarationPtr > (that.value);
        break;

      case 53: // tag_list
        value.copy< OSM::Filter::TagListPtr > (that.value);
        break;

      case 51: // zoom_range
        value.copy< OSM::Filter::ZoomRangePtr > (that.value);
        break;

      case 42: // "number"
        value.copy< double > (that.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
        value.copy< std::string > (that.value);
        break;

      case 44: // "zoom specifier"
        value.copy< uint8_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  BisonParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  BisonParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  BisonParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  BisonParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  BisonParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  BisonParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  BisonParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  BisonParser::debug_level_type
  BisonParser::debug_level () const
  {
    return yydebug_;
  }

  void
  BisonParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline BisonParser::state_type
  BisonParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  BisonParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  BisonParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  BisonParser::parse ()
  {
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex (driver, loc));
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 49: // action_block
      case 50: // command_list
        yylhs.value.build< OSM::Filter::CommandListPtr > ();
        break;

      case 55: // command
        yylhs.value.build< OSM::Filter::CommandPtr > ();
        break;

      case 56: // boolean_value_expression
      case 57: // boolean_term
      case 58: // boolean_factor
      case 59: // boolean_primary
      case 60: // predicate
      case 61: // unary_predicate
      case 62: // comparison_predicate
      case 63: // like_text_predicate
      case 64: // exists_predicate
      case 65: // list_predicate
      case 66: // literal_list
      case 67: // expression
      case 68: // term
      case 69: // factor
      case 70: // function
      case 71: // function_argument_list
      case 72: // function_argument
      case 73: // literal
      case 74: // general_literal
      case 75: // boolean_literal
      case 76: // numeric_literal
      case 77: // attribute
        yylhs.value.build< OSM::Filter::ExpressionNodePtr > ();
        break;

      case 47: // rule_list
        yylhs.value.build< OSM::Filter::RuleListPtr > ();
        break;

      case 48: // rule
        yylhs.value.build< OSM::Filter::RulePtr > ();
        break;

      case 54: // tag_decl_list
        yylhs.value.build< OSM::Filter::TagDeclarationListPtr > ();
        break;

      case 52: // tag_decl
        yylhs.value.build< OSM::Filter::TagDeclarationPtr > ();
        break;

      case 53: // tag_list
        yylhs.value.build< OSM::Filter::TagListPtr > ();
        break;

      case 51: // zoom_range
        yylhs.value.build< OSM::Filter::ZoomRangePtr > ();
        break;

      case 42: // "number"
        yylhs.value.build< double > ();
        break;

      case 41: // "identifier"
      case 43: // "string literal"
        yylhs.value.build< std::string > ();
        break;

      case 44: // "zoom specifier"
        yylhs.value.build< uint8_t > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 132 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_back(yystack_[0].value.as< OSM::Filter::RulePtr > ()) ; }
#line 730 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 3:
#line 133 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_front(yystack_[1].value.as< OSM::Filter::RulePtr > ()) ; }
#line 736 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 136 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::CommandListPtr > ()) ; }
#line 742 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 137 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(nullptr, yystack_[0].value.as< OSM::Filter::CommandListPtr > ()) ; }
#line 748 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 142 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[1].value.as< OSM::Filter::CommandListPtr > () ; }
#line 754 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 147 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = std::make_shared<OSM::Filter::CommandList>() ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_back(yystack_[0].value.as< OSM::Filter::CommandPtr > ()) ;  }
#line 760 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 148 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[0].value.as< OSM::Filter::CommandListPtr > () ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_front(yystack_[1].value.as< OSM::Filter::CommandPtr > ()) ; }
#line 766 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 152 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[1].value.as< uint8_t > (), 255) ; }
#line 772 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 153 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[3].value.as< uint8_t > (), yystack_[1].value.as< uint8_t > ()); }
#line 778 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 154 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[2].value.as< uint8_t > (), 255); }
#line 784 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 155 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(0, yystack_[1].value.as< uint8_t > ()); }
#line 790 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 158 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationPtr > () = std::make_shared<OSM::Filter::TagDeclaration>(yystack_[0].value.as< std::string > (), nullptr); }
#line 796 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 159 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationPtr > () = std::make_shared<OSM::Filter::TagDeclaration>(yystack_[2].value.as< std::string > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()); }
#line 802 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 162 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = std::make_shared<OSM::Filter::TagList>() ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 808 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 163 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = yystack_[2].value.as< OSM::Filter::TagListPtr > () ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 814 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 166 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationListPtr > () = std::make_shared<OSM::Filter::TagDeclarationList>() ; yylhs.value.as< OSM::Filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< OSM::Filter::TagDeclarationPtr > ()) ; }
#line 820 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 167 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationListPtr > () = yystack_[2].value.as< OSM::Filter::TagDeclarationListPtr > () ; yylhs.value.as< OSM::Filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< OSM::Filter::TagDeclarationPtr > ()) ; }
#line 826 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 171 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Add, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 832 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 173 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Set, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;}
#line 838 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 175 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Delete, yystack_[1].value.as< std::string > ()) ; }
#line 844 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 177 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagDeclarationListPtr > ()) ; }
#line 850 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 178 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::AttachCommand>(*yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 856 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 179 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagDeclarationListPtr > ()) ; }
#line 862 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 181 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>(*yystack_[1].value.as< OSM::Filter::ZoomRangePtr > ()) ; }
#line 868 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 182 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>( OSM::Filter::ZoomRange(0, 255)) ; }
#line 874 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 184 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 880 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 185 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 886 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 29:
#line 187 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>( OSM::Filter::SimpleCommand::Continue) ;}
#line 892 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 30:
#line 189 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::RuleCommand>( yystack_[0].value.as< OSM::Filter::RulePtr > ()) ;}
#line 898 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 31:
#line 194 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 904 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 32:
#line 195 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Or, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 910 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 33:
#line 199 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 916 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 34:
#line 200 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::And, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 922 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 35:
#line 204 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 928 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 36:
#line 205 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Not, yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > (), nullptr) ; }
#line 934 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 37:
#line 209 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 940 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 38:
#line 210 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 946 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 39:
#line 214 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 952 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 40:
#line 215 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 958 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 41:
#line 216 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 964 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 42:
#line 217 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 970 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 43:
#line 218 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 976 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 44:
#line 222 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::UnaryPredicate>( yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ;}
#line 982 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 45:
#line 225 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Equal, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 988 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 46:
#line 226 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::NotEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 994 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 47:
#line 227 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Less, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1000 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 48:
#line 228 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Greater, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1006 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 49:
#line 229 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::LessOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1012 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 50:
#line 230 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::GreaterOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1018 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 51:
#line 234 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), true) ; }
#line 1024 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 52:
#line 235 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), false) ; }
#line 1030 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 53:
#line 239 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExistsPredicate>(yystack_[0].value.as< std::string > ()) ; }
#line 1036 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 54:
#line 243 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[4].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > (), true) ; }
#line 1042 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 55:
#line 244 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[5].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > (), false) ; }
#line 1048 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 56:
#line 248 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExpressionNode>() ; yylhs.value.as< OSM::Filter::ExpressionNodePtr > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1054 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 57:
#line 249 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1060 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 58:
#line 253 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1066 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 59:
#line 254 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('+',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1072 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 60:
#line 255 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('.',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1078 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 61:
#line 256 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('-', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1084 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 62:
#line 260 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1090 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 63:
#line 261 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('*', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1096 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 64:
#line 262 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('/', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1102 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 65:
#line 266 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1108 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 66:
#line 267 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1114 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 67:
#line 268 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1120 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 68:
#line 269 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1126 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 69:
#line 273 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[2].value.as< std::string > ()) ; }
#line 1132 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 70:
#line 274 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
			yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;
		 }
#line 1140 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 71:
#line 280 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExpressionNode>() ; yylhs.value.as< OSM::Filter::ExpressionNodePtr > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1146 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 72:
#line 281 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1152 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 73:
#line 285 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1158 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 74:
#line 289 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1164 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 75:
#line 290 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1170 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 76:
#line 294 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(yystack_[0].value.as< std::string > ()) ; }
#line 1176 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 77:
#line 295 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1182 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 78:
#line 300 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(true) ; }
#line 1188 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 79:
#line 301 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () =  std::make_shared<OSM::Filter::LiteralExpressionNode>(false) ; }
#line 1194 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 80:
#line 305 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>((double)yystack_[0].value.as< double > ()) ;
	}
#line 1202 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 81:
#line 311 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Attribute>(yystack_[0].value.as< std::string > ()) ;
	}
#line 1210 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;


#line 1214 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  BisonParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  BisonParser::yysyntax_error_ (state_type yystate, symbol_number_type yytoken) const
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char BisonParser::yypact_ninf_ = -108;

  const signed char BisonParser::yytable_ninf_ = -1;

  const short int
  BisonParser::yypact_[] =
  {
      89,    70,     7,    18,    89,  -108,   -16,    12,    23,    40,
      42,    50,    67,    52,  -108,    85,    70,    -7,  -108,  -108,
      74,     7,     3,  -108,  -108,    10,   127,  -108,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,   113,    88,    56,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,    82,    97,   106,    -2,
     101,    96,  -108,    -3,  -108,  -108,    52,    69,  -108,   109,
      84,  -108,  -108,  -108,  -108,     8,    77,   103,   -10,   119,
       7,   116,     7,    99,   100,    31,    31,    31,    31,    31,
      31,    31,    31,    31,    31,    31,    31,    31,  -108,   102,
      -1,    31,   104,    96,  -108,   105,   107,  -108,  -108,  -108,
    -108,  -108,   123,    31,  -108,   124,  -108,   125,   126,    26,
     127,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,  -108,
    -108,  -108,  -108,  -108,  -108,  -108,   121,   122,   128,   -25,
    -108,  -108,  -108,  -108,  -108,  -108,    26,   130,  -108,    31,
     131,   132,  -108,  -108,  -108,  -108,   129,   136,  -108,  -108,
      26,  -108,  -108,  -108
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     0,     2,     5,     0,     0,     0,     0,
       0,     0,     0,     0,    30,     0,     7,     0,    78,    79,
       0,     0,    81,    80,    76,     0,    31,    33,    35,    37,
      39,    40,    41,    42,    43,    44,    58,    62,    65,    66,
      75,    77,    74,    67,     1,     3,     0,     0,     0,     0,
      13,     0,    17,     0,    29,    15,     0,     0,    26,     0,
       0,     6,     8,    36,    53,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,    24,     0,     0,    28,    25,    23,
      38,    68,     0,     0,    69,    81,    73,     0,    71,     0,
      32,     4,    34,    51,    52,    45,    46,    47,    48,    49,
      50,    59,    61,    60,    63,    64,     0,     0,     0,     0,
       9,    14,    22,    18,    27,    16,     0,     0,    70,     0,
       0,    56,    19,    20,    12,    11,     0,     0,    72,    54,
       0,    10,    55,    57
  };

  const short int
  BisonParser::yypgoto_[] =
  {
    -108,   150,    62,    90,   143,    68,    71,   -12,   111,  -108,
     139,    93,    94,   148,  -108,  -108,  -108,  -108,  -108,  -108,
     -99,   -21,    45,  -108,  -108,    28,  -108,  -107,  -108,  -108,
    -108,  -108
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,     3,     4,     5,    15,    51,    52,    57,    53,    16,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     140,    35,    36,    37,    38,   107,   108,    39,    40,    41,
      42,    43
  };

  const unsigned char
  BisonParser::yytable_[] =
  {
      66,    60,   141,   145,    18,    19,    67,    18,    19,    20,
      17,   103,   104,    70,    21,    70,    89,   129,    44,   146,
      93,    18,    19,    20,    68,    46,    94,   130,    21,   141,
     100,   105,    23,    24,    22,    23,    24,   147,    71,    69,
      18,    19,    90,   141,    95,    18,    19,   106,    22,    23,
      24,   153,   103,    47,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    14,    48,   126,   127,    49,    23,    24,
     131,    54,   105,    23,    24,    84,    85,    49,    14,    56,
      59,    50,   137,    73,    74,    75,    76,    77,    78,    79,
      80,    55,    96,    55,    49,     1,    58,     2,    97,   101,
       6,     7,     8,     9,    10,    81,    82,    96,    11,    12,
      13,    61,    83,    99,     1,    64,     2,    86,   106,    73,
      74,    75,    76,    77,    78,    79,    80,    93,    96,   124,
     125,    72,    87,   132,   134,    88,    91,    50,    98,   102,
     109,     1,   113,   114,   136,    68,   128,   138,   135,   139,
     142,   143,   101,   149,    45,   150,   144,   151,   152,    62,
      65,   111,    92,   110,   133,    63,   112,   148
  };

  const unsigned char
  BisonParser::yycheck_[] =
  {
      21,    13,   109,    28,    14,    15,     3,    14,    15,    16,
       3,    21,    22,     5,    21,     5,    18,    18,     0,    44,
      23,    14,    15,    16,    21,    41,    29,    28,    21,   136,
      22,    41,    42,    43,    41,    42,    43,   136,    28,    36,
      14,    15,    44,   150,    56,    14,    15,    68,    41,    42,
      43,   150,    21,    41,    75,    76,    77,    78,    79,    80,
      81,    82,    83,     1,    41,    86,    87,    27,    42,    43,
      91,    29,    41,    42,    43,    19,    20,    27,    16,    11,
      12,    41,   103,     6,     7,     8,     9,    10,    11,    12,
      13,    41,    23,    41,    27,    25,    29,    27,    29,    22,
      30,    31,    32,    33,    34,    17,    18,    23,    38,    39,
      40,    26,    24,    29,    25,    41,    27,    35,   139,     6,
       7,     8,     9,    10,    11,    12,    13,    23,    23,    84,
      85,     4,    35,    29,    29,    29,    35,    41,    29,    36,
      21,    25,    43,    43,    21,    21,    44,    22,    41,    23,
      29,    29,    22,    22,     4,    23,    28,    28,    22,    16,
      21,    71,    51,    70,    93,    17,    72,   139
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,    25,    27,    47,    48,    49,    30,    31,    32,    33,
      34,    38,    39,    40,    48,    50,    55,     3,    14,    15,
      16,    21,    41,    42,    43,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    67,    68,    69,    70,    73,
      74,    75,    76,    77,     0,    47,    41,    41,    41,    27,
      41,    51,    52,    54,    29,    41,    51,    53,    29,    51,
      53,    26,    50,    59,    41,    56,    67,     3,    21,    36,
       5,    28,     4,     6,     7,     8,     9,    10,    11,    12,
      13,    17,    18,    24,    19,    20,    35,    35,    29,    18,
      44,    35,    54,    23,    29,    53,    23,    29,    29,    29,
      22,    22,    36,    21,    22,    41,    67,    71,    72,    21,
      57,    49,    58,    43,    43,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    68,    68,    67,    67,    44,    18,
      28,    67,    29,    52,    29,    41,    21,    67,    22,    23,
      66,    73,    29,    29,    28,    28,    44,    66,    71,    22,
      23,    28,    22,    66
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    46,    47,    47,    48,    48,    49,    50,    50,    51,
      51,    51,    51,    52,    52,    53,    53,    54,    54,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    56,    56,    57,    57,    58,    58,    59,    59,    60,
      60,    60,    60,    60,    61,    62,    62,    62,    62,    62,
      62,    63,    63,    64,    65,    65,    66,    66,    67,    67,
      67,    67,    68,    68,    68,    69,    69,    69,    69,    70,
      70,    71,    71,    72,    73,    73,    74,    74,    75,    75,
      76,    77
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     2,     4,     1,     3,     1,     2,     3,
       5,     4,     4,     1,     3,     1,     3,     1,     3,     5,
       5,     3,     4,     3,     3,     3,     2,     4,     3,     2,
       1,     1,     3,     1,     3,     1,     2,     1,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     5,     6,     1,     3,     1,     3,
       3,     3,     1,     3,     3,     1,     1,     1,     3,     3,
       4,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const BisonParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"!\"", "\"&&\"", "\"||\"",
  "\"~\"", "\"!~\"", "\"==\"", "\"!=\"", "\"<\"", "\">\"", "\"<=\"",
  "\">=\"", "\"true\"", "\"false\"", "\"^\"", "\"+\"", "\"-\"", "\"*\"",
  "\"/\"", "\"(\"", "\")\"", "\",\"", "\".\"", "\"{\"", "\"}\"", "\"[\"",
  "\"]\"", "\";\"", "\"add tag\"", "\"set tag\"", "\"delete tag\"",
  "\"write\"", "\"continue\"", "\"=\"", "\"in\"", "\"@layer\"",
  "\"exclude\"", "\"write all\"", "\"attach\"", "\"identifier\"",
  "\"number\"", "\"string literal\"", "\"zoom specifier\"", "UMINUS",
  "$accept", "rule_list", "rule", "action_block", "command_list",
  "zoom_range", "tag_decl", "tag_list", "tag_decl_list", "command",
  "boolean_value_expression", "boolean_term", "boolean_factor",
  "boolean_primary", "predicate", "unary_predicate",
  "comparison_predicate", "like_text_predicate", "exists_predicate",
  "list_predicate", "literal_list", "expression", "term", "factor",
  "function", "function_argument_list", "function_argument", "literal",
  "general_literal", "boolean_literal", "numeric_literal", "attribute", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  BisonParser::yyrline_[] =
  {
       0,   132,   132,   133,   136,   137,   142,   147,   148,   152,
     153,   154,   155,   158,   159,   162,   163,   166,   167,   171,
     173,   175,   177,   178,   179,   181,   182,   184,   185,   187,
     189,   194,   195,   199,   200,   204,   205,   209,   210,   214,
     215,   216,   217,   218,   222,   225,   226,   227,   228,   229,
     230,   234,   235,   239,   243,   244,   248,   249,   253,   254,
     255,   256,   260,   261,   262,   266,   267,   268,   269,   273,
     274,   280,   281,   285,   289,   290,   294,   295,   300,   301,
     305,   311
  };

  // Print the state stack on the debug stream.
  void
  BisonParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BisonParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 6 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:1155
} // OSM
#line 1698 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:1155
#line 318 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:1156

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



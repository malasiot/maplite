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
#line 44 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:408



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
      case 48: // action_block
      case 49: // command_list
        value.move< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 52: // command
        value.move< OSM::Filter::CommandPtr > (that.value);
        break;

      case 53: // boolean_value_expression
      case 54: // boolean_term
      case 55: // boolean_factor
      case 56: // boolean_primary
      case 57: // predicate
      case 58: // comparison_predicate
      case 59: // like_text_predicate
      case 60: // exists_predicate
      case 61: // list_predicate
      case 62: // literal_list
      case 63: // expression
      case 64: // term
      case 65: // factor
      case 66: // function
      case 67: // function_argument_list
      case 68: // function_argument
      case 69: // literal
      case 70: // general_literal
      case 71: // boolean_literal
      case 72: // numeric_literal
      case 73: // attribute
        value.move< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 46: // rule_list
        value.move< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 47: // rule
        value.move< OSM::Filter::RulePtr > (that.value);
        break;

      case 51: // tag_list
        value.move< OSM::Filter::TagListPtr > (that.value);
        break;

      case 50: // zoom_range
        value.move< OSM::Filter::ZoomRangePtr > (that.value);
        break;

      case 41: // "number"
        value.move< double > (that.value);
        break;

      case 40: // "identifier"
      case 42: // "string literal"
        value.move< std::string > (that.value);
        break;

      case 43: // "zoom specifier"
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
      case 48: // action_block
      case 49: // command_list
        value.copy< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 52: // command
        value.copy< OSM::Filter::CommandPtr > (that.value);
        break;

      case 53: // boolean_value_expression
      case 54: // boolean_term
      case 55: // boolean_factor
      case 56: // boolean_primary
      case 57: // predicate
      case 58: // comparison_predicate
      case 59: // like_text_predicate
      case 60: // exists_predicate
      case 61: // list_predicate
      case 62: // literal_list
      case 63: // expression
      case 64: // term
      case 65: // factor
      case 66: // function
      case 67: // function_argument_list
      case 68: // function_argument
      case 69: // literal
      case 70: // general_literal
      case 71: // boolean_literal
      case 72: // numeric_literal
      case 73: // attribute
        value.copy< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 46: // rule_list
        value.copy< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 47: // rule
        value.copy< OSM::Filter::RulePtr > (that.value);
        break;

      case 51: // tag_list
        value.copy< OSM::Filter::TagListPtr > (that.value);
        break;

      case 50: // zoom_range
        value.copy< OSM::Filter::ZoomRangePtr > (that.value);
        break;

      case 41: // "number"
        value.copy< double > (that.value);
        break;

      case 40: // "identifier"
      case 42: // "string literal"
        value.copy< std::string > (that.value);
        break;

      case 43: // "zoom specifier"
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
      case 48: // action_block
      case 49: // command_list
        yylhs.value.build< OSM::Filter::CommandListPtr > ();
        break;

      case 52: // command
        yylhs.value.build< OSM::Filter::CommandPtr > ();
        break;

      case 53: // boolean_value_expression
      case 54: // boolean_term
      case 55: // boolean_factor
      case 56: // boolean_primary
      case 57: // predicate
      case 58: // comparison_predicate
      case 59: // like_text_predicate
      case 60: // exists_predicate
      case 61: // list_predicate
      case 62: // literal_list
      case 63: // expression
      case 64: // term
      case 65: // factor
      case 66: // function
      case 67: // function_argument_list
      case 68: // function_argument
      case 69: // literal
      case 70: // general_literal
      case 71: // boolean_literal
      case 72: // numeric_literal
      case 73: // attribute
        yylhs.value.build< OSM::Filter::ExpressionNodePtr > ();
        break;

      case 46: // rule_list
        yylhs.value.build< OSM::Filter::RuleListPtr > ();
        break;

      case 47: // rule
        yylhs.value.build< OSM::Filter::RulePtr > ();
        break;

      case 51: // tag_list
        yylhs.value.build< OSM::Filter::TagListPtr > ();
        break;

      case 50: // zoom_range
        yylhs.value.build< OSM::Filter::ZoomRangePtr > ();
        break;

      case 41: // "number"
        yylhs.value.build< double > ();
        break;

      case 40: // "identifier"
      case 42: // "string literal"
        yylhs.value.build< std::string > ();
        break;

      case 43: // "zoom specifier"
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
#line 125 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_back(yystack_[0].value.as< OSM::Filter::RulePtr > ()) ; }
#line 703 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 3:
#line 126 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_front(yystack_[1].value.as< OSM::Filter::RulePtr > ()) ; }
#line 709 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 129 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::CommandListPtr > ()) ; }
#line 715 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 130 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(nullptr, yystack_[0].value.as< OSM::Filter::CommandListPtr > ()) ; }
#line 721 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 135 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[1].value.as< OSM::Filter::CommandListPtr > () ; }
#line 727 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 140 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = std::make_shared<OSM::Filter::CommandList>() ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_back(yystack_[0].value.as< OSM::Filter::CommandPtr > ()) ;  }
#line 733 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 141 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[0].value.as< OSM::Filter::CommandListPtr > () ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_front(yystack_[1].value.as< OSM::Filter::CommandPtr > ()) ; }
#line 739 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 145 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[3].value.as< uint8_t > (), yystack_[1].value.as< uint8_t > ()); }
#line 745 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 146 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[2].value.as< uint8_t > (), 255); }
#line 751 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 147 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(0, yystack_[1].value.as< uint8_t > ()); }
#line 757 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 150 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = std::make_shared<OSM::Filter::TagList>() ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 763 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 151 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = yystack_[2].value.as< OSM::Filter::TagListPtr > () ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 769 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 154 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Add, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 775 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 156 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Set, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;}
#line 781 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 158 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Delete, yystack_[1].value.as< std::string > ()) ; }
#line 787 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 160 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 793 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 161 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 799 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 163 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>(*yystack_[1].value.as< OSM::Filter::ZoomRangePtr > ()) ; }
#line 805 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 164 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>( OSM::Filter::ZoomRange(0, 255)) ; }
#line 811 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 166 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 817 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 167 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 823 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 169 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>( OSM::Filter::SimpleCommand::Continue) ;}
#line 829 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 171 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::RuleCommand>( yystack_[0].value.as< OSM::Filter::RulePtr > ()) ;}
#line 835 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 182 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 841 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 183 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Or, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 847 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 187 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 853 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 188 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::And, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 859 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 29:
#line 192 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 865 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 30:
#line 193 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Not, yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > (), nullptr) ; }
#line 871 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 31:
#line 197 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 877 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 32:
#line 198 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 883 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 33:
#line 202 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 889 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 34:
#line 203 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 895 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 35:
#line 204 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 901 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 36:
#line 205 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 907 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 37:
#line 210 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Equal, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 913 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 38:
#line 211 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::NotEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 919 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 39:
#line 212 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Less, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 925 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 40:
#line 213 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Greater, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 931 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 41:
#line 214 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::LessOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 937 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 42:
#line 215 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::GreaterOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 943 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 43:
#line 219 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), true) ; }
#line 949 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 44:
#line 220 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), false) ; }
#line 955 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 45:
#line 224 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExistsPredicate>(yystack_[0].value.as< std::string > ()) ; }
#line 961 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 46:
#line 228 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[4].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > (), true) ; }
#line 967 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 47:
#line 229 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[5].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > (), false) ; }
#line 973 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 48:
#line 233 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExpressionNode>() ; yylhs.value.as< OSM::Filter::ExpressionNodePtr > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 979 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 49:
#line 234 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 985 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 50:
#line 238 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 991 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 51:
#line 239 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('+',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 997 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 52:
#line 240 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('.',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1003 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 53:
#line 241 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('-', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1009 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 54:
#line 245 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1015 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 55:
#line 246 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('*', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1021 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 56:
#line 247 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('/', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1027 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 57:
#line 251 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1033 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 58:
#line 252 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1039 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 59:
#line 253 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1045 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 60:
#line 254 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1051 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 61:
#line 258 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[2].value.as< std::string > ()) ; }
#line 1057 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 62:
#line 259 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
			yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;
		 }
#line 1065 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 63:
#line 265 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExpressionNode>() ; yylhs.value.as< OSM::Filter::ExpressionNodePtr > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1071 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 64:
#line 266 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1077 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 65:
#line 270 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1083 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 66:
#line 274 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1089 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 67:
#line 275 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1095 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 68:
#line 279 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(yystack_[0].value.as< std::string > ()) ; }
#line 1101 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 69:
#line 280 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1107 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 70:
#line 285 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(true) ; }
#line 1113 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 71:
#line 286 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () =  std::make_shared<OSM::Filter::LiteralExpressionNode>(false) ; }
#line 1119 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 72:
#line 290 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>((double)yystack_[0].value.as< double > ()) ;
	}
#line 1127 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 73:
#line 296 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Attribute>(yystack_[0].value.as< std::string > ()) ;
	}
#line 1135 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;


#line 1139 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
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


  const signed char BisonParser::yypact_ninf_ = -111;

  const signed char BisonParser::yytable_ninf_ = -1;

  const short int
  BisonParser::yypact_[] =
  {
     -11,    80,    24,    13,   -11,  -111,    -6,    19,    20,   -10,
      34,   -10,    51,  -111,    41,    80,    -9,  -111,  -111,    31,
      24,    -1,  -111,  -111,    56,    78,    89,  -111,  -111,  -111,
    -111,  -111,  -111,   114,     1,     4,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,    59,    67,    74,     3,  -111,    64,
      45,  -111,    64,    86,  -111,    77,  -111,  -111,  -111,  -111,
      94,    79,    81,    55,   109,   107,    24,    24,    91,    95,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,  -111,    88,   118,   105,    98,  -111,   106,
    -111,  -111,  -111,  -111,   119,    58,  -111,   120,  -111,   117,
     121,    -5,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,   113,   116,
     115,   -17,  -111,  -111,  -111,    -5,   124,  -111,    58,   125,
     126,  -111,  -111,  -111,  -111,   122,   129,  -111,  -111,    -5,
    -111,  -111,  -111
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     0,     2,     5,     0,     0,     0,     0,
       0,     0,     0,    24,     0,     7,     0,    70,    71,     0,
       0,    73,    72,    68,     0,    25,    27,    29,    31,    33,
      34,    35,    36,     0,    50,    54,    57,    58,    67,    69,
      66,    59,     1,     3,     0,     0,     0,     0,    12,     0,
       0,    23,     0,     0,    20,     0,     6,     8,    30,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    16,     0,     0,     0,     0,    18,     0,
      22,    19,    32,    60,     0,     0,    61,    73,    65,     0,
      63,     0,     4,    26,    28,    43,    44,    37,    38,    39,
      40,    41,    42,    51,    53,    52,    55,    56,     0,     0,
       0,     0,    17,    13,    21,     0,     0,    62,     0,     0,
      48,    14,    15,    11,    10,     0,     0,    64,    46,     0,
       9,    47,    49
  };

  const short int
  BisonParser::yypgoto_[] =
  {
    -111,   144,     7,    87,   138,    37,    -8,  -111,   -19,    90,
    -111,   139,  -111,  -111,  -111,  -111,  -111,  -110,   -20,     2,
    -111,  -111,    26,  -111,   -97,  -111,  -111,  -111,  -111
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,     3,     4,     5,    14,    49,    50,    15,    24,    25,
      26,    27,    28,    29,    30,    31,    32,   129,    33,    34,
      35,    36,    99,   100,    37,    38,    39,    40,    41
  };

  const unsigned char
  BisonParser::yytable_[] =
  {
      61,    60,    62,    53,   130,    17,    18,    19,    13,    17,
      18,   134,    20,    42,     1,   136,     2,    47,    76,    77,
      63,    84,    13,    79,    80,    78,   135,    16,   130,   142,
      48,    21,    22,    23,    44,    64,    22,    23,    17,    18,
      19,    86,   130,    98,    89,    20,    85,   103,    52,    55,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    45,
      46,   118,   119,    51,    21,    22,    23,    56,    87,    17,
      18,    59,    17,    18,    88,   126,    95,    96,    47,    95,
      54,   116,   117,    66,    65,    68,    69,    70,    71,    72,
      73,    74,    75,    67,    81,    97,    22,    23,    97,    22,
      23,    93,    82,    83,    48,     1,    91,     2,    98,    87,
       6,     7,     8,     9,    10,    90,    92,    94,    11,    12,
      68,    69,    70,    71,    72,    73,    74,    75,    87,    87,
     101,   120,     1,   105,   122,   124,   121,   106,   123,   127,
     125,    63,   131,   133,   128,   132,    93,   138,    43,   139,
     140,   141,   102,    57,   137,    58,     0,   104
  };

  const short int
  BisonParser::yycheck_[] =
  {
      20,    20,     3,    11,   101,    14,    15,    16,     1,    14,
      15,    28,    21,     0,    25,   125,    27,    27,    17,    18,
      21,    18,    15,    19,    20,    24,    43,     3,   125,   139,
      40,    40,    41,    42,    40,    36,    41,    42,    14,    15,
      16,    49,   139,    63,    52,    21,    43,    66,    11,    12,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    40,
      40,    81,    82,    29,    40,    41,    42,    26,    23,    14,
      15,    40,    14,    15,    29,    95,    21,    22,    27,    21,
      29,    79,    80,     5,    28,     6,     7,     8,     9,    10,
      11,    12,    13,     4,    35,    40,    41,    42,    40,    41,
      42,    22,    35,    29,    40,    25,    29,    27,   128,    23,
      30,    31,    32,    33,    34,    29,    22,    36,    38,    39,
       6,     7,     8,     9,    10,    11,    12,    13,    23,    23,
      21,    43,    25,    42,    29,    29,    18,    42,    40,    22,
      21,    21,    29,    28,    23,    29,    22,    22,     4,    23,
      28,    22,    65,    15,   128,    16,    -1,    67
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,    25,    27,    46,    47,    48,    30,    31,    32,    33,
      34,    38,    39,    47,    49,    52,     3,    14,    15,    16,
      21,    40,    41,    42,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    63,    64,    65,    66,    69,    70,    71,
      72,    73,     0,    46,    40,    40,    40,    27,    40,    50,
      51,    29,    50,    51,    29,    50,    26,    49,    56,    40,
      53,    63,     3,    21,    36,    28,     5,     4,     6,     7,
       8,     9,    10,    11,    12,    13,    17,    18,    24,    19,
      20,    35,    35,    29,    18,    43,    51,    23,    29,    51,
      29,    29,    22,    22,    36,    21,    22,    40,    63,    67,
      68,    21,    48,    53,    54,    42,    42,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    64,    64,    63,    63,
      43,    18,    29,    40,    29,    21,    63,    22,    23,    62,
      69,    29,    29,    28,    28,    43,    62,    67,    22,    23,
      28,    22,    62
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    45,    46,    46,    47,    47,    48,    49,    49,    50,
      50,    50,    51,    51,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    53,    53,    54,    54,    55,
      55,    56,    56,    57,    57,    57,    57,    58,    58,    58,
      58,    58,    58,    59,    59,    60,    61,    61,    62,    62,
      63,    63,    63,    63,    64,    64,    64,    65,    65,    65,
      65,    66,    66,    67,    67,    68,    69,    69,    70,    70,
      71,    71,    72,    73
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     2,     4,     1,     3,     1,     2,     5,
       4,     4,     1,     3,     5,     5,     3,     4,     3,     3,
       2,     4,     3,     2,     1,     1,     3,     1,     3,     1,
       2,     1,     3,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     5,     6,     1,     3,
       1,     3,     3,     3,     1,     3,     3,     1,     1,     1,
       3,     3,     4,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1
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
  "\"exclude\"", "\"write all\"", "\"identifier\"", "\"number\"",
  "\"string literal\"", "\"zoom specifier\"", "UMINUS", "$accept",
  "rule_list", "rule", "action_block", "command_list", "zoom_range",
  "tag_list", "command", "boolean_value_expression", "boolean_term",
  "boolean_factor", "boolean_primary", "predicate", "comparison_predicate",
  "like_text_predicate", "exists_predicate", "list_predicate",
  "literal_list", "expression", "term", "factor", "function",
  "function_argument_list", "function_argument", "literal",
  "general_literal", "boolean_literal", "numeric_literal", "attribute", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  BisonParser::yyrline_[] =
  {
       0,   125,   125,   126,   129,   130,   135,   140,   141,   145,
     146,   147,   150,   151,   154,   156,   158,   160,   161,   163,
     164,   166,   167,   169,   171,   182,   183,   187,   188,   192,
     193,   197,   198,   202,   203,   204,   205,   210,   211,   212,
     213,   214,   215,   219,   220,   224,   228,   229,   233,   234,
     238,   239,   240,   241,   245,   246,   247,   251,   252,   253,
     254,   258,   259,   265,   266,   270,   274,   275,   279,   280,
     285,   286,   290,   296
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
#line 1612 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:1155
#line 303 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:1156

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



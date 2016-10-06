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
      case 48: // action_block
      case 49: // command_list
        value.move< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 54: // command
        value.move< OSM::Filter::CommandPtr > (that.value);
        break;

      case 55: // boolean_value_expression
      case 56: // boolean_term
      case 57: // boolean_factor
      case 58: // boolean_primary
      case 59: // predicate
      case 60: // comparison_predicate
      case 61: // like_text_predicate
      case 62: // exists_predicate
      case 63: // list_predicate
      case 64: // literal_list
      case 65: // expression
      case 66: // term
      case 67: // factor
      case 68: // function
      case 69: // function_argument_list
      case 70: // function_argument
      case 71: // literal
      case 72: // general_literal
      case 73: // boolean_literal
      case 74: // numeric_literal
      case 75: // attribute
        value.move< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 46: // rule_list
        value.move< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 47: // rule
        value.move< OSM::Filter::RulePtr > (that.value);
        break;

      case 53: // tag_decl_list
        value.move< OSM::Filter::TagDeclarationListPtr > (that.value);
        break;

      case 51: // tag_decl
        value.move< OSM::Filter::TagDeclarationPtr > (that.value);
        break;

      case 52: // tag_list
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

      case 54: // command
        value.copy< OSM::Filter::CommandPtr > (that.value);
        break;

      case 55: // boolean_value_expression
      case 56: // boolean_term
      case 57: // boolean_factor
      case 58: // boolean_primary
      case 59: // predicate
      case 60: // comparison_predicate
      case 61: // like_text_predicate
      case 62: // exists_predicate
      case 63: // list_predicate
      case 64: // literal_list
      case 65: // expression
      case 66: // term
      case 67: // factor
      case 68: // function
      case 69: // function_argument_list
      case 70: // function_argument
      case 71: // literal
      case 72: // general_literal
      case 73: // boolean_literal
      case 74: // numeric_literal
      case 75: // attribute
        value.copy< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 46: // rule_list
        value.copy< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 47: // rule
        value.copy< OSM::Filter::RulePtr > (that.value);
        break;

      case 53: // tag_decl_list
        value.copy< OSM::Filter::TagDeclarationListPtr > (that.value);
        break;

      case 51: // tag_decl
        value.copy< OSM::Filter::TagDeclarationPtr > (that.value);
        break;

      case 52: // tag_list
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

      case 54: // command
        yylhs.value.build< OSM::Filter::CommandPtr > ();
        break;

      case 55: // boolean_value_expression
      case 56: // boolean_term
      case 57: // boolean_factor
      case 58: // boolean_primary
      case 59: // predicate
      case 60: // comparison_predicate
      case 61: // like_text_predicate
      case 62: // exists_predicate
      case 63: // list_predicate
      case 64: // literal_list
      case 65: // expression
      case 66: // term
      case 67: // factor
      case 68: // function
      case 69: // function_argument_list
      case 70: // function_argument
      case 71: // literal
      case 72: // general_literal
      case 73: // boolean_literal
      case 74: // numeric_literal
      case 75: // attribute
        yylhs.value.build< OSM::Filter::ExpressionNodePtr > ();
        break;

      case 46: // rule_list
        yylhs.value.build< OSM::Filter::RuleListPtr > ();
        break;

      case 47: // rule
        yylhs.value.build< OSM::Filter::RulePtr > ();
        break;

      case 53: // tag_decl_list
        yylhs.value.build< OSM::Filter::TagDeclarationListPtr > ();
        break;

      case 51: // tag_decl
        yylhs.value.build< OSM::Filter::TagDeclarationPtr > ();
        break;

      case 52: // tag_list
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
#line 131 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_back(yystack_[0].value.as< OSM::Filter::RulePtr > ()) ; }
#line 727 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 3:
#line 132 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_front(yystack_[1].value.as< OSM::Filter::RulePtr > ()) ; }
#line 733 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 135 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::CommandListPtr > ()) ; }
#line 739 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 136 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(nullptr, yystack_[0].value.as< OSM::Filter::CommandListPtr > ()) ; }
#line 745 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 141 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[1].value.as< OSM::Filter::CommandListPtr > () ; }
#line 751 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 146 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = std::make_shared<OSM::Filter::CommandList>() ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_back(yystack_[0].value.as< OSM::Filter::CommandPtr > ()) ;  }
#line 757 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 147 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[0].value.as< OSM::Filter::CommandListPtr > () ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_front(yystack_[1].value.as< OSM::Filter::CommandPtr > ()) ; }
#line 763 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 151 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[3].value.as< uint8_t > (), yystack_[1].value.as< uint8_t > ()); }
#line 769 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 152 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[2].value.as< uint8_t > (), 255); }
#line 775 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 153 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(0, yystack_[1].value.as< uint8_t > ()); }
#line 781 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 156 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationPtr > () = std::make_shared<OSM::Filter::TagDeclaration>(yystack_[0].value.as< std::string > (), nullptr); }
#line 787 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 157 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationPtr > () = std::make_shared<OSM::Filter::TagDeclaration>(yystack_[2].value.as< std::string > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()); }
#line 793 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 160 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = std::make_shared<OSM::Filter::TagList>() ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 799 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 161 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = yystack_[2].value.as< OSM::Filter::TagListPtr > () ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 805 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 164 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationListPtr > () = std::make_shared<OSM::Filter::TagDeclarationList>() ; yylhs.value.as< OSM::Filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< OSM::Filter::TagDeclarationPtr > ()) ; }
#line 811 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 165 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationListPtr > () = yystack_[2].value.as< OSM::Filter::TagDeclarationListPtr > () ; yylhs.value.as< OSM::Filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< OSM::Filter::TagDeclarationPtr > ()) ; }
#line 817 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 169 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Add, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 823 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 171 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Set, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;}
#line 829 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 173 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Delete, yystack_[1].value.as< std::string > ()) ; }
#line 835 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 175 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagDeclarationListPtr > ()) ; }
#line 841 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 176 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagDeclarationListPtr > ()) ; }
#line 847 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 178 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>(*yystack_[1].value.as< OSM::Filter::ZoomRangePtr > ()) ; }
#line 853 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 179 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>( OSM::Filter::ZoomRange(0, 255)) ; }
#line 859 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 181 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 865 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 182 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 871 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 184 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>( OSM::Filter::SimpleCommand::Continue) ;}
#line 877 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 186 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::RuleCommand>( yystack_[0].value.as< OSM::Filter::RulePtr > ()) ;}
#line 883 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 29:
#line 197 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 889 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 30:
#line 198 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Or, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 895 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 31:
#line 202 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 901 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 32:
#line 203 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::And, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 907 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 33:
#line 207 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 913 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 34:
#line 208 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Not, yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > (), nullptr) ; }
#line 919 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 35:
#line 212 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 925 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 36:
#line 213 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 931 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 37:
#line 217 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 937 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 38:
#line 218 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 943 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 39:
#line 219 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 949 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 40:
#line 220 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 955 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 41:
#line 225 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Equal, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 961 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 42:
#line 226 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::NotEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 967 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 43:
#line 227 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Less, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 973 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 44:
#line 228 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Greater, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 979 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 45:
#line 229 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::LessOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 985 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 46:
#line 230 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::GreaterOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 991 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 47:
#line 234 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), true) ; }
#line 997 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 48:
#line 235 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), false) ; }
#line 1003 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 49:
#line 239 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExistsPredicate>(yystack_[0].value.as< std::string > ()) ; }
#line 1009 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 50:
#line 243 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[4].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > (), true) ; }
#line 1015 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 51:
#line 244 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[5].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > (), false) ; }
#line 1021 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 52:
#line 248 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExpressionNode>() ; yylhs.value.as< OSM::Filter::ExpressionNodePtr > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1027 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 53:
#line 249 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1033 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 54:
#line 253 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1039 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 55:
#line 254 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('+',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1045 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 56:
#line 255 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('.',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1051 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 57:
#line 256 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('-', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1057 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 58:
#line 260 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1063 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 59:
#line 261 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('*', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1069 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 60:
#line 262 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('/', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1075 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 61:
#line 266 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1081 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 62:
#line 267 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1087 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 63:
#line 268 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1093 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 64:
#line 269 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1099 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 65:
#line 273 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[2].value.as< std::string > ()) ; }
#line 1105 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 66:
#line 274 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
			yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;
		 }
#line 1113 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 67:
#line 280 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ExpressionNode>() ; yylhs.value.as< OSM::Filter::ExpressionNodePtr > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1119 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 68:
#line 281 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1125 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 69:
#line 285 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1131 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 70:
#line 289 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1137 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 71:
#line 290 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1143 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 72:
#line 294 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(yystack_[0].value.as< std::string > ()) ; }
#line 1149 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 73:
#line 295 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1155 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 74:
#line 300 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(true) ; }
#line 1161 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 75:
#line 301 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () =  std::make_shared<OSM::Filter::LiteralExpressionNode>(false) ; }
#line 1167 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 76:
#line 305 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>((double)yystack_[0].value.as< double > ()) ;
	}
#line 1175 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 77:
#line 311 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Attribute>(yystack_[0].value.as< std::string > ()) ;
	}
#line 1183 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;


#line 1187 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
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


  const signed char BisonParser::yypact_ninf_ = -104;

  const signed char BisonParser::yytable_ninf_ = -1;

  const short int
  BisonParser::yypact_[] =
  {
      47,    67,     6,    15,    47,  -104,    22,    33,    37,    49,
      59,    51,   -13,  -104,    64,    67,    -8,  -104,  -104,    53,
       6,     2,  -104,  -104,    85,    90,   103,  -104,  -104,  -104,
    -104,  -104,  -104,   109,     0,    16,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,    91,    92,    96,    -6,    93,    89,
    -104,    -4,  -104,  -104,    94,    79,  -104,   101,  -104,  -104,
    -104,  -104,   110,    74,    95,   -11,   112,   111,     6,     6,
      97,    98,    29,    29,    29,    29,    29,    29,    29,    29,
      29,    29,    29,    29,    29,  -104,    99,   117,    29,    80,
      89,  -104,    81,   104,  -104,  -104,  -104,  -104,   116,    29,
    -104,   120,  -104,   121,   115,    25,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,  -104,   118,   119,   122,    -2,  -104,  -104,  -104,  -104,
    -104,    25,   123,  -104,    29,   124,   126,  -104,  -104,  -104,
    -104,   125,   129,  -104,  -104,    25,  -104,  -104,  -104
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     0,     2,     5,     0,     0,     0,     0,
       0,     0,     0,    28,     0,     7,     0,    74,    75,     0,
       0,    77,    76,    72,     0,    29,    31,    33,    35,    37,
      38,    39,    40,     0,    54,    58,    61,    62,    71,    73,
      70,    63,     1,     3,     0,     0,     0,     0,    12,     0,
      16,     0,    27,    14,     0,     0,    24,     0,     6,     8,
      34,    49,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,     0,     0,
       0,    22,     0,     0,    26,    23,    36,    64,     0,     0,
      65,    77,    69,     0,    67,     0,     4,    30,    32,    47,
      48,    41,    42,    43,    44,    45,    46,    55,    57,    56,
      59,    60,     0,     0,     0,     0,    13,    21,    17,    25,
      15,     0,     0,    66,     0,     0,    52,    18,    19,    11,
      10,     0,     0,    68,    50,     0,     9,    51,    53
  };

  const short int
  BisonParser::yypgoto_[] =
  {
    -104,   148,    50,    87,   140,   100,    66,   105,   108,  -104,
     -19,   102,  -104,   142,  -104,  -104,  -104,  -104,  -104,   -70,
     -20,    42,  -104,  -104,    26,  -104,  -103,  -104,  -104,  -104,
    -104
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,     3,     4,     5,    14,    49,    50,    55,    51,    15,
      24,    25,    26,    27,    28,    29,    30,    31,    32,   135,
      33,    34,    35,    36,   103,   104,    37,    38,    39,    40,
      41
  };

  const unsigned char
  BisonParser::yytable_[] =
  {
      63,    62,   136,    17,    18,    64,    17,    18,    19,    16,
      99,   100,    86,    20,    47,    42,    56,    78,    79,    90,
      17,    18,    19,    65,    80,    91,   140,    20,   136,   101,
      22,    23,    21,    22,    23,    81,    82,    87,    66,    17,
      18,   141,   136,    17,    18,   102,    21,    22,    23,   107,
      99,    13,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   142,    44,   122,   123,    13,    22,    23,   126,   101,
      22,    23,     1,    45,     2,   148,    47,    46,    47,   132,
      70,    71,    72,    73,    74,    75,    76,    77,    52,    48,
      58,    53,     1,    61,     2,    68,    97,     6,     7,     8,
       9,    10,    93,    90,    93,    11,    12,    69,    94,   127,
     129,    54,    57,    67,   102,    70,    71,    72,    73,    74,
      75,    76,    77,   120,   121,    85,    83,    84,    88,    48,
      95,    98,    96,   105,    53,   125,     1,   131,   134,   109,
     110,    65,   124,   133,   130,    97,   144,   137,   138,   145,
     139,   147,    43,   146,   106,    59,   128,    89,    60,    92,
     143,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108
  };

  const short int
  BisonParser::yycheck_[] =
  {
      20,    20,   105,    14,    15,     3,    14,    15,    16,     3,
      21,    22,    18,    21,    27,     0,    29,    17,    18,    23,
      14,    15,    16,    21,    24,    29,    28,    21,   131,    40,
      41,    42,    40,    41,    42,    19,    20,    43,    36,    14,
      15,    43,   145,    14,    15,    65,    40,    41,    42,    68,
      21,     1,    72,    73,    74,    75,    76,    77,    78,    79,
      80,   131,    40,    83,    84,    15,    41,    42,    88,    40,
      41,    42,    25,    40,    27,   145,    27,    40,    27,    99,
       6,     7,     8,     9,    10,    11,    12,    13,    29,    40,
      26,    40,    25,    40,    27,     5,    22,    30,    31,    32,
      33,    34,    23,    23,    23,    38,    39,     4,    29,    29,
      29,    11,    12,    28,   134,     6,     7,     8,     9,    10,
      11,    12,    13,    81,    82,    29,    35,    35,    35,    40,
      29,    36,    22,    21,    40,    18,    25,    21,    23,    42,
      42,    21,    43,    22,    40,    22,    22,    29,    29,    23,
      28,    22,     4,    28,    67,    15,    90,    49,    16,    54,
     134,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,    25,    27,    46,    47,    48,    30,    31,    32,    33,
      34,    38,    39,    47,    49,    54,     3,    14,    15,    16,
      21,    40,    41,    42,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    65,    66,    67,    68,    71,    72,    73,
      74,    75,     0,    46,    40,    40,    40,    27,    40,    50,
      51,    53,    29,    40,    50,    52,    29,    50,    26,    49,
      58,    40,    55,    65,     3,    21,    36,    28,     5,     4,
       6,     7,     8,     9,    10,    11,    12,    13,    17,    18,
      24,    19,    20,    35,    35,    29,    18,    43,    35,    53,
      23,    29,    52,    23,    29,    29,    22,    22,    36,    21,
      22,    40,    65,    69,    70,    21,    48,    55,    56,    42,
      42,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      66,    66,    65,    65,    43,    18,    65,    29,    51,    29,
      40,    21,    65,    22,    23,    64,    71,    29,    29,    28,
      28,    43,    64,    69,    22,    23,    28,    22,    64
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    45,    46,    46,    47,    47,    48,    49,    49,    50,
      50,    50,    51,    51,    52,    52,    53,    53,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    55,
      55,    56,    56,    57,    57,    58,    58,    59,    59,    59,
      59,    60,    60,    60,    60,    60,    60,    61,    61,    62,
      63,    63,    64,    64,    65,    65,    65,    65,    66,    66,
      66,    67,    67,    67,    67,    68,    68,    69,    69,    70,
      71,    71,    72,    72,    73,    73,    74,    75
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     2,     4,     1,     3,     1,     2,     5,
       4,     4,     1,     3,     1,     3,     1,     3,     5,     5,
       3,     4,     3,     3,     2,     4,     3,     2,     1,     1,
       3,     1,     3,     1,     2,     1,     3,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       5,     6,     1,     3,     1,     3,     3,     3,     1,     3,
       3,     1,     1,     1,     3,     3,     4,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1
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
  "tag_decl", "tag_list", "tag_decl_list", "command",
  "boolean_value_expression", "boolean_term", "boolean_factor",
  "boolean_primary", "predicate", "comparison_predicate",
  "like_text_predicate", "exists_predicate", "list_predicate",
  "literal_list", "expression", "term", "factor", "function",
  "function_argument_list", "function_argument", "literal",
  "general_literal", "boolean_literal", "numeric_literal", "attribute", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  BisonParser::yyrline_[] =
  {
       0,   131,   131,   132,   135,   136,   141,   146,   147,   151,
     152,   153,   156,   157,   160,   161,   164,   165,   169,   171,
     173,   175,   176,   178,   179,   181,   182,   184,   186,   197,
     198,   202,   203,   207,   208,   212,   213,   217,   218,   219,
     220,   225,   226,   227,   228,   229,   230,   234,   235,   239,
     243,   244,   248,   249,   253,   254,   255,   256,   260,   261,
     262,   266,   267,   268,   269,   273,   274,   280,   281,   285,
     289,   290,   294,   295,   300,   301,   305,   311
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
#line 1667 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:1155
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



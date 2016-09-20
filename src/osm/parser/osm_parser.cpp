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

#line 37 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "osm_parser.hpp"

// User implementation prologue.

#line 51 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:407
// Unqualified %code blocks.
#line 31 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:408


#include <osm_rule_parser.hpp>

	// Prototype for the yylex function
static OSM::BisonParser::symbol_type yylex(OSM::Filter::Parser &driver, OSM::BisonParser::location_type &loc);

#line 61 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:408


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

#line 6 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:474
namespace OSM {
#line 147 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:474

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
      case 45: // action_block
      case 46: // command_list
      case 47: // command
        value.move< OSM::Filter::Command * > (that.value);
        break;

      case 48: // boolean_value_expression
      case 49: // boolean_term
      case 50: // boolean_factor
      case 51: // boolean_primary
      case 52: // predicate
      case 53: // comparison_predicate
      case 54: // like_text_predicate
      case 55: // exists_predicate
      case 56: // list_predicate
      case 57: // literal_list
      case 58: // expression
      case 59: // term
      case 60: // factor
      case 61: // function
      case 62: // function_argument_list
      case 63: // function_argument
      case 64: // literal
      case 65: // general_literal
      case 66: // boolean_literal
      case 67: // numeric_literal
      case 68: // attribute
        value.move< OSM::Filter::ExpressionNode * > (that.value);
        break;

      case 41: // layer_list
      case 42: // layer
        value.move< OSM::Filter::LayerDefinition * > (that.value);
        break;

      case 43: // rule_list
      case 44: // rule
        value.move< OSM::Filter::Rule * > (that.value);
        break;

      case 37: // "number"
        value.move< double > (that.value);
        break;

      case 36: // "identifier"
      case 38: // "string literal"
        value.move< std::string > (that.value);
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
      case 45: // action_block
      case 46: // command_list
      case 47: // command
        value.copy< OSM::Filter::Command * > (that.value);
        break;

      case 48: // boolean_value_expression
      case 49: // boolean_term
      case 50: // boolean_factor
      case 51: // boolean_primary
      case 52: // predicate
      case 53: // comparison_predicate
      case 54: // like_text_predicate
      case 55: // exists_predicate
      case 56: // list_predicate
      case 57: // literal_list
      case 58: // expression
      case 59: // term
      case 60: // factor
      case 61: // function
      case 62: // function_argument_list
      case 63: // function_argument
      case 64: // literal
      case 65: // general_literal
      case 66: // boolean_literal
      case 67: // numeric_literal
      case 68: // attribute
        value.copy< OSM::Filter::ExpressionNode * > (that.value);
        break;

      case 41: // layer_list
      case 42: // layer
        value.copy< OSM::Filter::LayerDefinition * > (that.value);
        break;

      case 43: // rule_list
      case 44: // rule
        value.copy< OSM::Filter::Rule * > (that.value);
        break;

      case 37: // "number"
        value.copy< double > (that.value);
        break;

      case 36: // "identifier"
      case 38: // "string literal"
        value.copy< std::string > (that.value);
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
      case 45: // action_block
      case 46: // command_list
      case 47: // command
        yylhs.value.build< OSM::Filter::Command * > ();
        break;

      case 48: // boolean_value_expression
      case 49: // boolean_term
      case 50: // boolean_factor
      case 51: // boolean_primary
      case 52: // predicate
      case 53: // comparison_predicate
      case 54: // like_text_predicate
      case 55: // exists_predicate
      case 56: // list_predicate
      case 57: // literal_list
      case 58: // expression
      case 59: // term
      case 60: // factor
      case 61: // function
      case 62: // function_argument_list
      case 63: // function_argument
      case 64: // literal
      case 65: // general_literal
      case 66: // boolean_literal
      case 67: // numeric_literal
      case 68: // attribute
        yylhs.value.build< OSM::Filter::ExpressionNode * > ();
        break;

      case 41: // layer_list
      case 42: // layer
        yylhs.value.build< OSM::Filter::LayerDefinition * > ();
        break;

      case 43: // rule_list
      case 44: // rule
        yylhs.value.build< OSM::Filter::Rule * > ();
        break;

      case 37: // "number"
        yylhs.value.build< double > ();
        break;

      case 36: // "identifier"
      case 38: // "string literal"
        yylhs.value.build< std::string > ();
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
#line 101 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { driver.layers_ = yylhs.value.as< OSM::Filter::LayerDefinition * > () = yystack_[0].value.as< OSM::Filter::LayerDefinition * > () ; }
#line 663 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 3:
#line 102 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { driver.layers_ = yylhs.value.as< OSM::Filter::LayerDefinition * > () = yystack_[1].value.as< OSM::Filter::LayerDefinition * > () ; yylhs.value.as< OSM::Filter::LayerDefinition * > ()->next_ = yystack_[0].value.as< OSM::Filter::LayerDefinition * > () ; }
#line 669 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 105 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::LayerDefinition * > () = new OSM::Filter::LayerDefinition{yystack_[2].value.as< std::string > (), yystack_[1].value.as< std::string > ()} ; yylhs.value.as< OSM::Filter::LayerDefinition * > ()->rules_ = yystack_[0].value.as< OSM::Filter::Rule * > () ; }
#line 675 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 107 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Rule * > () = yystack_[0].value.as< OSM::Filter::Rule * > () ; }
#line 681 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 108 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Rule * > () = yystack_[1].value.as< OSM::Filter::Rule * > () ; yylhs.value.as< OSM::Filter::Rule * > ()->next_ = yystack_[0].value.as< OSM::Filter::Rule * > () ; }
#line 687 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 111 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Rule * > () = new OSM::Filter::Rule{yystack_[1].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::Command * > ()} ; }
#line 693 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 112 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Rule * > () = new OSM::Filter::Rule{nullptr, yystack_[0].value.as< OSM::Filter::Command * > ()} ; }
#line 699 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 117 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = yystack_[1].value.as< OSM::Filter::Command * > () ; }
#line 705 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 121 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = yystack_[0].value.as< OSM::Filter::Command * > () ;  }
#line 711 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 122 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = yystack_[1].value.as< OSM::Filter::Command * > () ; }
#line 717 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 123 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = yystack_[2].value.as< OSM::Filter::Command * > () ; yystack_[2].value.as< OSM::Filter::Command * > ()->next_ = yystack_[0].value.as< OSM::Filter::Command * > () ;}
#line 723 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 127 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = new OSM::Filter::Command( OSM::Filter::Command::Add, yystack_[2].value.as< std::string > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 729 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 128 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = new OSM::Filter::Command( OSM::Filter::Command::Set, yystack_[2].value.as< std::string > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ;}
#line 735 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 129 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = new OSM::Filter::Command( OSM::Filter::Command::Delete, yystack_[0].value.as< std::string > ()) ; }
#line 741 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 130 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = new OSM::Filter::Command( OSM::Filter::Command::Store, yystack_[1].value.as< std::string > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 747 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 131 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::Command * > () = new OSM::Filter::Command( OSM::Filter::Command::Continue) ;}
#line 753 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 140 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 759 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 141 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BooleanOperator( OSM::Filter::BooleanOperator::Or, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 765 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 145 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 771 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 146 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BooleanOperator( OSM::Filter::BooleanOperator::And, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 777 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 150 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 783 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 151 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BooleanOperator( OSM::Filter::BooleanOperator::Not, yystack_[0].value.as< OSM::Filter::ExpressionNode * > (), NULL) ; }
#line 789 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 155 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 795 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 156 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[1].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 801 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 160 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 807 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 161 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 813 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 162 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 819 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 29:
#line 163 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 825 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 30:
#line 168 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::Equal, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ) ; }
#line 831 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 31:
#line 169 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::NotEqual, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ) ; }
#line 837 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 32:
#line 170 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::Less, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ) ; }
#line 843 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 33:
#line 171 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::Greater, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ) ; }
#line 849 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 34:
#line 172 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::LessOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ) ; }
#line 855 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 35:
#line 173 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ComparisonPredicate( OSM::Filter::ComparisonPredicate::GreaterOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ) ; }
#line 861 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 36:
#line 177 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::LikeTextPredicate(yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< std::string > (), true) ; }
#line 867 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 37:
#line 178 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::LikeTextPredicate(yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< std::string > (), false) ; }
#line 873 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 38:
#line 182 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ExistsPredicate(yystack_[0].value.as< std::string > ()) ; }
#line 879 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 39:
#line 186 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ListPredicate(yystack_[4].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNode * > (), true) ; }
#line 885 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 40:
#line 187 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ListPredicate(yystack_[5].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNode * > (), false) ; }
#line 891 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 41:
#line 191 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ExpressionNode() ; yylhs.value.as< OSM::Filter::ExpressionNode * > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 897 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 42:
#line 192 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 903 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 43:
#line 196 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 909 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 44:
#line 197 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BinaryOperator('+',yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 915 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 45:
#line 198 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BinaryOperator('.',yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 921 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 46:
#line 199 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BinaryOperator('-', yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 927 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 47:
#line 203 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 933 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 48:
#line 204 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BinaryOperator('*', yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 939 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 49:
#line 205 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::BinaryOperator('/', yystack_[2].value.as< OSM::Filter::ExpressionNode * > (), yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 945 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 50:
#line 209 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 951 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 51:
#line 210 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 957 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 52:
#line 211 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 963 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 53:
#line 212 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[1].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 969 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 54:
#line 216 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::Function(yystack_[2].value.as< std::string > ()) ; }
#line 975 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 55:
#line 217 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    {
			yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::Function(yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNode * > ()) ;
		 }
#line 983 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 56:
#line 223 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::ExpressionNode() ; yylhs.value.as< OSM::Filter::ExpressionNode * > ()->appendChild(yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 989 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 57:
#line 224 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; yystack_[0].value.as< OSM::Filter::ExpressionNode * > ()->prependChild(yystack_[2].value.as< OSM::Filter::ExpressionNode * > ()) ; }
#line 995 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 58:
#line 228 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 1001 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 59:
#line 232 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 1007 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 60:
#line 233 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 1013 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 61:
#line 237 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::LiteralExpressionNode(yystack_[0].value.as< std::string > ()) ; }
#line 1019 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 62:
#line 238 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = yystack_[0].value.as< OSM::Filter::ExpressionNode * > () ; }
#line 1025 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 63:
#line 243 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::LiteralExpressionNode(true) ; }
#line 1031 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 64:
#line 244 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNode * > () =  new OSM::Filter::LiteralExpressionNode(false) ; }
#line 1037 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 65:
#line 248 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::LiteralExpressionNode((double)yystack_[0].value.as< double > ()) ;
	}
#line 1045 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 66:
#line 254 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNode * > () = new OSM::Filter::Attribute(yystack_[0].value.as< std::string > ()) ;
	}
#line 1053 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
    break;


#line 1057 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:847
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


  const signed char BisonParser::yypact_ninf_ = -93;

  const signed char BisonParser::yytable_ninf_ = -1;

  const signed char
  BisonParser::yypact_[] =
  {
     -18,   -14,    27,   -18,     8,   -93,   -93,    -1,    -5,   -93,
     -93,    19,    42,    76,     0,   -93,   -93,   -93,    -1,   -93,
      34,    72,    64,   -93,   -93,   -93,   -93,   -93,   -93,    90,
      69,   -15,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
      59,    63,    54,    55,    56,    58,   -93,    83,    84,    78,
       4,    89,   -93,   -93,    42,    42,    75,    77,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,   -93,
     -93,    81,    85,   -93,    46,   -93,    76,    95,    46,   -93,
      96,   -93,    97,    98,    -8,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
      46,    46,   -93,   -93,    -8,   100,   -93,    46,   101,   102,
     -93,   -93,   104,   -93,   -93,    -8,   -93,   -93
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     2,     0,     1,     3,     0,     0,    63,
      64,     0,     0,     0,    66,    65,    61,     4,     5,     8,
       0,    18,    20,    22,    24,    26,    27,    28,    29,     0,
      43,    47,    50,    51,    60,    62,    59,    52,    23,    38,
       0,     0,     0,     0,     0,     0,    17,     0,    10,     0,
       0,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
      53,     0,     0,    15,     0,     9,    11,     0,     0,    54,
      66,    58,     0,    56,     0,    19,    21,    36,    37,    30,
      31,    32,    33,    34,    35,    44,    46,    45,    48,    49,
       0,     0,    16,    12,     0,     0,    55,     0,     0,    41,
      13,    14,     0,    57,    39,     0,    40,    42
  };

  const signed char
  BisonParser::yypgoto_[] =
  {
     -93,   117,   -93,   106,   -93,   107,    52,   -93,   -11,    74,
     -93,   122,   -93,   -93,   -93,   -93,   -93,   -92,   -12,    -3,
     -93,   -93,    24,   -93,   -76,   -93,   -93,   -93,   -93
  };

  const signed char
  BisonParser::yydefgoto_[] =
  {
      -1,     2,     3,    17,    18,    19,    47,    48,    20,    21,
      22,    23,    24,    25,    26,    27,    28,   108,    29,    30,
      31,    32,    82,    83,    33,    34,    35,    36,    37
  };

  const unsigned char
  BisonParser::yytable_[] =
  {
      41,    40,     8,    49,    67,    68,     9,    10,   109,     9,
      10,    11,   112,     9,    10,    11,    12,     1,     9,    10,
      12,    50,     4,   117,    13,    78,    79,     5,   109,    15,
      16,    14,    15,    16,    51,    14,    15,    16,    81,   109,
      80,    15,    16,    85,     7,     8,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    39,     9,    10,    11,    13,
       9,    10,   102,    12,    98,    99,   105,    78,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    54,    14,    15,
      16,    69,    80,    15,    16,    70,    64,    65,   110,   111,
      71,    72,    73,    66,    74,    81,    56,    57,    58,    59,
      60,    61,    62,    63,    42,    43,    44,    45,    46,    75,
      84,    76,    77,    87,   100,    88,   104,    50,   101,   106,
       6,   107,    70,   114,    52,   115,   116,    53,   103,    86,
      38,   113
  };

  const unsigned char
  BisonParser::yycheck_[] =
  {
      12,    12,     3,     3,    19,    20,    14,    15,    84,    14,
      15,    16,   104,    14,    15,    16,    21,    35,    14,    15,
      21,    21,    36,   115,    25,    21,    22,     0,   104,    37,
      38,    36,    37,    38,    34,    36,    37,    38,    50,   115,
      36,    37,    38,    54,    36,     3,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    36,    14,    15,    16,    25,
      14,    15,    74,    21,    67,    68,    78,    21,     4,     6,
       7,     8,     9,    10,    11,    12,    13,     5,    36,    37,
      38,    22,    36,    37,    38,    22,    17,    18,   100,   101,
      36,    36,    36,    24,    36,   107,     6,     7,     8,     9,
      10,    11,    12,    13,    28,    29,    30,    31,    32,    26,
      21,    27,    34,    38,    33,    38,    21,    21,    33,    22,
       3,    23,    22,    22,    18,    23,    22,    20,    76,    55,
       8,   107
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,    35,    41,    42,    36,     0,    41,    36,     3,    14,
      15,    16,    21,    25,    36,    37,    38,    43,    44,    45,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    58,
      59,    60,    61,    64,    65,    66,    67,    68,    51,    36,
      48,    58,    28,    29,    30,    31,    32,    46,    47,     3,
      21,    34,    43,    45,     5,     4,     6,     7,     8,     9,
      10,    11,    12,    13,    17,    18,    24,    19,    20,    22,
      22,    36,    36,    36,    36,    26,    27,    34,    21,    22,
      36,    58,    62,    63,    21,    48,    49,    38,    38,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    59,    59,
      33,    33,    58,    46,    21,    58,    22,    23,    57,    64,
      58,    58,    57,    62,    22,    23,    22,    57
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    40,    41,    41,    42,    43,    43,    44,    44,    45,
      46,    46,    46,    47,    47,    47,    47,    47,    48,    48,
      49,    49,    50,    50,    51,    51,    52,    52,    52,    52,
      53,    53,    53,    53,    53,    53,    54,    54,    55,    56,
      56,    57,    57,    58,    58,    58,    58,    59,    59,    59,
      60,    60,    60,    60,    61,    61,    62,    62,    63,    64,
      64,    65,    65,    66,    66,    67,    68
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     2,     4,     1,     2,     2,     1,     3,
       1,     2,     3,     4,     4,     2,     3,     1,     1,     3,
       1,     3,     1,     2,     1,     3,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     5,
       6,     1,     3,     1,     3,     3,     3,     1,     3,     3,
       1,     1,     1,     3,     3,     4,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const BisonParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"!\"", "\"&&\"", "\"||\"",
  "\"~\"", "\"!~\"", "\"==\"", "\"!=\"", "\"<\"", "\">\"", "\"<=\"",
  "\">=\"", "\"true\"", "\"false\"", "\"^\"", "\"+\"", "\"-\"", "\"*\"",
  "\"/\"", "\"(\"", "\")\"", "\",\"", "\".\"", "\"{\"", "\"}\"", "\";\"",
  "\"add tag\"", "\"set tag\"", "\"delete tag\"", "\"store\"",
  "\"continue\"", "\"=\"", "\"in\"", "\"@layer\"", "\"identifier\"",
  "\"number\"", "\"string literal\"", "UMINUS", "$accept", "layer_list",
  "layer", "rule_list", "rule", "action_block", "command_list", "command",
  "boolean_value_expression", "boolean_term", "boolean_factor",
  "boolean_primary", "predicate", "comparison_predicate",
  "like_text_predicate", "exists_predicate", "list_predicate",
  "literal_list", "expression", "term", "factor", "function",
  "function_argument_list", "function_argument", "literal",
  "general_literal", "boolean_literal", "numeric_literal", "attribute", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  BisonParser::yyrline_[] =
  {
       0,   101,   101,   102,   105,   107,   108,   111,   112,   117,
     121,   122,   123,   127,   128,   129,   130,   131,   140,   141,
     145,   146,   150,   151,   155,   156,   160,   161,   162,   163,
     168,   169,   170,   171,   172,   173,   177,   178,   182,   186,
     187,   191,   192,   196,   197,   198,   199,   203,   204,   205,
     209,   210,   211,   212,   216,   217,   223,   224,   228,   232,
     233,   237,   238,   243,   244,   248,   254
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


#line 6 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:1155
} // OSM
#line 1513 "/home/malasiot/source/mftools/src/osm/parser/osm_parser.cpp" // lalr1.cc:1155
#line 261 "/home/malasiot/source/mftools/src/osm/osm.y" // lalr1.cc:1156

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

static int yydebug_=1 ;

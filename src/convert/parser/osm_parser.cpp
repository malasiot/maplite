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
#line 27 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:408

#include "osm_rule_parser.hpp"

static OSM::BisonParser::symbol_type yylex(OSM::Filter::Parser &driver, OSM::BisonParser::location_type &loc);

#line 59 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:408


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
#line 145 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:474

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
      case 51: // action_block
      case 52: // command_list
        value.move< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 57: // command
        value.move< OSM::Filter::CommandPtr > (that.value);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.move< OSM::Filter::ExpressionListPtr > (that.value);
        break;

      case 58: // boolean_value_expression
      case 59: // boolean_term
      case 60: // boolean_factor
      case 61: // boolean_primary
      case 62: // predicate
      case 63: // unary_predicate
      case 64: // comparison_predicate
      case 65: // like_text_predicate
      case 66: // list_predicate
      case 68: // expression
      case 69: // term
      case 70: // factor
      case 71: // function
      case 73: // function_argument
      case 74: // literal
      case 75: // general_literal
      case 76: // boolean_literal
      case 77: // numeric_literal
      case 78: // attribute
        value.move< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 49: // rule_list
        value.move< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 50: // rule
        value.move< OSM::Filter::RulePtr > (that.value);
        break;

      case 56: // tag_decl_list
        value.move< OSM::Filter::TagDeclarationListPtr > (that.value);
        break;

      case 54: // tag_decl
        value.move< OSM::Filter::TagDeclarationPtr > (that.value);
        break;

      case 55: // tag_list
        value.move< OSM::Filter::TagListPtr > (that.value);
        break;

      case 53: // zoom_range
        value.move< OSM::Filter::ZoomRangePtr > (that.value);
        break;

      case 42: // "number"
        value.move< double > (that.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.move< std::string > (that.value);
        break;

      case 45: // "zoom specifier"
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
      case 51: // action_block
      case 52: // command_list
        value.copy< OSM::Filter::CommandListPtr > (that.value);
        break;

      case 57: // command
        value.copy< OSM::Filter::CommandPtr > (that.value);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.copy< OSM::Filter::ExpressionListPtr > (that.value);
        break;

      case 58: // boolean_value_expression
      case 59: // boolean_term
      case 60: // boolean_factor
      case 61: // boolean_primary
      case 62: // predicate
      case 63: // unary_predicate
      case 64: // comparison_predicate
      case 65: // like_text_predicate
      case 66: // list_predicate
      case 68: // expression
      case 69: // term
      case 70: // factor
      case 71: // function
      case 73: // function_argument
      case 74: // literal
      case 75: // general_literal
      case 76: // boolean_literal
      case 77: // numeric_literal
      case 78: // attribute
        value.copy< OSM::Filter::ExpressionNodePtr > (that.value);
        break;

      case 49: // rule_list
        value.copy< OSM::Filter::RuleListPtr > (that.value);
        break;

      case 50: // rule
        value.copy< OSM::Filter::RulePtr > (that.value);
        break;

      case 56: // tag_decl_list
        value.copy< OSM::Filter::TagDeclarationListPtr > (that.value);
        break;

      case 54: // tag_decl
        value.copy< OSM::Filter::TagDeclarationPtr > (that.value);
        break;

      case 55: // tag_list
        value.copy< OSM::Filter::TagListPtr > (that.value);
        break;

      case 53: // zoom_range
        value.copy< OSM::Filter::ZoomRangePtr > (that.value);
        break;

      case 42: // "number"
        value.copy< double > (that.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.copy< std::string > (that.value);
        break;

      case 45: // "zoom specifier"
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
      case 51: // action_block
      case 52: // command_list
        yylhs.value.build< OSM::Filter::CommandListPtr > ();
        break;

      case 57: // command
        yylhs.value.build< OSM::Filter::CommandPtr > ();
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        yylhs.value.build< OSM::Filter::ExpressionListPtr > ();
        break;

      case 58: // boolean_value_expression
      case 59: // boolean_term
      case 60: // boolean_factor
      case 61: // boolean_primary
      case 62: // predicate
      case 63: // unary_predicate
      case 64: // comparison_predicate
      case 65: // like_text_predicate
      case 66: // list_predicate
      case 68: // expression
      case 69: // term
      case 70: // factor
      case 71: // function
      case 73: // function_argument
      case 74: // literal
      case 75: // general_literal
      case 76: // boolean_literal
      case 77: // numeric_literal
      case 78: // attribute
        yylhs.value.build< OSM::Filter::ExpressionNodePtr > ();
        break;

      case 49: // rule_list
        yylhs.value.build< OSM::Filter::RuleListPtr > ();
        break;

      case 50: // rule
        yylhs.value.build< OSM::Filter::RulePtr > ();
        break;

      case 56: // tag_decl_list
        yylhs.value.build< OSM::Filter::TagDeclarationListPtr > ();
        break;

      case 54: // tag_decl
        yylhs.value.build< OSM::Filter::TagDeclarationPtr > ();
        break;

      case 55: // tag_list
        yylhs.value.build< OSM::Filter::TagListPtr > ();
        break;

      case 53: // zoom_range
        yylhs.value.build< OSM::Filter::ZoomRangePtr > ();
        break;

      case 42: // "number"
        yylhs.value.build< double > ();
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        yylhs.value.build< std::string > ();
        break;

      case 45: // "zoom specifier"
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
  case 3:
#line 109 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.script_ = yystack_[1].value.as< std::string > () ; }
#line 736 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 111 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_back(yystack_[0].value.as< OSM::Filter::RulePtr > ()) ; }
#line 742 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 112 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { driver.rules_.push_front(yystack_[1].value.as< OSM::Filter::RulePtr > ()) ; }
#line 748 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 115 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::CommandListPtr > ()->commands_) ; }
#line 754 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 116 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::RulePtr > () = std::make_shared<OSM::Filter::Rule>(nullptr, yystack_[0].value.as< OSM::Filter::CommandListPtr > ()->commands_) ; }
#line 760 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 121 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[1].value.as< OSM::Filter::CommandListPtr > () ; }
#line 766 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 126 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = std::make_shared<OSM::Filter::CommandList>() ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_back(yystack_[0].value.as< OSM::Filter::CommandPtr > ()) ;  }
#line 772 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 127 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandListPtr > () = yystack_[0].value.as< OSM::Filter::CommandListPtr > () ; yylhs.value.as< OSM::Filter::CommandListPtr > ()->commands_.push_front(yystack_[1].value.as< OSM::Filter::CommandPtr > ()) ; }
#line 778 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 131 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[1].value.as< uint8_t > (), 255) ; }
#line 784 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 132 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[3].value.as< uint8_t > (), yystack_[1].value.as< uint8_t > ()); }
#line 790 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 133 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(yystack_[2].value.as< uint8_t > (), 255); }
#line 796 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 134 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ZoomRangePtr > () = std::make_shared<OSM::Filter::ZoomRange>(0, yystack_[1].value.as< uint8_t > ()); }
#line 802 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 137 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationPtr > () = std::make_shared<OSM::Filter::TagDeclaration>(yystack_[0].value.as< std::string > (), nullptr); }
#line 808 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 138 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationPtr > () = std::make_shared<OSM::Filter::TagDeclaration>(yystack_[2].value.as< std::string > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()); }
#line 814 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 141 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = std::make_shared<OSM::Filter::TagList>() ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 820 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 142 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagListPtr > () = yystack_[2].value.as< OSM::Filter::TagListPtr > () ; yylhs.value.as< OSM::Filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 826 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 145 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationListPtr > () = std::make_shared<OSM::Filter::TagDeclarationList>() ; yylhs.value.as< OSM::Filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< OSM::Filter::TagDeclarationPtr > ()) ; }
#line 832 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 146 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::TagDeclarationListPtr > () = yystack_[2].value.as< OSM::Filter::TagDeclarationListPtr > () ; yylhs.value.as< OSM::Filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< OSM::Filter::TagDeclarationPtr > ()) ; }
#line 838 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 150 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Add, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 844 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 152 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Set, yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ;}
#line 850 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 154 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>(OSM::Filter::Command::Delete, yystack_[1].value.as< std::string > ()) ; }
#line 856 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 156 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagDeclarationListPtr > ()) ; }
#line 862 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 157 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::AttachCommand>(*yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 868 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 158 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagDeclarationListPtr > ()) ; }
#line 874 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 160 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>(*yystack_[1].value.as< OSM::Filter::ZoomRangePtr > ()) ; }
#line 880 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 161 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::WriteAllCommand>( OSM::Filter::ZoomRange(0, 255)) ; }
#line 886 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 29:
#line 163 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>(*yystack_[2].value.as< OSM::Filter::ZoomRangePtr > (), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 892 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 30:
#line 164 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::ExcludeCommand>( OSM::Filter::ZoomRange(0, 255), *yystack_[1].value.as< OSM::Filter::TagListPtr > ()) ; }
#line 898 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 31:
#line 166 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::SimpleCommand>( OSM::Filter::SimpleCommand::Continue) ;}
#line 904 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 32:
#line 168 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::RuleCommand>( yystack_[0].value.as< OSM::Filter::RulePtr > ()) ;}
#line 910 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 33:
#line 169 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::CommandPtr > () = std::make_shared<OSM::Filter::FunctionCommand>(yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 916 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 34:
#line 174 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 922 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 35:
#line 175 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Or, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 928 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 36:
#line 179 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 934 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 37:
#line 180 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::And, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 940 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 38:
#line 184 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 946 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 39:
#line 185 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BooleanOperator>( OSM::Filter::BooleanOperator::Not, yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > (), nullptr) ; }
#line 952 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 40:
#line 189 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 958 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 41:
#line 190 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 964 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 42:
#line 194 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 970 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 43:
#line 195 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 976 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 44:
#line 196 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 982 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 45:
#line 197 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 988 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 46:
#line 201 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::UnaryPredicate>( yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ;}
#line 994 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 47:
#line 204 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Equal, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1000 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 48:
#line 205 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::NotEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1006 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 49:
#line 206 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Less, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1012 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 50:
#line 207 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::Greater, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1018 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 51:
#line 208 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::LessOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1024 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 52:
#line 209 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ComparisonPredicate>( OSM::Filter::ComparisonPredicate::GreaterOrEqual, yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ) ; }
#line 1030 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 53:
#line 213 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), true) ; }
#line 1036 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 54:
#line 214 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LikeTextPredicate>(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), false) ; }
#line 1042 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 55:
#line 218 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[4].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionListPtr > ()->children(), true) ; }
#line 1048 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 56:
#line 219 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::ListPredicate>(yystack_[5].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionListPtr > ()->children(), false) ; }
#line 1054 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 57:
#line 223 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionListPtr > () = std::make_shared<OSM::Filter::ExpressionList>() ;  yylhs.value.as< OSM::Filter::ExpressionListPtr > ()->append(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ;  }
#line 1060 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 58:
#line 224 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionListPtr > () = yystack_[0].value.as< OSM::Filter::ExpressionListPtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionListPtr > ()->prepend(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1066 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 59:
#line 228 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1072 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 60:
#line 229 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('+',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1078 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 61:
#line 230 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('.',yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1084 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 62:
#line 231 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('-', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1090 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 63:
#line 235 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1096 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 64:
#line 236 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('*', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1102 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 65:
#line 237 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::BinaryOperator>('/', yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > (), yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1108 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 66:
#line 241 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1114 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 67:
#line 242 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1120 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 68:
#line 243 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1126 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 69:
#line 244 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[1].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1132 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 70:
#line 248 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[2].value.as< std::string > (), &driver.lua_) ; }
#line 1138 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 71:
#line 249 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
			yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Function>(yystack_[3].value.as< std::string > (), yystack_[1].value.as< OSM::Filter::ExpressionListPtr > ()->children(), &driver.lua_) ;
		 }
#line 1146 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 72:
#line 255 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
				yylhs.value.as< OSM::Filter::ExpressionListPtr > () = std::make_shared<OSM::Filter::ExpressionList>() ;
				yylhs.value.as< OSM::Filter::ExpressionListPtr > ()->append(yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > ()) ;
			}
#line 1155 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 73:
#line 259 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionListPtr > () = yystack_[0].value.as< OSM::Filter::ExpressionListPtr > () ; yystack_[0].value.as< OSM::Filter::ExpressionListPtr > ()->prepend(yystack_[2].value.as< OSM::Filter::ExpressionNodePtr > ()) ; }
#line 1161 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 74:
#line 263 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1167 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 75:
#line 267 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1173 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 76:
#line 268 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1179 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 77:
#line 272 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(yystack_[0].value.as< std::string > ()) ; }
#line 1185 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 78:
#line 273 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = yystack_[0].value.as< OSM::Filter::ExpressionNodePtr > () ; }
#line 1191 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 79:
#line 278 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>(true) ; }
#line 1197 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 80:
#line 279 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    { yylhs.value.as< OSM::Filter::ExpressionNodePtr > () =  std::make_shared<OSM::Filter::LiteralExpressionNode>(false) ; }
#line 1203 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 81:
#line 283 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::LiteralExpressionNode>((double)yystack_[0].value.as< double > ()) ;
	}
#line 1211 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;

  case 82:
#line 289 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:847
    {
		yylhs.value.as< OSM::Filter::ExpressionNodePtr > () = std::make_shared<OSM::Filter::Attribute>(yystack_[0].value.as< std::string > ()) ;
	}
#line 1219 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
    break;


#line 1223 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:847
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


  const short int BisonParser::yypact_ninf_ = -138;

  const signed char BisonParser::yytable_ninf_ = -1;

  const short int
  BisonParser::yypact_[] =
  {
      42,    64,     5,    94,     9,  -138,    94,  -138,   -23,   -13,
       3,    37,    87,    46,   104,    31,    71,  -138,   106,    64,
     109,    -4,  -138,  -138,     5,     0,  -138,  -138,    -1,   135,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,   117,    58,    81,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,   105,
     107,   114,    -3,   110,   103,  -138,    51,  -138,  -138,    31,
      54,  -138,   118,    61,    -8,  -138,  -138,  -138,  -138,    63,
     100,   113,   120,     5,   123,     5,   108,   111,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,  -138,   101,    13,     8,    70,   103,  -138,    91,   115,
    -138,  -138,  -138,     8,  -138,    71,  -138,   128,   129,  -138,
    -138,   132,    10,   135,  -138,  -138,  -138,  -138,  -138,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,   126,
     130,   133,    43,  -138,  -138,  -138,  -138,  -138,  -138,   136,
    -138,     8,    10,   138,   134,  -138,  -138,  -138,  -138,   137,
    -138,   140,  -138,    10,  -138,  -138,  -138
  };

  const unsigned char
  BisonParser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     2,     4,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,     0,     9,
       0,     0,    79,    80,     0,    82,    81,    77,     0,    34,
      36,    38,    40,    42,    43,    44,    45,    46,    59,    63,
      66,    67,    76,    78,    75,    68,     3,     1,     5,     0,
       0,     0,     0,    15,     0,    19,     0,    31,    17,     0,
       0,    28,     0,     0,     0,     8,    10,    33,    39,     0,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,     0,     0,    26,     0,     0,
      30,    27,    25,     0,    70,    82,    74,     0,    72,    41,
      69,     0,     0,    35,     6,    37,    53,    54,    47,    48,
      49,    50,    51,    52,    60,    62,    61,    64,    65,     0,
       0,     0,     0,    11,    16,    24,    20,    29,    18,     0,
      71,     0,     0,     0,    57,    21,    22,    14,    13,     0,
      73,     0,    55,     0,    12,    56,    58
  };

  const short int
  BisonParser::yypgoto_[] =
  {
    -138,  -138,   112,    11,    89,   145,   121,    72,   -14,   116,
    -138,   142,    96,    92,   150,  -138,  -138,  -138,  -138,  -138,
    -137,   -24,    49,  -138,    62,    32,  -138,  -110,  -138,  -138,
    -138,  -138
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,     4,     5,     6,     7,    18,    54,    55,    60,    56,
      19,    28,    29,    30,    31,    32,    33,    34,    35,    36,
     143,    37,    38,    39,    40,   107,   108,    41,    42,    43,
      44,    45
  };

  const unsigned char
  BisonParser::yytable_[] =
  {
      70,    63,   144,    71,    73,   151,    22,    23,    21,    47,
      22,    23,    17,   103,   104,    92,   156,    24,    49,    22,
      23,    64,    22,    23,    22,    23,    24,    74,    50,   103,
      17,   132,   144,   105,    26,    27,    72,    25,    26,    27,
     106,   133,    93,   144,    51,    98,    25,    26,    27,   105,
      26,    27,    26,    27,   118,   119,   120,   121,   122,   123,
     124,   125,   126,    20,    52,   129,   130,     1,    73,     2,
     134,   148,    58,    52,    96,    84,    85,    99,    53,   139,
      97,    20,    86,   100,    99,   109,     3,    58,   149,     1,
     102,     2,    64,    96,     8,     9,    10,    11,    12,   135,
      87,    88,    13,    14,    15,    16,    76,    77,    78,    79,
      80,    81,    82,    83,    99,    46,    57,   106,    48,     1,
     137,     2,   110,    76,    77,    78,    79,    80,    81,    82,
      83,    52,    65,    61,    59,    62,   127,   128,    67,    75,
      89,   112,    90,    91,    53,    94,   131,   101,     1,   111,
     140,   116,   141,   142,   117,   145,   138,   153,   110,   146,
     152,   147,   155,   114,    66,   154,    69,   115,   136,   113,
      95,    68,     0,   150
  };

  const short int
  BisonParser::yycheck_[] =
  {
      24,    15,   112,     3,     5,   142,    14,    15,     3,     0,
      14,    15,     1,    21,    22,    18,   153,    21,    41,    14,
      15,    21,    14,    15,    14,    15,    21,    28,    41,    21,
      19,    18,   142,    41,    42,    43,    36,    41,    42,    43,
      64,    28,    45,   153,    41,    59,    41,    42,    43,    41,
      42,    43,    42,    43,    78,    79,    80,    81,    82,    83,
      84,    85,    86,     1,    27,    89,    90,    25,     5,    27,
      94,    28,    41,    27,    23,    17,    18,    23,    41,   103,
      29,    19,    24,    29,    23,    22,    44,    41,    45,    25,
      29,    27,    21,    23,    30,    31,    32,    33,    34,    29,
      19,    20,    38,    39,    40,    41,     6,     7,     8,     9,
      10,    11,    12,    13,    23,     3,    29,   141,     6,    25,
      29,    27,    22,     6,     7,     8,     9,    10,    11,    12,
      13,    27,    26,    29,    13,    14,    87,    88,    29,     4,
      35,    21,    35,    29,    41,    35,    45,    29,    25,    36,
      22,    43,    23,    21,    43,    29,    41,    23,    22,    29,
      22,    28,    22,    74,    19,    28,    24,    75,    96,    73,
      54,    21,    -1,   141
  };

  const unsigned char
  BisonParser::yystos_[] =
  {
       0,    25,    27,    44,    48,    49,    50,    51,    30,    31,
      32,    33,    34,    38,    39,    40,    41,    50,    52,    57,
      71,     3,    14,    15,    21,    41,    42,    43,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    68,    69,    70,
      71,    74,    75,    76,    77,    78,    49,     0,    49,    41,
      41,    41,    27,    41,    53,    54,    56,    29,    41,    53,
      55,    29,    53,    55,    21,    26,    52,    29,    61,    58,
      68,     3,    36,     5,    28,     4,     6,     7,     8,     9,
      10,    11,    12,    13,    17,    18,    24,    19,    20,    35,
      35,    29,    18,    45,    35,    56,    23,    29,    55,    23,
      29,    29,    29,    21,    22,    41,    68,    72,    73,    22,
      22,    36,    21,    59,    51,    60,    43,    43,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    69,    69,    68,
      68,    45,    18,    28,    68,    29,    54,    29,    41,    68,
      22,    23,    21,    67,    74,    29,    29,    28,    28,    45,
      72,    67,    22,    23,    28,    22,    67
  };

  const unsigned char
  BisonParser::yyr1_[] =
  {
       0,    47,    48,    48,    49,    49,    50,    50,    51,    52,
      52,    53,    53,    53,    53,    54,    54,    55,    55,    56,
      56,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    58,    58,    59,    59,    60,    60,
      61,    61,    62,    62,    62,    62,    63,    64,    64,    64,
      64,    64,    64,    65,    65,    66,    66,    67,    67,    68,
      68,    68,    68,    69,    69,    69,    70,    70,    70,    70,
      71,    71,    72,    72,    73,    74,    74,    75,    75,    76,
      76,    77,    78
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     2,     1,     2,     4,     1,     3,     1,
       2,     3,     5,     4,     4,     1,     3,     1,     3,     1,
       3,     5,     5,     3,     4,     3,     3,     3,     2,     4,
       3,     2,     1,     2,     1,     3,     1,     3,     1,     2,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     5,     6,     1,     3,     1,
       3,     3,     3,     1,     3,     3,     1,     1,     1,     3,
       3,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1
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
  "\"number\"", "\"string literal\"", "\"LUA script\"",
  "\"zoom specifier\"", "UMINUS", "$accept", "filter", "rule_list", "rule",
  "action_block", "command_list", "zoom_range", "tag_decl", "tag_list",
  "tag_decl_list", "command", "boolean_value_expression", "boolean_term",
  "boolean_factor", "boolean_primary", "predicate", "unary_predicate",
  "comparison_predicate", "like_text_predicate", "list_predicate",
  "literal_list", "expression", "term", "factor", "function",
  "function_argument_list", "function_argument", "literal",
  "general_literal", "boolean_literal", "numeric_literal", "attribute", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  BisonParser::yyrline_[] =
  {
       0,   108,   108,   109,   111,   112,   115,   116,   121,   126,
     127,   131,   132,   133,   134,   137,   138,   141,   142,   145,
     146,   150,   152,   154,   156,   157,   158,   160,   161,   163,
     164,   166,   168,   169,   174,   175,   179,   180,   184,   185,
     189,   190,   194,   195,   196,   197,   201,   204,   205,   206,
     207,   208,   209,   213,   214,   218,   219,   223,   224,   228,
     229,   230,   231,   235,   236,   237,   241,   242,   243,   244,
     248,   249,   255,   259,   263,   267,   268,   272,   273,   278,
     279,   283,   289
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
#line 1709 "/home/malasiot/source/mftools/src/convert/parser/osm_parser.cpp" // lalr1.cc:1155
#line 294 "/home/malasiot/source/mftools/src/convert/osm.y" // lalr1.cc:1156

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



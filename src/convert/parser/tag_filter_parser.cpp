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

#line 37 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:399

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "tag_filter_parser.hpp"

// User implementation prologue.

#line 51 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:407
// Unqualified %code blocks.
#line 24 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:408

#include "tag_filter_config_parser.hpp"
static tag_filter::Parser::symbol_type yylex(TagFilterConfigParser &driver, tag_filter::Parser::location_type &loc);

#line 58 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:408


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

#line 9 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:474
namespace tag_filter {
#line 144 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:474

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
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
  Parser::Parser (TagFilterConfigParser &driver_yyarg, tag_filter::Parser::location_type &loc_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg),
      loc (loc_yyarg)
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 42: // "number"
        value.move< double > (that.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.move< std::string > (that.value);
        break;

      case 51: // action_block
      case 52: // command_list
        value.move< tag_filter::CommandListPtr > (that.value);
        break;

      case 57: // command
        value.move< tag_filter::CommandPtr > (that.value);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.move< tag_filter::ExpressionListPtr > (that.value);
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
        value.move< tag_filter::ExpressionNodePtr > (that.value);
        break;

      case 49: // rule_list
        value.move< tag_filter::RuleListPtr > (that.value);
        break;

      case 50: // rule
        value.move< tag_filter::RulePtr > (that.value);
        break;

      case 56: // tag_decl_list
        value.move< tag_filter::TagDeclarationListPtr > (that.value);
        break;

      case 54: // tag_decl
        value.move< tag_filter::TagDeclarationPtr > (that.value);
        break;

      case 55: // tag_list
        value.move< tag_filter::TagListPtr > (that.value);
        break;

      case 53: // zoom_range
        value.move< tag_filter::ZoomRangePtr > (that.value);
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
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 42: // "number"
        value.copy< double > (that.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.copy< std::string > (that.value);
        break;

      case 51: // action_block
      case 52: // command_list
        value.copy< tag_filter::CommandListPtr > (that.value);
        break;

      case 57: // command
        value.copy< tag_filter::CommandPtr > (that.value);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.copy< tag_filter::ExpressionListPtr > (that.value);
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
        value.copy< tag_filter::ExpressionNodePtr > (that.value);
        break;

      case 49: // rule_list
        value.copy< tag_filter::RuleListPtr > (that.value);
        break;

      case 50: // rule
        value.copy< tag_filter::RulePtr > (that.value);
        break;

      case 56: // tag_decl_list
        value.copy< tag_filter::TagDeclarationListPtr > (that.value);
        break;

      case 54: // tag_decl
        value.copy< tag_filter::TagDeclarationPtr > (that.value);
        break;

      case 55: // tag_list
        value.copy< tag_filter::TagListPtr > (that.value);
        break;

      case 53: // zoom_range
        value.copy< tag_filter::ZoomRangePtr > (that.value);
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
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
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
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
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
      case 42: // "number"
        yylhs.value.build< double > ();
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        yylhs.value.build< std::string > ();
        break;

      case 51: // action_block
      case 52: // command_list
        yylhs.value.build< tag_filter::CommandListPtr > ();
        break;

      case 57: // command
        yylhs.value.build< tag_filter::CommandPtr > ();
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        yylhs.value.build< tag_filter::ExpressionListPtr > ();
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
        yylhs.value.build< tag_filter::ExpressionNodePtr > ();
        break;

      case 49: // rule_list
        yylhs.value.build< tag_filter::RuleListPtr > ();
        break;

      case 50: // rule
        yylhs.value.build< tag_filter::RulePtr > ();
        break;

      case 56: // tag_decl_list
        yylhs.value.build< tag_filter::TagDeclarationListPtr > ();
        break;

      case 54: // tag_decl
        yylhs.value.build< tag_filter::TagDeclarationPtr > ();
        break;

      case 55: // tag_list
        yylhs.value.build< tag_filter::TagListPtr > ();
        break;

      case 53: // zoom_range
        yylhs.value.build< tag_filter::ZoomRangePtr > ();
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
#line 105 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { driver.script_ = yystack_[1].value.as< std::string > () ; }
#line 735 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 4:
#line 107 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { driver.rules_.push_back(yystack_[0].value.as< tag_filter::RulePtr > ()) ; }
#line 741 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 5:
#line 108 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { driver.rules_.push_front(yystack_[1].value.as< tag_filter::RulePtr > ()) ; }
#line 747 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 6:
#line 111 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::RulePtr > () = std::make_shared<tag_filter::Rule>(yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::CommandListPtr > ()->commands_) ; }
#line 753 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 7:
#line 112 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::RulePtr > () = std::make_shared<tag_filter::Rule>(nullptr, yystack_[0].value.as< tag_filter::CommandListPtr > ()->commands_) ; }
#line 759 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 8:
#line 117 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandListPtr > () = yystack_[1].value.as< tag_filter::CommandListPtr > () ; }
#line 765 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 9:
#line 122 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandListPtr > () = std::make_shared<tag_filter::CommandList>() ; yylhs.value.as< tag_filter::CommandListPtr > ()->commands_.push_back(yystack_[0].value.as< tag_filter::CommandPtr > ()) ;  }
#line 771 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 10:
#line 123 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandListPtr > () = yystack_[0].value.as< tag_filter::CommandListPtr > () ; yylhs.value.as< tag_filter::CommandListPtr > ()->commands_.push_front(yystack_[1].value.as< tag_filter::CommandPtr > ()) ; }
#line 777 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 11:
#line 127 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ZoomRangePtr > () = std::make_shared<tag_filter::ZoomRange>(yystack_[1].value.as< uint8_t > (), 255) ; }
#line 783 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 12:
#line 128 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ZoomRangePtr > () = std::make_shared<tag_filter::ZoomRange>(yystack_[3].value.as< uint8_t > (), yystack_[1].value.as< uint8_t > ()); }
#line 789 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 13:
#line 129 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ZoomRangePtr > () = std::make_shared<tag_filter::ZoomRange>(yystack_[2].value.as< uint8_t > (), 255); }
#line 795 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 14:
#line 130 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ZoomRangePtr > () = std::make_shared<tag_filter::ZoomRange>(0, yystack_[1].value.as< uint8_t > ()); }
#line 801 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 15:
#line 133 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::TagDeclarationPtr > () = std::make_shared<tag_filter::TagDeclaration>(yystack_[0].value.as< std::string > (), nullptr); }
#line 807 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 16:
#line 134 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::TagDeclarationPtr > () = std::make_shared<tag_filter::TagDeclaration>(yystack_[2].value.as< std::string > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()); }
#line 813 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 17:
#line 137 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::TagListPtr > () = std::make_shared<tag_filter::TagList>() ; yylhs.value.as< tag_filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 819 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 18:
#line 138 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::TagListPtr > () = yystack_[2].value.as< tag_filter::TagListPtr > () ; yylhs.value.as< tag_filter::TagListPtr > ()->tags_.push_back(yystack_[0].value.as< std::string > ()) ; }
#line 825 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 19:
#line 141 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::TagDeclarationListPtr > () = std::make_shared<tag_filter::TagDeclarationList>() ; yylhs.value.as< tag_filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< tag_filter::TagDeclarationPtr > ()) ; }
#line 831 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 20:
#line 142 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::TagDeclarationListPtr > () = yystack_[2].value.as< tag_filter::TagDeclarationListPtr > () ; yylhs.value.as< tag_filter::TagDeclarationListPtr > ()->tags_.push_back(yystack_[0].value.as< tag_filter::TagDeclarationPtr > ()) ; }
#line 837 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 21:
#line 146 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::SimpleCommand>(tag_filter::Command::Add, yystack_[3].value.as< std::string > (), yystack_[1].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 843 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 22:
#line 148 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::SimpleCommand>(tag_filter::Command::Set, yystack_[3].value.as< std::string > (), yystack_[1].value.as< tag_filter::ExpressionNodePtr > ()) ;}
#line 849 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 23:
#line 150 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::SimpleCommand>(tag_filter::Command::Delete, yystack_[1].value.as< std::string > ()) ; }
#line 855 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 24:
#line 152 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::WriteCommand>(*yystack_[2].value.as< tag_filter::ZoomRangePtr > (), *yystack_[1].value.as< tag_filter::TagDeclarationListPtr > ()) ; }
#line 861 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 25:
#line 153 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::AttachCommand>(*yystack_[1].value.as< tag_filter::TagListPtr > ()) ; }
#line 867 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 26:
#line 154 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::WriteCommand>( tag_filter::ZoomRange(0, 255), *yystack_[1].value.as< tag_filter::TagDeclarationListPtr > ()) ; }
#line 873 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 27:
#line 156 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::WriteAllCommand>(*yystack_[1].value.as< tag_filter::ZoomRangePtr > ()) ; }
#line 879 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 28:
#line 157 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::WriteAllCommand>( tag_filter::ZoomRange(0, 255)) ; }
#line 885 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 29:
#line 159 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::ExcludeCommand>(*yystack_[2].value.as< tag_filter::ZoomRangePtr > (), *yystack_[1].value.as< tag_filter::TagListPtr > ()) ; }
#line 891 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 30:
#line 160 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::ExcludeCommand>( tag_filter::ZoomRange(0, 255), *yystack_[1].value.as< tag_filter::TagListPtr > ()) ; }
#line 897 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 31:
#line 162 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::SimpleCommand>( tag_filter::SimpleCommand::Continue) ;}
#line 903 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 32:
#line 164 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::RuleCommand>( yystack_[0].value.as< tag_filter::RulePtr > ()) ;}
#line 909 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 33:
#line 165 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::CommandPtr > () = std::make_shared<tag_filter::FunctionCommand>(yystack_[1].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 915 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 34:
#line 170 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 921 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 35:
#line 171 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BooleanOperator>( tag_filter::BooleanOperator::Or, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 927 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 36:
#line 175 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 933 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 37:
#line 176 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BooleanOperator>( tag_filter::BooleanOperator::And, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 939 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 38:
#line 180 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 945 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 39:
#line 181 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BooleanOperator>( tag_filter::BooleanOperator::Not, yystack_[0].value.as< tag_filter::ExpressionNodePtr > (), nullptr) ; }
#line 951 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 40:
#line 185 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 957 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 41:
#line 186 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[1].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 963 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 42:
#line 190 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 969 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 43:
#line 191 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 975 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 44:
#line 192 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 981 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 45:
#line 193 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 987 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 46:
#line 197 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::UnaryPredicate>( yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ;}
#line 993 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 47:
#line 200 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::Equal, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ; }
#line 999 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 48:
#line 201 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::NotEqual, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ; }
#line 1005 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 49:
#line 202 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::Less, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ; }
#line 1011 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 50:
#line 203 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::Greater, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ; }
#line 1017 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 51:
#line 204 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::LessOrEqual, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ; }
#line 1023 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 52:
#line 205 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ComparisonPredicate>( tag_filter::ComparisonPredicate::GreaterOrEqual, yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ) ; }
#line 1029 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 53:
#line 209 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::LikeTextPredicate>(yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), true) ; }
#line 1035 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 54:
#line 210 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::LikeTextPredicate>(yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< std::string > (), false) ; }
#line 1041 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 55:
#line 214 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ListPredicate>(yystack_[4].value.as< std::string > (), yystack_[1].value.as< tag_filter::ExpressionListPtr > ()->children(), true) ; }
#line 1047 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 56:
#line 215 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::ListPredicate>(yystack_[5].value.as< std::string > (), yystack_[1].value.as< tag_filter::ExpressionListPtr > ()->children(), false) ; }
#line 1053 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 57:
#line 219 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionListPtr > () = std::make_shared<tag_filter::ExpressionList>() ;  yylhs.value.as< tag_filter::ExpressionListPtr > ()->append(yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ;  }
#line 1059 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 58:
#line 220 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionListPtr > () = yystack_[0].value.as< tag_filter::ExpressionListPtr > () ; yystack_[0].value.as< tag_filter::ExpressionListPtr > ()->prepend(yystack_[2].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1065 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 59:
#line 224 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1071 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 60:
#line 225 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BinaryOperator>('+',yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1077 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 61:
#line 226 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BinaryOperator>('.',yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1083 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 62:
#line 227 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BinaryOperator>('-', yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1089 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 63:
#line 231 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1095 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 64:
#line 232 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BinaryOperator>('*', yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1101 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 65:
#line 233 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::BinaryOperator>('/', yystack_[2].value.as< tag_filter::ExpressionNodePtr > (), yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1107 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 66:
#line 237 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1113 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 67:
#line 238 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1119 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 68:
#line 239 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1125 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 69:
#line 240 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[1].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1131 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 70:
#line 244 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::Function>(yystack_[2].value.as< std::string > (), &driver.lua_) ; }
#line 1137 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 71:
#line 245 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    {
                        yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::Function>(yystack_[3].value.as< std::string > (), yystack_[1].value.as< tag_filter::ExpressionListPtr > ()->children(), &driver.lua_) ;
		 }
#line 1145 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 72:
#line 251 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    {
                                yylhs.value.as< tag_filter::ExpressionListPtr > () = std::make_shared<tag_filter::ExpressionList>() ;
				yylhs.value.as< tag_filter::ExpressionListPtr > ()->append(yystack_[0].value.as< tag_filter::ExpressionNodePtr > ()) ;
			}
#line 1154 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 73:
#line 255 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionListPtr > () = yystack_[0].value.as< tag_filter::ExpressionListPtr > () ; yystack_[0].value.as< tag_filter::ExpressionListPtr > ()->prepend(yystack_[2].value.as< tag_filter::ExpressionNodePtr > ()) ; }
#line 1160 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 74:
#line 259 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1166 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 75:
#line 263 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1172 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 76:
#line 264 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1178 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 77:
#line 268 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::LiteralExpressionNode>(yystack_[0].value.as< std::string > ()) ; }
#line 1184 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 78:
#line 269 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = yystack_[0].value.as< tag_filter::ExpressionNodePtr > () ; }
#line 1190 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 79:
#line 274 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::LiteralExpressionNode>(true) ; }
#line 1196 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 80:
#line 275 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    { yylhs.value.as< tag_filter::ExpressionNodePtr > () =  std::make_shared<tag_filter::LiteralExpressionNode>(false) ; }
#line 1202 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 81:
#line 279 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    {
                yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::LiteralExpressionNode>((double)yystack_[0].value.as< double > ()) ;
	}
#line 1210 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;

  case 82:
#line 285 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:847
    {
                yylhs.value.as< tag_filter::ExpressionNodePtr > () = std::make_shared<tag_filter::Attribute>(yystack_[0].value.as< std::string > ()) ;
	}
#line 1218 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
    break;


#line 1222 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:847
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
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type yystate, symbol_number_type yytoken) const
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


  const short int Parser::yypact_ninf_ = -138;

  const signed char Parser::yytable_ninf_ = -1;

  const short int
  Parser::yypact_[] =
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
  Parser::yydefact_[] =
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
  Parser::yypgoto_[] =
  {
    -138,  -138,   112,    11,    89,   145,   121,    72,   -14,   116,
    -138,   142,    96,    92,   150,  -138,  -138,  -138,  -138,  -138,
    -137,   -24,    49,  -138,    62,    32,  -138,  -110,  -138,  -138,
    -138,  -138
  };

  const short int
  Parser::yydefgoto_[] =
  {
      -1,     4,     5,     6,     7,    18,    54,    55,    60,    56,
      19,    28,    29,    30,    31,    32,    33,    34,    35,    36,
     143,    37,    38,    39,    40,   107,   108,    41,    42,    43,
      44,    45
  };

  const unsigned char
  Parser::yytable_[] =
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
  Parser::yycheck_[] =
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
  Parser::yystos_[] =
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
  Parser::yyr1_[] =
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
  Parser::yyr2_[] =
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
  const Parser::yytname_[] =
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
  Parser::yyrline_[] =
  {
       0,   104,   104,   105,   107,   108,   111,   112,   117,   122,
     123,   127,   128,   129,   130,   133,   134,   137,   138,   141,
     142,   146,   148,   150,   152,   153,   154,   156,   157,   159,
     160,   162,   164,   165,   170,   171,   175,   176,   180,   181,
     185,   186,   190,   191,   192,   193,   197,   200,   201,   202,
     203,   204,   205,   209,   210,   214,   215,   219,   220,   224,
     225,   226,   227,   231,   232,   233,   237,   238,   239,   240,
     244,   245,   251,   255,   259,   263,   264,   268,   269,   274,
     275,   279,   285
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
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
  Parser::yy_reduce_print_ (int yyrule)
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


#line 9 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:1155
} // tag_filter
#line 1708 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.cpp" // lalr1.cc:1155
#line 290 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:1156

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



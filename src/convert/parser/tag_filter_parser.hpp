// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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

/**
 ** \file /home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.hpp
 ** Define the tag_filter::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_YY_HOME_MALASIOT_SOURCE_MFTOOLS_SRC_CONVERT_PARSER_TAG_FILTER_PARSER_HPP_INCLUDED
# define YY_YY_HOME_MALASIOT_SOURCE_MFTOOLS_SRC_CONVERT_PARSER_TAG_FILTER_PARSER_HPP_INCLUDED
// //                    "%code requires" blocks.
#line 19 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:372

#include "tag_filter_rule.hpp"
class TagFilterConfigParser ;

#line 49 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.hpp" // lalr1.cc:372


# include <vector>
# include <iostream>
# include <stdexcept>
# include <string>
# include "stack.hh"
# include "location.hh"

#ifndef YYASSERT
# include <cassert>
# define YYASSERT assert
#endif


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 9 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:372
namespace tag_filter {
#line 125 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.hpp" // lalr1.cc:372



  /// A char[S] buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current state.
  template <size_t S>
  struct variant
  {
    /// Type of *this.
    typedef variant<S> self_type;

    /// Empty construction.
    variant ()
    {}

    /// Construct and fill.
    template <typename T>
    variant (const T& t)
    {
      YYASSERT (sizeof (T) <= S);
      new (yyas_<T> ()) T (t);
    }

    /// Destruction, allowed only if empty.
    ~variant ()
    {}

    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    build ()
    {
      return *new (yyas_<T> ()) T;
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    build (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as ()
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a other, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsability.
    /// Swapping between built and (possibly) non-built is done with
    /// variant::move ().
    template <typename T>
    void
    swap (self_type& other)
    {
      std::swap (as<T> (), other.as<T> ());
    }

    /// Move the content of \a other to this.
    ///
    /// Destroys \a other.
    template <typename T>
    void
    move (self_type& other)
    {
      build<T> ();
      swap<T> (other);
      other.destroy<T> ();
    }

    /// Copy the content of \a other to this.
    template <typename T>
    void
    copy (const self_type& other)
    {
      build<T> (other.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
    /// Prohibit blind copies.
    self_type& operator=(const self_type&);
    variant (const self_type&);

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ ()
    {
      void *yyp = yybuffer_.yyraw;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const
    {
      const void *yyp = yybuffer_.yyraw;
      return static_cast<const T*> (yyp);
     }

    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store any of the semantic values.
      char yyraw[S];
    } yybuffer_;
  };


  /// A Bison parser.
  class Parser
  {
  public:
#ifndef YYSTYPE
    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // "number"
      char dummy1[sizeof(double)];

      // "identifier"
      // "string literal"
      // "LUA script"
      char dummy2[sizeof(std::string)];

      // action_block
      // command_list
      char dummy3[sizeof(tag_filter::CommandListPtr)];

      // command
      char dummy4[sizeof(tag_filter::CommandPtr)];

      // literal_list
      // function_argument_list
      char dummy5[sizeof(tag_filter::ExpressionListPtr)];

      // boolean_value_expression
      // boolean_term
      // boolean_factor
      // boolean_primary
      // predicate
      // unary_predicate
      // comparison_predicate
      // like_text_predicate
      // list_predicate
      // expression
      // term
      // factor
      // function
      // function_argument
      // literal
      // general_literal
      // boolean_literal
      // numeric_literal
      // attribute
      char dummy6[sizeof(tag_filter::ExpressionNodePtr)];

      // rule_list
      char dummy7[sizeof(tag_filter::RuleListPtr)];

      // rule
      char dummy8[sizeof(tag_filter::RulePtr)];

      // tag_decl_list
      char dummy9[sizeof(tag_filter::TagDeclarationListPtr)];

      // tag_decl
      char dummy10[sizeof(tag_filter::TagDeclarationPtr)];

      // tag_list
      char dummy11[sizeof(tag_filter::TagListPtr)];

      // zoom_range
      char dummy12[sizeof(tag_filter::ZoomRangePtr)];

      // "zoom specifier"
      char dummy13[sizeof(uint8_t)];
};

    /// Symbol semantic values.
    typedef variant<sizeof(union_type)> semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        TOK_END = 0,
        TOK_NOT = 258,
        TOK_AND = 259,
        TOK_OR = 260,
        TOK_MATCHES = 261,
        TOK_NOT_MATCHES = 262,
        TOK_EQUAL = 263,
        TOK_NOT_EQUAL = 264,
        TOK_LESS_THAN = 265,
        TOK_GREATER_THAN = 266,
        TOK_LESS_THAN_OR_EQUAL = 267,
        TOK_GREATER_THAN_OR_EQUAL = 268,
        TOK_TRUEX = 269,
        TOK_FALSEX = 270,
        TOK_EXISTS = 271,
        TOK_PLUS = 272,
        TOK_MINUS = 273,
        TOK_STAR = 274,
        TOK_DIV = 275,
        TOK_LPAR = 276,
        TOK_RPAR = 277,
        TOK_COMMA = 278,
        TOK_DOT = 279,
        TOK_LEFT_BRACE = 280,
        TOK_RIGHT_BRACE = 281,
        TOK_LEFT_BRACKET = 282,
        TOK_RIGHT_BRACKET = 283,
        TOK_COLON = 284,
        TOK_ADD_CMD = 285,
        TOK_SET_CMD = 286,
        TOK_DELETE_CMD = 287,
        TOK_WRITE_CMD = 288,
        TOK_CONTINUE_CMD = 289,
        TOK_ASSIGN = 290,
        TOK_IN = 291,
        TOK_LAYER = 292,
        TOK_EXCLUDE_CMD = 293,
        TOK_WRITE_ALL_CMD = 294,
        TOK_ATTACH_CMD = 295,
        TOK_IDENTIFIER = 296,
        TOK_NUMBER = 297,
        TOK_STRING = 298,
        TOK_LUA_SCRIPT = 299,
        TOK_ZOOM_SPEC = 300,
        TOK_UMINUS = 301
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Internal symbol number.
    typedef int symbol_number_type;

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols, and symbols from each type.

  basic_symbol (typename Base::kind_type t, const location_type& l);

  basic_symbol (typename Base::kind_type t, const double v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::CommandListPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::CommandPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::ExpressionListPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::ExpressionNodePtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::RuleListPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::RulePtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::TagDeclarationListPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::TagDeclarationPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::TagListPtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const tag_filter::ZoomRangePtr v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const uint8_t v, const location_type& l);


      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      ~basic_symbol ();

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// -1 when this symbol is empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      enum { empty = 0 };

      /// The symbol type.
      /// -1 when this symbol is empty.
      token_number_type type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;

    // Symbol constructors declarations.
    static inline
    symbol_type
    make_END (const location_type& l);

    static inline
    symbol_type
    make_NOT (const location_type& l);

    static inline
    symbol_type
    make_AND (const location_type& l);

    static inline
    symbol_type
    make_OR (const location_type& l);

    static inline
    symbol_type
    make_MATCHES (const location_type& l);

    static inline
    symbol_type
    make_NOT_MATCHES (const location_type& l);

    static inline
    symbol_type
    make_EQUAL (const location_type& l);

    static inline
    symbol_type
    make_NOT_EQUAL (const location_type& l);

    static inline
    symbol_type
    make_LESS_THAN (const location_type& l);

    static inline
    symbol_type
    make_GREATER_THAN (const location_type& l);

    static inline
    symbol_type
    make_LESS_THAN_OR_EQUAL (const location_type& l);

    static inline
    symbol_type
    make_GREATER_THAN_OR_EQUAL (const location_type& l);

    static inline
    symbol_type
    make_TRUEX (const location_type& l);

    static inline
    symbol_type
    make_FALSEX (const location_type& l);

    static inline
    symbol_type
    make_EXISTS (const location_type& l);

    static inline
    symbol_type
    make_PLUS (const location_type& l);

    static inline
    symbol_type
    make_MINUS (const location_type& l);

    static inline
    symbol_type
    make_STAR (const location_type& l);

    static inline
    symbol_type
    make_DIV (const location_type& l);

    static inline
    symbol_type
    make_LPAR (const location_type& l);

    static inline
    symbol_type
    make_RPAR (const location_type& l);

    static inline
    symbol_type
    make_COMMA (const location_type& l);

    static inline
    symbol_type
    make_DOT (const location_type& l);

    static inline
    symbol_type
    make_LEFT_BRACE (const location_type& l);

    static inline
    symbol_type
    make_RIGHT_BRACE (const location_type& l);

    static inline
    symbol_type
    make_LEFT_BRACKET (const location_type& l);

    static inline
    symbol_type
    make_RIGHT_BRACKET (const location_type& l);

    static inline
    symbol_type
    make_COLON (const location_type& l);

    static inline
    symbol_type
    make_ADD_CMD (const location_type& l);

    static inline
    symbol_type
    make_SET_CMD (const location_type& l);

    static inline
    symbol_type
    make_DELETE_CMD (const location_type& l);

    static inline
    symbol_type
    make_WRITE_CMD (const location_type& l);

    static inline
    symbol_type
    make_CONTINUE_CMD (const location_type& l);

    static inline
    symbol_type
    make_ASSIGN (const location_type& l);

    static inline
    symbol_type
    make_IN (const location_type& l);

    static inline
    symbol_type
    make_LAYER (const location_type& l);

    static inline
    symbol_type
    make_EXCLUDE_CMD (const location_type& l);

    static inline
    symbol_type
    make_WRITE_ALL_CMD (const location_type& l);

    static inline
    symbol_type
    make_ATTACH_CMD (const location_type& l);

    static inline
    symbol_type
    make_IDENTIFIER (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_NUMBER (const double& v, const location_type& l);

    static inline
    symbol_type
    make_STRING (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_LUA_SCRIPT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_ZOOM_SPEC (const uint8_t& v, const location_type& l);

    static inline
    symbol_type
    make_UMINUS (const location_type& l);


    /// Build a parser object.
    Parser (TagFilterConfigParser &driver_yyarg, tag_filter::Parser::location_type &loc_yyarg);
    virtual ~Parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
    Parser (const Parser&);
    Parser& operator= (const Parser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yytoken   the lookahead token type, or yyempty_.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         symbol_number_type yytoken) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short int yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (token_type t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short int yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned char yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short int yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short int yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const unsigned char yytable_[];

  static const short int yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned char yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned char yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if YYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short int yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// "empty" when empty.
      symbol_number_type type_get () const;

      enum { empty = 0 };

      /// The state.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned int n = 1);

    // Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 173,     ///< Last index in yytable_.
      yynnts_ = 32,  ///< Number of nonterminal symbols.
      yyempty_ = -2,
      yyfinal_ = 47, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 47  ///< Number of tokens.
    };


    // User arguments.
    TagFilterConfigParser &driver;
    tag_filter::Parser::location_type &loc;
  };

  // Symbol number corresponding to token number t.
  inline
  Parser::token_number_type
  Parser::yytranslate_ (token_type t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
    };
    const unsigned int user_token_number_max_ = 301;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
      switch (other.type_get ())
    {
      case 42: // "number"
        value.copy< double > (other.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.copy< std::string > (other.value);
        break;

      case 51: // action_block
      case 52: // command_list
        value.copy< tag_filter::CommandListPtr > (other.value);
        break;

      case 57: // command
        value.copy< tag_filter::CommandPtr > (other.value);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.copy< tag_filter::ExpressionListPtr > (other.value);
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
        value.copy< tag_filter::ExpressionNodePtr > (other.value);
        break;

      case 49: // rule_list
        value.copy< tag_filter::RuleListPtr > (other.value);
        break;

      case 50: // rule
        value.copy< tag_filter::RulePtr > (other.value);
        break;

      case 56: // tag_decl_list
        value.copy< tag_filter::TagDeclarationListPtr > (other.value);
        break;

      case 54: // tag_decl
        value.copy< tag_filter::TagDeclarationPtr > (other.value);
        break;

      case 55: // tag_list
        value.copy< tag_filter::TagListPtr > (other.value);
        break;

      case 53: // zoom_range
        value.copy< tag_filter::ZoomRangePtr > (other.value);
        break;

      case 45: // "zoom specifier"
        value.copy< uint8_t > (other.value);
        break;

      default:
        break;
    }

  }


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {
    (void) v;
      switch (this->type_get ())
    {
      case 42: // "number"
        value.copy< double > (v);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.copy< std::string > (v);
        break;

      case 51: // action_block
      case 52: // command_list
        value.copy< tag_filter::CommandListPtr > (v);
        break;

      case 57: // command
        value.copy< tag_filter::CommandPtr > (v);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.copy< tag_filter::ExpressionListPtr > (v);
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
        value.copy< tag_filter::ExpressionNodePtr > (v);
        break;

      case 49: // rule_list
        value.copy< tag_filter::RuleListPtr > (v);
        break;

      case 50: // rule
        value.copy< tag_filter::RulePtr > (v);
        break;

      case 56: // tag_decl_list
        value.copy< tag_filter::TagDeclarationListPtr > (v);
        break;

      case 54: // tag_decl
        value.copy< tag_filter::TagDeclarationPtr > (v);
        break;

      case 55: // tag_list
        value.copy< tag_filter::TagListPtr > (v);
        break;

      case 53: // zoom_range
        value.copy< tag_filter::ZoomRangePtr > (v);
        break;

      case 45: // "zoom specifier"
        value.copy< uint8_t > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const double v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::CommandListPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::CommandPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::ExpressionListPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::ExpressionNodePtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::RuleListPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::RulePtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::TagDeclarationListPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::TagDeclarationPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::TagListPtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const tag_filter::ZoomRangePtr v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const uint8_t v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 42: // "number"
        value.template destroy< double > ();
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.template destroy< std::string > ();
        break;

      case 51: // action_block
      case 52: // command_list
        value.template destroy< tag_filter::CommandListPtr > ();
        break;

      case 57: // command
        value.template destroy< tag_filter::CommandPtr > ();
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.template destroy< tag_filter::ExpressionListPtr > ();
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
        value.template destroy< tag_filter::ExpressionNodePtr > ();
        break;

      case 49: // rule_list
        value.template destroy< tag_filter::RuleListPtr > ();
        break;

      case 50: // rule
        value.template destroy< tag_filter::RulePtr > ();
        break;

      case 56: // tag_decl_list
        value.template destroy< tag_filter::TagDeclarationListPtr > ();
        break;

      case 54: // tag_decl
        value.template destroy< tag_filter::TagDeclarationPtr > ();
        break;

      case 55: // tag_list
        value.template destroy< tag_filter::TagListPtr > ();
        break;

      case 53: // zoom_range
        value.template destroy< tag_filter::ZoomRangePtr > ();
        break;

      case 45: // "zoom specifier"
        value.template destroy< uint8_t > ();
        break;

      default:
        break;
    }

  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 42: // "number"
        value.move< double > (s.value);
        break;

      case 41: // "identifier"
      case 43: // "string literal"
      case 44: // "LUA script"
        value.move< std::string > (s.value);
        break;

      case 51: // action_block
      case 52: // command_list
        value.move< tag_filter::CommandListPtr > (s.value);
        break;

      case 57: // command
        value.move< tag_filter::CommandPtr > (s.value);
        break;

      case 67: // literal_list
      case 72: // function_argument_list
        value.move< tag_filter::ExpressionListPtr > (s.value);
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
        value.move< tag_filter::ExpressionNodePtr > (s.value);
        break;

      case 49: // rule_list
        value.move< tag_filter::RuleListPtr > (s.value);
        break;

      case 50: // rule
        value.move< tag_filter::RulePtr > (s.value);
        break;

      case 56: // tag_decl_list
        value.move< tag_filter::TagDeclarationListPtr > (s.value);
        break;

      case 54: // tag_decl
        value.move< tag_filter::TagDeclarationPtr > (s.value);
        break;

      case 55: // tag_list
        value.move< tag_filter::TagListPtr > (s.value);
        break;

      case 53: // zoom_range
        value.move< tag_filter::ZoomRangePtr > (s.value);
        break;

      case 45: // "zoom specifier"
        value.move< uint8_t > (s.value);
        break;

      default:
        break;
    }

    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
     : type (empty)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }

  inline
  Parser::token_type
  Parser::by_type::token () const
  {
    // YYTOKNUM[NUM] -- (External) token number corresponding to the
    // (internal) symbol number NUM (which must be that of a token).  */
    static
    const unsigned short int
    yytoken_number_[] =
    {
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301
    };
    return static_cast<token_type> (yytoken_number_[type]);
  }
  // Implementation of make_symbol for each symbol type.
  Parser::symbol_type
  Parser::make_END (const location_type& l)
  {
    return symbol_type (token::TOK_END, l);
  }

  Parser::symbol_type
  Parser::make_NOT (const location_type& l)
  {
    return symbol_type (token::TOK_NOT, l);
  }

  Parser::symbol_type
  Parser::make_AND (const location_type& l)
  {
    return symbol_type (token::TOK_AND, l);
  }

  Parser::symbol_type
  Parser::make_OR (const location_type& l)
  {
    return symbol_type (token::TOK_OR, l);
  }

  Parser::symbol_type
  Parser::make_MATCHES (const location_type& l)
  {
    return symbol_type (token::TOK_MATCHES, l);
  }

  Parser::symbol_type
  Parser::make_NOT_MATCHES (const location_type& l)
  {
    return symbol_type (token::TOK_NOT_MATCHES, l);
  }

  Parser::symbol_type
  Parser::make_EQUAL (const location_type& l)
  {
    return symbol_type (token::TOK_EQUAL, l);
  }

  Parser::symbol_type
  Parser::make_NOT_EQUAL (const location_type& l)
  {
    return symbol_type (token::TOK_NOT_EQUAL, l);
  }

  Parser::symbol_type
  Parser::make_LESS_THAN (const location_type& l)
  {
    return symbol_type (token::TOK_LESS_THAN, l);
  }

  Parser::symbol_type
  Parser::make_GREATER_THAN (const location_type& l)
  {
    return symbol_type (token::TOK_GREATER_THAN, l);
  }

  Parser::symbol_type
  Parser::make_LESS_THAN_OR_EQUAL (const location_type& l)
  {
    return symbol_type (token::TOK_LESS_THAN_OR_EQUAL, l);
  }

  Parser::symbol_type
  Parser::make_GREATER_THAN_OR_EQUAL (const location_type& l)
  {
    return symbol_type (token::TOK_GREATER_THAN_OR_EQUAL, l);
  }

  Parser::symbol_type
  Parser::make_TRUEX (const location_type& l)
  {
    return symbol_type (token::TOK_TRUEX, l);
  }

  Parser::symbol_type
  Parser::make_FALSEX (const location_type& l)
  {
    return symbol_type (token::TOK_FALSEX, l);
  }

  Parser::symbol_type
  Parser::make_EXISTS (const location_type& l)
  {
    return symbol_type (token::TOK_EXISTS, l);
  }

  Parser::symbol_type
  Parser::make_PLUS (const location_type& l)
  {
    return symbol_type (token::TOK_PLUS, l);
  }

  Parser::symbol_type
  Parser::make_MINUS (const location_type& l)
  {
    return symbol_type (token::TOK_MINUS, l);
  }

  Parser::symbol_type
  Parser::make_STAR (const location_type& l)
  {
    return symbol_type (token::TOK_STAR, l);
  }

  Parser::symbol_type
  Parser::make_DIV (const location_type& l)
  {
    return symbol_type (token::TOK_DIV, l);
  }

  Parser::symbol_type
  Parser::make_LPAR (const location_type& l)
  {
    return symbol_type (token::TOK_LPAR, l);
  }

  Parser::symbol_type
  Parser::make_RPAR (const location_type& l)
  {
    return symbol_type (token::TOK_RPAR, l);
  }

  Parser::symbol_type
  Parser::make_COMMA (const location_type& l)
  {
    return symbol_type (token::TOK_COMMA, l);
  }

  Parser::symbol_type
  Parser::make_DOT (const location_type& l)
  {
    return symbol_type (token::TOK_DOT, l);
  }

  Parser::symbol_type
  Parser::make_LEFT_BRACE (const location_type& l)
  {
    return symbol_type (token::TOK_LEFT_BRACE, l);
  }

  Parser::symbol_type
  Parser::make_RIGHT_BRACE (const location_type& l)
  {
    return symbol_type (token::TOK_RIGHT_BRACE, l);
  }

  Parser::symbol_type
  Parser::make_LEFT_BRACKET (const location_type& l)
  {
    return symbol_type (token::TOK_LEFT_BRACKET, l);
  }

  Parser::symbol_type
  Parser::make_RIGHT_BRACKET (const location_type& l)
  {
    return symbol_type (token::TOK_RIGHT_BRACKET, l);
  }

  Parser::symbol_type
  Parser::make_COLON (const location_type& l)
  {
    return symbol_type (token::TOK_COLON, l);
  }

  Parser::symbol_type
  Parser::make_ADD_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_ADD_CMD, l);
  }

  Parser::symbol_type
  Parser::make_SET_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_SET_CMD, l);
  }

  Parser::symbol_type
  Parser::make_DELETE_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_DELETE_CMD, l);
  }

  Parser::symbol_type
  Parser::make_WRITE_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_WRITE_CMD, l);
  }

  Parser::symbol_type
  Parser::make_CONTINUE_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_CONTINUE_CMD, l);
  }

  Parser::symbol_type
  Parser::make_ASSIGN (const location_type& l)
  {
    return symbol_type (token::TOK_ASSIGN, l);
  }

  Parser::symbol_type
  Parser::make_IN (const location_type& l)
  {
    return symbol_type (token::TOK_IN, l);
  }

  Parser::symbol_type
  Parser::make_LAYER (const location_type& l)
  {
    return symbol_type (token::TOK_LAYER, l);
  }

  Parser::symbol_type
  Parser::make_EXCLUDE_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_EXCLUDE_CMD, l);
  }

  Parser::symbol_type
  Parser::make_WRITE_ALL_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_WRITE_ALL_CMD, l);
  }

  Parser::symbol_type
  Parser::make_ATTACH_CMD (const location_type& l)
  {
    return symbol_type (token::TOK_ATTACH_CMD, l);
  }

  Parser::symbol_type
  Parser::make_IDENTIFIER (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOK_IDENTIFIER, v, l);
  }

  Parser::symbol_type
  Parser::make_NUMBER (const double& v, const location_type& l)
  {
    return symbol_type (token::TOK_NUMBER, v, l);
  }

  Parser::symbol_type
  Parser::make_STRING (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOK_STRING, v, l);
  }

  Parser::symbol_type
  Parser::make_LUA_SCRIPT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOK_LUA_SCRIPT, v, l);
  }

  Parser::symbol_type
  Parser::make_ZOOM_SPEC (const uint8_t& v, const location_type& l)
  {
    return symbol_type (token::TOK_ZOOM_SPEC, v, l);
  }

  Parser::symbol_type
  Parser::make_UMINUS (const location_type& l)
  {
    return symbol_type (token::TOK_UMINUS, l);
  }


#line 9 "/home/malasiot/source/mftools/src/convert/tag_filter.y" // lalr1.cc:372
} // tag_filter
#line 1765 "/home/malasiot/source/mftools/src/convert/parser/tag_filter_parser.hpp" // lalr1.cc:372




#endif // !YY_YY_HOME_MALASIOT_SOURCE_MFTOOLS_SRC_CONVERT_PARSER_TAG_FILTER_PARSER_HPP_INCLUDED

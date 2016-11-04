#ifndef __OSM_FILTER_RULE_HPP__
#define __OSM_FILTER_RULE_HPP__

#include <memory>
#include <deque>
#include <vector>
#include <boost/regex.hpp>

class LuaContext ;

namespace OSM {
namespace Filter {

class ExpressionNode ;
class Context ;
class Command ;
class Rule ;

typedef std::shared_ptr<ExpressionNode> ExpressionNodePtr ;
typedef std::shared_ptr<Rule> RulePtr ;
typedef std::shared_ptr<Command> CommandPtr ;

struct Literal {
public:
    enum Type { String, Number, Boolean, Null } ;

    Literal(): type_(Null) {}
    Literal(const std::string &val, bool auto_conv = true) ;
    Literal(const double val): type_(Number), number_val_(val) {}
    Literal(const bool val): type_(Boolean), boolean_val_(val) {}

    bool isNull() const { return type_ == Null ; }
    bool toBoolean() const ;
    std::string toString() const ;
    double toNumber() const ;

    Type type_ ;
    std::string string_val_ ;
    double number_val_ ;
    bool boolean_val_ ;
};

class CommandList {
public:
    std::deque<CommandPtr> commands_ ;
};
typedef std::shared_ptr<CommandList> CommandListPtr ;

class Rule {
public:

    Rule(ExpressionNodePtr cond, const std::deque<CommandPtr> &cmds): condition_(cond), commands_(cmds) {}

    ExpressionNodePtr condition_ ;
    std::deque<CommandPtr> commands_ ;
};

class RuleList {
public:
    std::deque<RulePtr> rules_ ;
};

typedef std::shared_ptr<RuleList> RuleListPtr ;

class ZoomRange {
public:

    ZoomRange(uint8_t min_zoom, uint8_t max_zoom):
    min_zoom_(min_zoom), max_zoom_(max_zoom){}

    uint8_t min_zoom_, max_zoom_ ;
};

typedef std::shared_ptr<ZoomRange> ZoomRangePtr ;

class ExpressionNode {
public:

    ExpressionNode() {}

    virtual Literal eval(Context &ctx) { return false ; }
};

class ExpressionList {
public:
    ExpressionList() {}

    void append(ExpressionNodePtr node) { children_.push_back(node) ; }
    void prepend(ExpressionNodePtr node) { children_.push_front(node) ; }

    const std::deque<ExpressionNodePtr> &children() const { return children_ ; }

private:
    std::deque<ExpressionNodePtr> children_ ;
};

typedef std::shared_ptr<ExpressionList> ExpressionListPtr ;

class Command {
public:
    enum Type { Set, Add, Write, Continue, Delete, WriteAll, Exclude, Attach, Conditional, UserFunction } ;

    Command() {}

    virtual Type type() const = 0 ;
    virtual ~Command() {}
};

class SimpleCommand: public Command {
public:

    SimpleCommand(Type cmd, std::string ident = std::string(), ExpressionNodePtr val = ExpressionNodePtr()): cmd_(cmd), tag_(ident), val_(val) {}

    Type type() const { return cmd_ ; }

    ExpressionNodePtr val_ ;
    std::string tag_ ;
    Type cmd_ ;
};


class TagList {
public:
    std::vector<std::string> tags_ ;
};

typedef std::shared_ptr<TagList> TagListPtr ;

class TagDeclaration {
public:
    TagDeclaration(const std::string &tag, const ExpressionNodePtr &val): tag_(tag), val_(val) {}

    std::string tag_ ;
    ExpressionNodePtr val_ ;
};

typedef std::shared_ptr<TagDeclaration> TagDeclarationPtr ;

class TagDeclarationList {
public:

    std::deque<TagDeclarationPtr> tags_ ;
};

typedef std::shared_ptr<TagDeclarationList> TagDeclarationListPtr ;

class AttachCommand: public Command {
public:

    AttachCommand(const TagList tags): tags_(tags) {}

    Type type() const { return Attach ; }

    TagList tags_ ;

};

class WriteCommand: public Command {
public:

    WriteCommand(const ZoomRange &zr, const TagDeclarationList &tags): tags_(tags), zoom_range_(zr) {}

    Type type() const { return Write ; }

    ZoomRange zoom_range_ ;
    TagDeclarationList tags_ ;
};

class ExcludeCommand: public Command {
public:

    ExcludeCommand(const ZoomRange &zr, const TagList &tags): tags_(tags.tags_), zoom_range_(zr) {}

    Type type() const { return Exclude ; }

    ZoomRange zoom_range_ ;
    std::vector<std::string> tags_ ;
};

class WriteAllCommand: public Command {
public:

    WriteAllCommand(const ZoomRange &zr): zoom_range_(zr) {}

    Type type() const { return WriteAll ; }

    ZoomRange zoom_range_ ;

};

class Function ;

class FunctionCommand: public Command {
public:

    Type type() const { return UserFunction ; }

    FunctionCommand(ExpressionNodePtr func): func_(func) {}

    ExpressionNodePtr func_ ;
};

class RuleCommand: public Command {
public:

    Type type() const { return Conditional ; }

    RuleCommand(RulePtr rule): rule_(rule) {}

    RulePtr rule_ ;
};

class ActionBlock {
public:
    ActionBlock() {}
    std::deque<CommandPtr> commands_ ;
};

class LiteralExpressionNode: public ExpressionNode {
public:

    LiteralExpressionNode(const Literal &l): val_(l) {}

    Literal eval(Context &ctx) { return val_ ; }

    Literal val_ ;
};

class Attribute: public ExpressionNode {
public:
    Attribute(const std::string name): name_(name) {}

    Literal eval(Context &ctx) ;

private:
    std::string name_ ;
};


class Function: public ExpressionNode {
public:
    Function(const std::string &name, LuaContext *lua): name_(name), lua_(lua) {}
    Function(const std::string &name, const std::deque<ExpressionNodePtr> &args, LuaContext *lua): name_(name), args_(args), lua_(lua) {}

    Literal eval(Context &ctx) ;

private:
    std::string name_ ;
    std::deque<ExpressionNodePtr> args_ ;
    LuaContext *lua_ ;
};


class BinaryOperator: public ExpressionNode {
public:
    BinaryOperator(int op, ExpressionNodePtr lhs, ExpressionNodePtr rhs): op_(op), lhs_(lhs), rhs_(rhs) {}

    Literal eval(Context &ctx) ;

private:
    int op_ ;
    ExpressionNodePtr lhs_, rhs_ ;
};


class BooleanOperator: public ExpressionNode {
public:
    enum Type { And, Or, Not } ;

    BooleanOperator(Type op, ExpressionNodePtr lhs, ExpressionNodePtr rhs): op_(op), lhs_(lhs), rhs_(rhs) {}

    Literal eval(Context &ctx) ;
private:
    Type op_ ;
    ExpressionNodePtr lhs_, rhs_ ;
};

class UnaryPredicate: public ExpressionNode {
public:

    UnaryPredicate(ExpressionNodePtr exp): exp_(exp) {}

    Literal eval(Context &ctx) ;

    ExpressionNodePtr exp_ ;
};

class ComparisonPredicate: public ExpressionNode {
public:
    enum Type { Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } ;

    ComparisonPredicate(Type op, ExpressionNodePtr lhs, ExpressionNodePtr rhs): op_(op), lhs_(lhs), rhs_(rhs) {}


    Literal eval(Context &ctx) ;

private:
    Type op_ ;
    ExpressionNodePtr lhs_, rhs_ ;
};

class LikeTextPredicate: public ExpressionNode {
public:

    LikeTextPredicate(ExpressionNodePtr op, const std::string &pattern_, bool is_pos) ;

    Literal eval(Context &ctx) ;
private:
    ExpressionNodePtr exp_ ;
    boost::regex pattern_ ;
    bool is_pos_ ;
};

class ListPredicate: public ExpressionNode {
public:

    ListPredicate(const std::string &identifier, const std::deque<ExpressionNodePtr> &op, bool is_pos) ;

    Literal eval(Context &ctx) ;

private:
    std::vector<ExpressionNodePtr> children_ ;
    std::string id_ ;
    std::vector<std::string> lvals_ ;
    bool is_pos_ ;

};

} // namespace Filter
} // namespace OSM
#endif

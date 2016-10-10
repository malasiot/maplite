#ifndef __OSM_RULE_PARSER_H__
#define __OSM_RULE_PARSER_H__

#include "osm_rule_scanner.hpp"
#include "osm_document.hpp"

#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <regex>

namespace OSM {

class FlexScanner ;

namespace Filter {

class ExpressionNode ;
class Context ;
class Command ;
class Rule ;

typedef std::shared_ptr<ExpressionNode> ExpressionNodePtr ;
typedef std::shared_ptr<Rule> RulePtr ;
typedef std::shared_ptr<Command> CommandPtr ;

class Parser {

public:

    Parser(std::istream &strm)  ;

    bool parse() ;

    void error(const BisonParser::location_type &loc,  const std::string& m) ;

    FlexScanner scanner_;
    BisonParser parser_;

    std::deque<RulePtr> rules_ ;

    std::string error_string_ ;
    OSM::BisonParser::location_type loc_ ;
 } ;


class LayerDefinition {
public:
    LayerDefinition(const std::string layer_name, const std::string &layer_type):
    name_(layer_name), type_(layer_type) {}

    std::string name_ ;
    std::string type_ ;
    std::string srid_ = "3857";

    Rule *rules_ = nullptr;
    LayerDefinition *next_ = nullptr;
};

class Context {

public:

    enum FeatureType { Way, Node, Relation } ;

    Context() {}
    Context(const OSM::Node &node): tags_(node.tags_), id_(node.id_), type_(Node) {}
    Context(const OSM::Way &way): tags_(way.tags_), id_(way.id_), type_(Way) {}
    Context(const OSM::Relation &rel): tags_(rel.tags_), id_(rel.id_), type_(Relation) {}

    FeatureType type() const { return type_ ; }

    bool has_tag(const std::string &tag) const { return tags_.contains(tag); }
    std::string value(const std::string &key) const { return tags_.get(key); }
    std::string id() const { return id_ ; }

    Dictionary tags_ ;
    std::string id_ ;
    FeatureType type_ ;
};


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

class Rule {
public:

    Rule(ExpressionNodePtr exp, const CommandListPtr &cmds): condition_(exp), commands_(cmds) {}

    ExpressionNodePtr condition_ ;
    CommandListPtr commands_ ;
};

class RuleList {
public:
    std::deque<RulePtr> rules_ ;
};

class ZoomRange {
public:

    ZoomRange(uint8_t min_zoom, uint8_t max_zoom):
    min_zoom_(min_zoom), max_zoom_(max_zoom){}

    uint8_t min_zoom_, max_zoom_ ;
};

class ExpressionNode {

    public:

    ExpressionNode() {}

    virtual Literal eval(Context &ctx) { return false ; }

    ExpressionNode(ExpressionNodePtr child) { appendChild(child) ; }
    ExpressionNode(ExpressionNodePtr a1, ExpressionNodePtr a2) {
        appendChild(a1) ;
        appendChild(a2) ;
    }
    ExpressionNode(ExpressionNodePtr a1, ExpressionNodePtr a2, ExpressionNodePtr a3) {
        appendChild(a1) ;
        appendChild(a2) ;
        appendChild(a3) ;
    }

    virtual ~ExpressionNode() {
    }

    void appendChild(ExpressionNodePtr node) { children_.push_back(node) ; }
    void prependChild(ExpressionNodePtr node) { children_.push_front(node) ; }

    std::deque<ExpressionNodePtr> children_ ;
};

class Command {
public:
    enum Type { Set, Add, Write, Continue, Delete, WriteAll, Exclude, Conditional } ;

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

class TagDeclaration {
public:
    TagDeclaration(const std::string &tag, const ExpressionNodePtr &val): tag_(tag), val_(val) {}

    std::string tag_ ;
    ExpressionNodePtr val_ ;
};

class TagDeclarationList {
public:

    std::deque<TagDeclarationPtr> tags_ ;
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

class RuleCommand: public Command {
public:

    Type type() const { return Conditional ; }

    RuleCommand(RulePtr rule): rule_(rule) {}

    RulePtr rule_ ;
};

class ActionBlock {
public:
    ActionBlock() {}
    CommandList commands_ ;
};

class LiteralExpressionNode: public ExpressionNode {
public:
    enum Type { String, Number, Boolean } ;

    LiteralExpressionNode(const std::string &str): val_(str) {}
    LiteralExpressionNode(const double val): val_(val) {}
    LiteralExpressionNode(const bool val): val_(val) {}

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
    Function(const std::string &name): name_(name) {}
    Function(const std::string &name, ExpressionNodePtr args): name_(name), ExpressionNode(args) {}

    Literal eval(Context &ctx) ;

private:
    std::string name_ ;

};


class BinaryOperator: public ExpressionNode {
public:
    BinaryOperator(int op_, ExpressionNodePtr op1, ExpressionNodePtr op2): op(op_), ExpressionNode(op1, op2) {}

    Literal eval(Context &ctx) ;

private:
    int op ;

};


class BooleanOperator: public ExpressionNode {
public:
    enum Type { And, Or, Not } ;

    BooleanOperator(Type op_, ExpressionNodePtr op1, ExpressionNodePtr op2): op(op_), ExpressionNode(op1, op2) {}

    Literal eval(Context &ctx) ;
private:
    Type op ;

};

class UnaryPredicate: public ExpressionNode {
public:

    UnaryPredicate(ExpressionNodePtr exp): ExpressionNode(exp) {}

    Literal eval(Context &ctx) ;
};

class ComparisonPredicate: public ExpressionNode {
public:
    enum Type { Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } ;

    ComparisonPredicate(Type op, ExpressionNodePtr lhs, ExpressionNodePtr rhs): op_(op), ExpressionNode(lhs, rhs) {}


    Literal eval(Context &ctx) ;

private:
    Type op_ ;
};

class LikeTextPredicate: public ExpressionNode {
public:

    LikeTextPredicate(ExpressionNodePtr op, const std::string &pattern_, bool is_pos) ;

    Literal eval(Context &ctx) ;
private:
    std::regex pattern_ ;
    bool is_pos_ ;

};

class ListPredicate: public ExpressionNode {
public:

    ListPredicate(const std::string &identifier, ExpressionNodePtr op, bool is_pos) ;


    Literal eval(Context &ctx) ;

private:
    std::string id_ ;
    std::vector<std::string> lvals_ ;
    bool is_pos_ ;

};

class IsTypePredicate: public ExpressionNode {
public:

    IsTypePredicate(const std::string &keyword):  keyword_(keyword) {}

    Literal eval(Context &ctx) ;

private:

    std::string keyword_ ;

};

class ExistsPredicate: public ExpressionNode {
public:

    ExistsPredicate(const std::string &tag):  tag_(tag) {}

    Literal eval(Context &ctx) ;

private:

    std::string tag_ ;

};


} // namespace Filter

} // namespace OSM

#endif

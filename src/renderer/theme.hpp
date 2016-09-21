#ifndef __MAPSFORGE_THEME_HPP__
#define __MAPSFORGE_THEME_HPP__

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "pugixml.hpp"

namespace mapsforge {

struct Rule ;
typedef std::shared_ptr<Rule> RulePtr ;

struct RenderInstruction ;
typedef std::shared_ptr<RenderInstruction> RenderInstructionPtr ;

struct Layer ;
typedef std::shared_ptr<Layer> LayerPtr ;

// mapsforge render theme v4

class RenderTheme {
public:

    RenderTheme() ;

    bool read(const std::string &file_name) ;

private:
    std::map<std::string, LayerPtr> layers_ ;
    std::vector<RulePtr> rules_ ;
    std::string default_layer_ ;
    std::string default_lang_ ;

private:
    LayerPtr get_safe_layer(const std::string &id) const ;
    RulePtr parse_rule(pugi::xml_node &node) ;

};

struct Layer {
    std::string id_ ;
    std::map<std::string, std::string> names_ ;
    std::vector<std::string> categories_ ;
    std::vector<LayerPtr> overlays_ ;

    std::vector<LayerPtr> children_ ;
    LayerPtr parent_ ;
    bool visible_, enabled_ ;
};

struct RenderInstruction {
    enum Type { AreaType, LineType, CaptionType, CircleType, LineSymbolType, PathTextType, SymbolType } ;

    virtual Type type() const = 0 ;
};

struct AreaInstruction: public RenderInstruction {
    virtual Type type() const { return AreaType ; }
};

struct LineInstruction: public RenderInstruction {
    virtual Type type() const { return LineType ; }
};

struct CaptionInstruction: public RenderInstruction {
    virtual Type type() const { return CaptionType ; }
};

struct SymbolInstruction: public RenderInstruction {
    virtual Type type() const { return SymbolType ; }
};

struct CircleInstruction: public RenderInstruction {
    virtual Type type() const { return CircleType ; }
};

struct LineSymbolInstruction: public RenderInstruction {
    virtual Type type() const { return LineSymbolType ; }
};

struct PathTextInstruction: public RenderInstruction {
    virtual Type type() const { return PathTextType ; }
};


struct Rule {
    enum ElementType { WayType, NodeType, AnyType } ;
    enum ClosedType { YesClosed, NoClosed, AnyClosed } ;

    std::string k_ ;
    std::vector<std::string> v_ ; // list of possible values

    ElementType e_ ;
    ClosedType closed_ ;
    uint8_t zoom_min_, zoom_max_ ; // 0  means undefined

    std::vector<RulePtr> children_ ; // sub-rules
    RulePtr parent_ ;
    std::vector<RenderInstructionPtr> instructions_ ;
};


}

#endif

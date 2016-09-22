#ifndef __MAPSFORGE_THEME_HPP__
#define __MAPSFORGE_THEME_HPP__

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "pugixml.hpp"
#include "dictionary.hpp"

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

    // load theme from file

    bool read(const std::string &file_name) ;

    // match feature against theme to determine render instructions

    void match(const std::string &layer, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way,
               std::vector<RenderInstructionPtr> &ris) const ;

    std::string defaultLayer() const { return default_layer_ ; }
    uint32_t backgroundColor() const { return map_bg_ ; }

private:
    std::map<std::string, LayerPtr> layers_ ;
    std::vector<RulePtr> rules_ ;
    std::string default_layer_ ;
    std::string default_lang_ ;
    std::string version_ ;
    uint32_t map_bg_, map_bg_outside_ ;

private:
    LayerPtr get_safe_layer(const std::string &id) const ;
    RulePtr parse_rule(pugi::xml_node &node) ;
    void get_categories(const LayerPtr &layer, std::set<std::string> &) const ;
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

struct Rule {
    enum ElementType { Node, Way, Any } ;
    enum ClosedType { Yes, No, Both } ;

    std::string k_, cat_ ;
    std::vector<std::string> v_ ; // list of possible values

    uint e_, closed_ ;

    uint8_t zoom_min_, zoom_max_ ; // 0  means undefined

    std::vector<RulePtr> children_ ; // sub-rules
    RulePtr parent_ ;
    std::vector<RenderInstructionPtr> instructions_ ;

    void match(const std::set<std::string> &categories, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way,
               std::vector<RenderInstructionPtr> &ris) const ;
};

struct RenderInstruction {
    enum Type { Area, Line, Caption, Circle, LineSymbol, PathText, Symbol } ;
    enum Scale { All, None, Stroke } ;
    enum Display { Allways, Never, IfSpace } ;
    enum LineCap { Butt, RoundCap, Square } ;
    enum LineJoin { Miter, RoundJoin, Bevel } ;
    enum FontFamily { Default, Monospace, SansSerif, Serif } ;
    enum FontStyle { Bold, BoldItalic, Italic, Normal } ;
    enum Position {  Auto, Center, Below, BelowLeft, BelowRight, Above, AboveLeft, AboveRight, Left, Right } ;

    virtual Type type() const = 0 ;
    virtual bool parse(pugi::xml_node &node) = 0 ;

    std::string cat_ ;
};

struct AreaInstruction: public RenderInstruction {
    virtual Type type() const { return Area ; }
    virtual bool parse(pugi::xml_node &node)  ;

    std::string src_ ;
    uint symbol_width_, symbol_height_, symbol_percent_ ;
    uint32_t fill_, stroke_ ;
    float stroke_width_ ;
    uint scale_ ;
};

struct LineInstruction: public RenderInstruction {
    virtual Type type() const { return Line ; }
    virtual bool parse(pugi::xml_node &node)  ;

    std::string src_ ;
    uint symbol_width_, symbol_height_, symbol_percent_ ;
    float dy_ ;
    uint scale_, stroke_line_cap_, stroke_line_join_ ;
    uint32_t stroke_ ;
    float stroke_width_ ;
    std::vector<float> stroke_dash_array_ ;
};

struct CaptionInstruction: public RenderInstruction {
    virtual Type type() const { return Caption ; }
    virtual bool parse(pugi::xml_node &node)  ;

    int priority_ ;
    std::string key_, symbol_id_ ;
    uint32_t fill_, stroke_ ;
    float font_size_, stroke_width_, dy_ ;
    uint font_family_, position_, display_, font_style_ ;
};

struct SymbolInstruction: public RenderInstruction {
    virtual Type type() const { return Symbol ; }
    virtual bool parse(pugi::xml_node &node)  ;

    std::string id_, src_ ;
    int priority_ ;
    uint symbol_width_, symbol_height_, symbol_percent_ ;
    uint display_ ;
};


struct CircleInstruction: public RenderInstruction {
    virtual Type type() const { return Circle ; }
    virtual bool parse(pugi::xml_node &node)  ;

    uint32_t fill_, stroke_ ;
    float radius_, stroke_width_ ;
    bool scale_radius_ ;
};

struct LineSymbolInstruction: public RenderInstruction {
    virtual Type type() const { return LineSymbol ; }
    virtual bool parse(pugi::xml_node &node)  ;

    std::string src_  ;
    uint scale_, display_ ;
    float dy_, repeat_gap_, repeat_start_ ;
    bool align_center_, rotate_, repeat_ ;
    uint symbol_width_, symbol_height_, symbol_percent_ ;
    int priority_ ;

};

struct PathTextInstruction: public RenderInstruction {
    virtual Type type() const { return PathText ; }
    virtual bool parse(pugi::xml_node &node)  ;

    int priority_ ;
    uint display_, font_style_, scale_, font_family_ ;
    std::string key_  ;
    bool repeat_, rotate_ ;
    float dy_, repeat_gap_, repeat_start_, font_size_, stroke_width_ ;
    uint32_t fill_, stroke_ ;
};





}

#endif

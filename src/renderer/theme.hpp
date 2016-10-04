#ifndef __MAPSFORGE_THEME_HPP__
#define __MAPSFORGE_THEME_HPP__

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "pugixml.hpp"
#include "dictionary.hpp"


struct Rule ;
typedef std::shared_ptr<Rule> RulePtr ;

struct RenderInstruction ;
typedef std::shared_ptr<RenderInstruction> RenderInstructionPtr ;

struct SymbolInstruction ;

struct Layer ;
typedef std::shared_ptr<Layer> LayerPtr ;

struct ThemeParseContext {
    std::map<std::string,  RenderInstruction *> imap_ ;
    std::string root_dir_ ;
    uint32_t order_ ;
};
// mapsforge render theme v4

class RenderTheme {
public:

    RenderTheme() ;

    // load theme from file

    bool read(const std::string &file_name, const std::string &resource_dir = std::string()) ;

    // match feature against theme to determine render instructions

    bool match(const std::string &layer, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way,
               std::vector<RenderInstructionPtr> &ris) const ;

    std::string defaultLayer() const { return default_layer_ ; }
    uint32_t backgroundColor() const { return map_bg_ ; }

private:


    friend class RenderInstruction ;

    std::map<std::string, LayerPtr> layers_ ;
    std::vector<RulePtr> rules_ ;
    std::string default_layer_ ;
    std::string default_lang_ ;
    std::string version_ ;
    uint32_t map_bg_, map_bg_outside_ ;
    std::string resource_dir_ ;

private:
    LayerPtr get_safe_layer(const std::string &id) const ;
    RulePtr parse_rule(pugi::xml_node &node, ThemeParseContext &) ;
    void get_categories(const LayerPtr &layer, std::set<std::string> &) const ;
};

struct Layer {

    Layer(): z_order_(0), visible_(true), enabled_(true) {}

    std::string id_ ;
    std::map<std::string, std::string> names_ ;
    std::vector<std::string> categories_ ;
    std::vector<LayerPtr> overlays_ ;

    std::vector<LayerPtr> children_ ;
    LayerPtr parent_ ;
    bool visible_, enabled_ ;
    uint8_t z_order_ ;
};

struct Rule {
    enum ElementType { Node, Way, Any } ;
    enum ClosedType { Yes, No, Both } ;

    std::string cat_ ;
    std::vector<std::string> k_, v_ ; // list of possible keys/values

    uint e_, closed_ ;

    uint8_t zoom_min_, zoom_max_ ; // 0  means undefined


    std::vector<RulePtr> children_ ; // sub-rules
    RulePtr parent_ ;
    std::vector<RenderInstructionPtr> instructions_ ;

    bool match(const std::set<std::string> &categories, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way,
               std::vector<RenderInstructionPtr> &ris) const ;
};

struct RenderInstruction {

    enum Type { Area, Line, Caption, Circle, LineSymbol, PathText, Symbol } ;
    enum Scale { All, None, Stroke } ;
    enum SymbolScaling { DefaultSize, CustomSize, Percent } ;
    enum Display { Allways, Never, IfSpace } ;
    enum LineCap { Butt, RoundCap, Square } ;
    enum LineJoin { Miter, RoundJoin, Bevel } ;
    enum FontFamily { Default, Monospace, SansSerif, Serif } ;
    enum FontStyle { Bold, BoldItalic, Italic, Normal } ;
    enum Position {  Auto, Center, Below, BelowLeft, BelowRight, Above, AboveLeft, AboveRight, Left, Right } ;

    Type type() { return type_ ; }

    RenderInstruction(): z_order_(0), priority_(0) {}

    std::string cat_, key_, id_ ;
    uint32_t z_order_ ; // order with which instructions were encountered in theme file determining the order of drawing
    int32_t priority_ ;
    Type type_ ;

    std::string src_ ;
    uint symbol_width_, symbol_height_, symbol_percent_ ;
    uint symbol_scaling_ ;
    uint32_t fill_, stroke_ ;
    float stroke_width_ ;
    float dy_ ;
    uint scale_, display_, stroke_line_cap_, stroke_line_join_ ;
    std::vector<float> stroke_dash_array_ ;
    std::string symbol_id_ ;
    float font_size_ ;
    uint font_family_, position_, font_style_ ;
    RenderInstruction *symbol_ ;
    float repeat_gap_, repeat_start_, radius_ ;
    bool align_center_, rotate_, repeat_, scale_radius_ ;

private:

    friend class RenderTheme ;

    bool parse_line(pugi::xml_node &node, ThemeParseContext &ctx)  ;
    bool parse_area(pugi::xml_node &node, ThemeParseContext &ctx)  ;
    bool parse_symbol(pugi::xml_node &node, ThemeParseContext &ctx)  ;
    bool parse_caption(pugi::xml_node &node, ThemeParseContext &ctx)  ;
    bool parse_circle(pugi::xml_node &node, ThemeParseContext &ctx)  ;
    bool parse_line_symbol(pugi::xml_node &node, ThemeParseContext &ctx)  ;
    bool parse_path_text(pugi::xml_node &node, ThemeParseContext &ctx)  ;

};


#endif

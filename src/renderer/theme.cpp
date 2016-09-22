#include "theme.hpp"

#include <cstring>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std ;

extern uint32_t css_color(const string &name, const string def) ;

namespace mapsforge {

RenderTheme::RenderTheme() {}

LayerPtr RenderTheme::get_safe_layer(const string &id) const {
    auto it = layers_.find(id) ;
    if ( it == layers_.end() ) return nullptr ;
    else return it->second ;
}

bool RenderTheme::read(const std::string &file_name)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());

    if ( !result ) {
        //        LOG_WARN_STREAM("XML [" << cfg_file << "] parsed with errors: " << result.description() ) ;
        return false ;
    }

    pugi::xml_node root = doc.first_child() ;
    if ( strcmp(root.name(), "rendertheme") != 0 ) return false ;

    version_ = root.attribute("version").as_string() ;
    map_bg_ = css_color(root.attribute("map-background").as_string(), "#ffffff") ;
    map_bg_outside_ = css_color(root.attribute("map-background-outside").as_string(), "#ffffff") ;

    pugi::xml_node style_menu = root.child("stylemenu") ;

    if ( style_menu ) {
        default_layer_ = style_menu.attribute("defaultvalue").as_string("") ;
        default_lang_ = style_menu.attribute("defaultlang").as_string("en") ;
    }

    map<string, string> layer_parents ;
    map<string, vector<string>> layer_overlays ;

    for( pugi::xml_node p: style_menu.children("layer") ) {

        LayerPtr layer(new Layer) ;

        string id = p.attribute("id").as_string() ;
        if ( id.empty() ) return false ;

        layer->id_ = id ;
        layer->enabled_ = p.attribute("enabled").as_bool(true) ;
        layer->visible_ = p.attribute("visible").as_bool(true) ;

        string parent = p.attribute("parent").as_string() ;
        if ( !parent.empty() ) layer_parents[id] = parent ;

        for( pugi::xml_node n: p.children("name") ) {

            string lang = n.attribute("lang").as_string("en") ;
            string val = n.attribute("value").as_string() ;

            layer->names_[lang] = val ;
        }

        for( pugi::xml_node n: p.children("cat") ) {
            string cat_id = n.attribute("id").as_string() ;

            if ( !cat_id.empty() )
                layer->categories_.push_back(cat_id) ;
            else
                return false ;
        }

        for( pugi::xml_node n: p.children("overlay") ) {
            string ovr_id = n.attribute("id").as_string() ;

            if ( !ovr_id.empty() )
                layer_overlays[id].push_back(ovr_id) ;
            else
                return false ;

        }

        layers_[id] = layer ;
    }

    // populate parent/child relationships

    for( auto lp: layer_parents ) {

        LayerPtr layer = get_safe_layer(lp.first) ;
        LayerPtr parent = get_safe_layer(lp.second) ;

        layer->parent_ = parent ;
        if ( layer ) parent->children_.push_back(layer) ;
    }

    // resolve overlays

    for( auto op: layer_overlays ) {

        LayerPtr layer = get_safe_layer(op.first) ;
        if ( !layer ) continue ;

        for( auto &ovr: op.second ) {
            LayerPtr overlay = get_safe_layer(ovr) ;
            if ( overlay ) layer->overlays_.push_back(overlay) ;
        }
    }

    for( pugi::xml_node p: root.children("rule") ) {
        RulePtr r = parse_rule(p) ;
        if ( !r ) return false ;
        rules_.push_back(r) ;

    }
}

void RenderTheme::match(const string &layer_id, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way, std::vector<RenderInstructionPtr> &ris) const
{
    set<string> categories ;

    LayerPtr layer = get_safe_layer(layer_id) ;
    get_categories(layer, categories) ;

    for( const RulePtr &r: rules_ ) {
        r->match(categories, tags, zoom, is_closed, is_way, ris)  ;
    }
}

static uint parse_enum(const string &name, const std::initializer_list<string> &list) {
    uint count = 0;
    for( auto q: list ) {
        if ( name == q ) return count ;
        ++count ;
    }
}

static uint parse_scale(const string &name) {
    return parse_enum(name, {"all", "none", "stroke"}) ;
}

static uint parse_display(const string &name) {
    return parse_enum(name, {"always", "never", "ifspace"}) ;
}

static uint parse_element(const string &name) {
    return parse_enum(name, {"node", "way", "any"}) ;
}

static uint parse_closed(const string &name) {
    return parse_enum(name, { "yes", "no", "any" }) ;
}

static uint parse_line_cap(const string &name) {
    return parse_enum(name, { "butt", "round", "square" }) ;
}

static uint parse_line_join(const string &name) {
    return parse_enum(name, { "miter", "round", "bevel" }) ;
}

static uint parse_font_family(const string &name) {
    return parse_enum(name, { "default", "monospace", "sans_serif", "serif" }) ;
}

static uint parse_font_style(const string &name) {
    return parse_enum(name, { "bold", "bold_italic", "italic", "normal" }) ;
}

static uint parse_position(const string &name) {
    return parse_enum(name, { "auto", "center", "below", "below_left", "below_right", "above", "above_left", "above_right", "left", "right" }) ;
}


static vector<float> parse_dash_array(const string &src) {

    vector<float> res ;

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",");
    tokenizer tokens(src, sep);

    for (tokenizer::iterator tok_iter = tokens.begin();
         tok_iter != tokens.end(); ++tok_iter) {

        try {
            res.push_back(boost::lexical_cast<float>(*tok_iter));
        }
        catch (boost::bad_lexical_cast &) {
            return vector<float>() ;
        }
    }

}

RulePtr RenderTheme::parse_rule(pugi::xml_node &p)
{
    RulePtr rule(new Rule) ;

    rule->e_ = parse_element(p.attribute("e").as_string("any")) ;
    rule->closed_ = parse_closed(p.attribute("closed").as_string("any") ) ;
    rule->k_ = p.attribute("k").as_string() ;
    rule->cat_ = p.attribute("cat").as_string() ;

    string v = p.attribute("v").as_string() ;

    rule->zoom_max_ = p.attribute("zoom-max").as_uint(255) ;
    rule->zoom_min_ = p.attribute("zoom-min").as_uint(0) ;

    if ( rule->k_.empty() || v.empty() ) return nullptr ;

    using boost::tokenizer;
    using boost::escaped_list_separator;

    boost::char_separator<char> sep("|");
    typedef tokenizer<boost::char_separator<char> > stokenizer;

    stokenizer tok(v, sep);

    for( stokenizer::iterator beg = tok.begin(); beg!=tok.end(); ++beg)
        if ( !beg->empty() ) rule->v_.push_back(*beg) ;

    for( pugi::xml_node n: p.children("rule") ) {
        RulePtr child = parse_rule(n) ;
        if ( child ) {
            rule->children_.push_back(child) ;
            child->parent_ = rule ;
        }
    }

    for( pugi::xml_node n: p.children() ) {
        string name = n.name() ;

        RenderInstructionPtr ri ;

        if ( name == "area" )            ri.reset(new AreaInstruction) ;
        else if ( name == "line" )       ri.reset(new LineInstruction) ;
        else if ( name == "caption ")    ri.reset(new CaptionInstruction) ;
        else if ( name == "circle" )     ri.reset(new CircleInstruction) ;
        else if ( name == "symbol" )     ri.reset(new SymbolInstruction) ;
        else if ( name == "pathText" )   ri.reset(new PathTextInstruction) ;
        else if ( name == "lineSymbol" ) ri.reset(new LineSymbolInstruction) ;
        else continue ;

        if ( ri->parse(n) )
            rule->instructions_.push_back(ri) ;
        else
            return nullptr ;
    }

    return rule ;
}

void RenderTheme::get_categories(const LayerPtr &layer, std::set<string> &categories) const
{
    if ( !layer || !layer->enabled_ ) return ;

    for( auto &s: layer->categories_ )
        categories.insert(s) ;

    get_categories(layer->parent_, categories) ;

    for( const LayerPtr &ovr: layer->overlays_ ) {
        get_categories(ovr, categories) ;
    }
}

bool AreaInstruction::parse(pugi::xml_node &p) {

    cat_ =              p.attribute("cat").as_string() ;
    src_ =              p.attribute("src").as_string() ;
    symbol_width_ =     p.attribute("symbol-width").as_uint() ;
    symbol_height_ =    p.attribute("symbol-width").as_uint() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_uint() ;
    fill_ =             css_color(p.attribute("fill").as_string(),"#000000") ;
    stroke_ =           css_color(p.attribute("stroke").as_string(),"#000000") ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0) ;

    return true ;
}

bool LineInstruction::parse(pugi::xml_node &p) {

    cat_ =              p.attribute("cat").as_string() ;
    src_ =              p.attribute("src").as_string() ;
    symbol_width_ =     p.attribute("symbol-width").as_uint() ;
    symbol_height_ =    p.attribute("symbol-width").as_uint() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_uint() ;
    dy_ =               p.attribute("dy").as_float(0) ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    stroke_ =           css_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0) ;

    stroke_dash_array_ = parse_dash_array(p.attribute("stroke-dash-array").as_string()) ;
    stroke_line_cap_ =   parse_line_cap(p.attribute("stroke-line-cap").as_string("round")) ;
    stroke_line_join_ =  parse_line_join(p.attribute("stroke-line-join").as_string("round")) ;

    return true ;
}

bool LineSymbolInstruction::parse(pugi::xml_node &p) {

    cat_ =              p.attribute("cat").as_string() ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    dy_ =               p.attribute("dy").as_float(0) ;
    src_ =              p.attribute("src").as_string() ;
    symbol_width_ =     p.attribute("symbol-width").as_uint() ;
    symbol_height_ =    p.attribute("symbol-width").as_uint() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_uint() ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    align_center_ =     p.attribute("align-center").as_bool(false) ;
    priority_ =         p.attribute("priority").as_int(0) ;
    repeat_ =           p.attribute("repeat").as_bool(false) ;
    repeat_gap_ =       p.attribute("repeat-gap").as_float(200.0) ;
    repeat_start_ =     p.attribute("repeat_start").as_float(30.0) ;
    rotate_ =           p.attribute("rotate").as_bool("true") ;

    return true ;
}


bool CaptionInstruction::parse(pugi::xml_node &p) {

    cat_ =              p.attribute("cat").as_string() ;
    priority_ =         p.attribute("priority").as_int(0) ;
    key_ =              p.attribute("k").as_string() ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    dy_ =               p.attribute("dy").as_float(0) ;
    font_family_ =      parse_font_family(p.attribute("font-family").as_string("default")) ;
    font_style_ =       parse_font_style(p.attribute("font-style").as_string("normal")) ;
    font_size_ =        p.attribute("font-size").as_float(0.0) ;
    fill_ =             css_color(p.attribute("fill").as_string(),"#000000") ;
    stroke_ =           css_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0.0) ;
    position_ =         parse_position(p.attribute("position").as_string("auto")) ;
    symbol_id_ =        p.attribute("symbol-id").as_string() ;

    if ( key_.empty() ) return false ;

    return true ;
}


bool CircleInstruction::parse(pugi::xml_node &p) {

    cat_ =              p.attribute("cat").as_string() ;
    radius_ =           p.attribute("radius").as_float(-1.0) ;
    scale_radius_ =     p.attribute("scale-radius").as_bool(false) ;
    fill_ =             css_color(p.attribute("fill").as_string(), "#000000") ;
    stroke_ =           css_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0.0) ;

    if ( radius_ < 0 ) return false ;

    return true ;
}

bool SymbolInstruction::parse(pugi::xml_node &p) {

    cat_ =              p.attribute("cat").as_string() ;
    id_ =               p.attribute("id").as_string() ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    priority_ =         p.attribute("priority").as_int(0) ;
    src_ =              p.attribute("src").as_string() ;
    symbol_width_ =     p.attribute("symbol-width").as_uint() ;
    symbol_height_ =    p.attribute("symbol-width").as_uint() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_uint() ;

    if ( src_.empty() ) return false ;
    return true ;
}

bool PathTextInstruction::parse(pugi::xml_node &p)
{
    cat_ =              p.attribute("cat").as_string() ;
    priority_ =         p.attribute("priority").as_int(0) ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    key_ =              p.attribute("k").as_string() ;
    dy_ =               p.attribute("dy").as_float(0) ;
    font_family_ =      parse_font_family(p.attribute("font-family").as_string("default")) ;
    font_style_ =       parse_font_style(p.attribute("font-style").as_string("normal")) ;
    font_size_ =        p.attribute("font-size").as_float(0.0) ;
    fill_ =             css_color(p.attribute("fill").as_string(), "#000000") ;
    stroke_ =           css_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0.0) ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    repeat_ =           p.attribute("repeat").as_bool(true) ;
    repeat_gap_ =       p.attribute("repeat-gap").as_float(50.0) ;
    repeat_start_ =     p.attribute("repeat_start").as_float(10.0) ;
    rotate_ =           p.attribute("rotate").as_bool("true") ;

    if ( key_.empty() ) return false ;
    return true ;
}

void Rule::match(const std::set<string> &categories, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way,
                 std::vector<RenderInstructionPtr> &ris) const
{
    if ( !cat_.empty() && categories.count(cat_) == 0 ) return ;
    if ( zoom < zoom_min_ || zoom > zoom_max_ ) return  ;
    if ( ( closed_ == Rule::Yes && !is_closed ) || ( closed_ == Rule::No && is_closed ) ) return ;
    if ( ( e_ == Rule::Way && !is_way ) || ( e_ == Rule::Node && is_way ) ) return  ;

    string val ;
    if ( !tags.contains(k_) ) val = "~" ;
    else val = tags.get(k_) ;

    bool kv_match = false ;
    for( auto &v: v_ ) {
        if ( v == "*" || v == val ) {
            kv_match = true ;
            break ;
        }
    }

    if ( !kv_match ) return  ;

    // we have done all tests, now add the instructions for this rule

    for( auto &ip: instructions_ ) {
        if ( ip->cat_.empty() || categories.count(ip->cat_) )
            ris.push_back(ip) ;
    }

    // match all subrules

    for( auto &child: children_ ) {
        child->match(categories, tags, zoom, is_closed, is_way, ris) ;
    }
}

}

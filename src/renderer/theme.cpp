#include "theme.hpp"

#include <cstring>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <clocale>

using namespace std ;

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

static uint parse_symbol_scale(const string &name) {
    return parse_enum(name, { "default", "size", "percent" }) ;
}

#define PACK_ARGB(a,r,g,b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))


static void get_argb_color(uint32_t clr, uint &a, uint &r, uint &g, uint &b) {
    a = ((clr >> 24) & 0xff) ;
    r = ((clr >> 16) & 0xff);
    g = ((clr >> 8) & 0xff) ;
    b = ((clr >> 0) & 0xff) ;
}

static uint32_t parse_color(const string &str, const string &def="") {
    static boost::regex clr_regex("#(?:([0-9a-f])([0-9a-f]))?([0-9a-f])([0-9a-f])([0-9a-f])([0-9a-f])([0-9a-f])([0-9a-f])", boost::regex::icase) ;

    boost::smatch what;

    unsigned int val = 0 ;

    if ( boost::regex_match(str, what, clr_regex) )
    {
        unsigned int hex, i ;

        for( i=1 ; i<what.size() ; i++ )
        {
            string s = what[i] ;


            if ( s.empty() ) hex = 0xf ;
            else {
                char c = s.at(0) ;

                if ( c >= '0' && c <= '9' ) hex = c - '0';
                else if ( c >= 'A' && c <= 'F' ) hex = c - 'A' + 10 ;
                else if ( c >= 'a' && c <= 'f' ) hex = c - 'a' + 10 ;
            }


            val = (val << 4) + hex;
        }
        return val ;
    }
    else return parse_color(def) ;
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

    return res ;
}

static void parse_option_list(const string &src, vector<string> &tokens) {
    using boost::tokenizer;
    using boost::escaped_list_separator;

    boost::char_separator<char> sep("|");
    typedef tokenizer<boost::char_separator<char> > stokenizer;

    stokenizer tok(src, sep);

    for( stokenizer::iterator beg = tok.begin(); beg!=tok.end(); ++beg)
        if ( !beg->empty() ) tokens.push_back(*beg) ;
}

static string parse_url(const string &url, const string &root_dir) {
    if ( boost::starts_with(url, "file:") ) {
        return "file:" + (boost::filesystem::path(root_dir) / url.substr(5)).native() ;
    }
    else return url ;
}


RenderTheme::RenderTheme() {}

LayerPtr RenderTheme::get_safe_layer(const string &id) const {
    auto it = layers_.find(id) ;
    if ( it == layers_.end() ) return nullptr ;
    else return it->second ;
}

bool RenderTheme::read(const std::string &file_name, const string &resource_dir)
{

    std::setlocale(LC_ALL, "C");

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());

    if ( !result ) {
        //        LOG_WARN_STREAM("XML [" << cfg_file << "] parsed with errors: " << result.description() ) ;
        return false ;
    }

    if ( resource_dir.empty() )
        resource_dir_ = boost::filesystem::path(file_name).parent_path().string() ;
    else
        resource_dir_ = resource_dir ;

    ThemeParseContext ctx ;
    ctx.root_dir_ = resource_dir_ ;
    ctx.order_ = 0 ;

    pugi::xml_node root = doc.first_child() ;
    if ( strcmp(root.name(), "rendertheme") != 0 ) return false ;

    version_ = root.attribute("version").as_string() ;
    map_bg_ = parse_color(root.attribute("map-background").as_string(), "#ffffff") ;
    map_bg_outside_ = parse_color(root.attribute("map-background-outside").as_string(), "#ffffff") ;

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
        layer->visible_ = p.attribute("visible").as_bool(false) ;

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

    uint32_t order = 0 ;
    for( pugi::xml_node p: root.children("rule") ) {
        RulePtr r = parse_rule(p, ctx) ;
        if ( !r ) return false ;
        rules_.push_back(r) ;

    }

    return true ;
}

static string make_match_key(const Dictionary &dict, const std::string &layer_id, uint8_t zoom, bool is_closed, bool is_way) {

    stringstream strm ;

    strm << layer_id << ';' ;

    DictionaryIterator it(dict) ;

    for( ;  it ; ++it  ) {
        string key = it.key() ;
        string value = it.value() ;

        if ( key == "name" ) continue ;
        if ( key == "addr:housenumber" ) continue ;
        if ( key == "ref" ) continue ;
        if ( key == "ele" ) continue ;

        strm << key << '=' << value << ';' ;
    }

    strm << (int)zoom << ';' << is_closed << ';' << is_way ;
    return strm.str() ;
}

bool RenderTheme::match(const string &layer_id, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way, std::vector<RenderInstructionPtr> &ris)
{
    set<string> categories ;

    LayerPtr layer = get_safe_layer(layer_id) ;
    get_categories(layer, categories) ;

 //   if ( !layer ) return false ;

    string key = make_match_key(tags, layer_id, zoom, is_closed, is_way) ;

    // TODO: make this thread safe

    auto it = rule_match_cache_.find(key) ;
    if ( it != rule_match_cache_.end() )
        ris = it->second ;
    else
    {
        for( const RulePtr &r: rules_ ) {
            if ( r->match(categories, tags, zoom, is_closed, is_way, ris) ) {
                rule_match_cache_.insert(std::make_pair(key, ris)) ;
                return true ;
            }
        }
    }

    return ( !ris.empty() ) ;
}

void RenderTheme::getVisibleLayers(std::vector<string> &ids) const
{
    for( const auto &lp: layers_ ) {
        LayerPtr layer = lp.second ;
        if ( !layer->visible_ ) continue ;
        ids.push_back(lp.first) ;
    }
}

void RenderTheme::getOverlays(const string &layer_id, std::vector<string> &ids) const
{
    LayerPtr layer = get_safe_layer(layer_id) ;
    if ( !layer ) return ;
    for( const auto &l: layer->overlays_ ) {
        ids.push_back(l->id_) ;
    }

}


RulePtr RenderTheme::parse_rule(pugi::xml_node &p, ThemeParseContext &ctx)
{
    RulePtr rule(new Rule) ;

    rule->e_ = parse_element(p.attribute("e").as_string("any")) ;
    rule->closed_ = parse_closed(p.attribute("closed").as_string("any") ) ;

    rule->cat_ = p.attribute("cat").as_string() ;

    string k = p.attribute("k").as_string() ;
    string v = p.attribute("v").as_string() ;

    rule->zoom_max_ = p.attribute("zoom-max").as_uint(255) ;
    rule->zoom_min_ = p.attribute("zoom-min").as_uint(0) ;

    if ( k.empty() || v.empty() ) return nullptr ;

    parse_option_list(k, rule->k_) ;
    parse_option_list(v, rule->v_) ;


    for( pugi::xml_node n: p.children() ) {
        string name = n.name() ;

        RenderInstructionPtr ri(new RenderInstruction) ;
        bool res ;

        if ( name == "area" )            res = ri->parse_area(n, ctx) ;
        else if ( name == "line" )       res = ri->parse_line(n, ctx) ;
        else if ( name == "caption" )    res = ri->parse_caption(n, ctx) ;
        else if ( name == "circle" )     res = ri->parse_circle(n, ctx) ;
        else if ( name == "symbol" )     res = ri->parse_symbol(n, ctx) ;
        else if ( name == "pathText" )   res = ri->parse_path_text(n, ctx) ;
        else if ( name == "lineSymbol" ) res = ri->parse_line_symbol(n, ctx) ;
        else continue ;

        if ( !res ) return nullptr ;

        ri->z_order_ = ctx.order_++ ;
        rule->instructions_.push_back(ri) ;
    }

    for( pugi::xml_node n: p.children("rule") ) {
        RulePtr child = parse_rule(n, ctx) ;
        if ( child ) {
            rule->children_.push_back(child) ;
            child->parent_ = rule ;
        }
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

bool RenderInstruction::parse_area(pugi::xml_node &p, ThemeParseContext &ctx) {

    cat_ =              p.attribute("cat").as_string() ;
    src_ =              parse_url(p.attribute("src").as_string(), ctx.root_dir_) ;
    symbol_width_ =     p.attribute("symbol-width").as_float() ;
    symbol_height_ =    p.attribute("symbol-width").as_float() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_float() ;
    symbol_scaling_ =   parse_symbol_scale(p.attribute("symbol-scaling").as_string("default")) ;
    fill_ =             parse_color(p.attribute("fill").as_string(),"#000000") ;
    stroke_ =           parse_color(p.attribute("stroke").as_string(),"#000000") ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0) ;
    type_ = Area ;

    return true ;
}

bool RenderInstruction::parse_line(pugi::xml_node &p, ThemeParseContext &ctx) {

    cat_ =              p.attribute("cat").as_string() ;
    src_ =              parse_url(p.attribute("src").as_string(), ctx.root_dir_) ;
    symbol_width_ =     p.attribute("symbol-width").as_float() ;
    symbol_height_ =    p.attribute("symbol-width").as_float() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_float() ;
    dy_ =               p.attribute("dy").as_float(0) ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    symbol_scaling_ =   parse_symbol_scale(p.attribute("symbol-scaling").as_string("default")) ;
    stroke_ =           parse_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0) ;

    stroke_dash_array_ = parse_dash_array(p.attribute("stroke-dasharray").as_string()) ;
    stroke_line_cap_ =   parse_line_cap(p.attribute("stroke-linecap").as_string("round")) ;
    stroke_line_join_ =  parse_line_join(p.attribute("stroke-linejoin").as_string("round")) ;
    type_ = Line ;

    return true ;
}

bool RenderInstruction::parse_line_symbol(pugi::xml_node &p, ThemeParseContext &ctx) {

    cat_ =              p.attribute("cat").as_string() ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    dy_ =               p.attribute("dy").as_float(0) ;
    src_ =              parse_url(p.attribute("src").as_string(), ctx.root_dir_) ;
    symbol_width_ =     p.attribute("symbol-width").as_uint() ;
    symbol_height_ =    p.attribute("symbol-width").as_uint() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_uint() ;
    symbol_scaling_ =   parse_symbol_scale(p.attribute("symbol-scaling").as_string("default")) ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    align_center_ =     p.attribute("align-center").as_bool(false) ;
    priority_ =         p.attribute("priority").as_int(0) ;
    repeat_ =           p.attribute("repeat").as_bool(false) ;
    repeat_gap_ =       p.attribute("repeat-gap").as_float(200.0) ;
    repeat_start_ =     p.attribute("repeat-start").as_float(30.0) ;
    rotate_ =           p.attribute("rotate").as_bool("true") ;
    type_ = LineSymbol;

    if ( src_.empty() ) return false ;

    return true ;
}


bool RenderInstruction::parse_caption(pugi::xml_node &p, ThemeParseContext &ctx) {

    cat_ =              p.attribute("cat").as_string() ;
    priority_ =         p.attribute("priority").as_int(0) ;
    key_ =              p.attribute("k").as_string() ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    dy_ =               p.attribute("dy").as_float(0) ;
    font_family_ =      parse_font_family(p.attribute("font-family").as_string("default")) ;
    font_style_ =       parse_font_style(p.attribute("font-style").as_string("normal")) ;
    font_size_ =        p.attribute("font-size").as_float(0.0) ;
    fill_ =             parse_color(p.attribute("fill").as_string(),"#000000") ;
    stroke_ =           parse_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0.0) ;
    position_ =         parse_position(p.attribute("position").as_string("auto")) ;
    symbol_id_ =        p.attribute("symbol-id").as_string() ;
    priority_ =         p.attribute("priority").as_int(0) ;
    type_ = Caption ;

    symbol_ = nullptr ;

    if ( !symbol_id_.empty() ) {
        auto it = ctx.imap_.find(symbol_id_) ;
        if ( it != ctx.imap_.end() )
            symbol_ = it->second ;
    }
    if ( key_.empty() ) return false ;

    return true ;
}


bool RenderInstruction::parse_circle(pugi::xml_node &p, ThemeParseContext &ctx) {

    cat_ =              p.attribute("cat").as_string() ;
    radius_ =           p.attribute("radius").as_float(-1.0) ;
    scale_radius_ =     p.attribute("scale-radius").as_bool(false) ;
    fill_ =             parse_color(p.attribute("fill").as_string(), "#000000") ;
    stroke_ =           parse_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0.0) ;
    priority_ =         p.attribute("priority").as_int(0) ;
    type_ = Circle ;

    if ( radius_ < 0 ) return false ;

    return true ;
}

bool RenderInstruction::parse_symbol(pugi::xml_node &p, ThemeParseContext &ctx) {

    cat_ =              p.attribute("cat").as_string() ;
    id_ =               p.attribute("id").as_string() ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    priority_ =         p.attribute("priority").as_int(0) ;
    src_ =              parse_url(p.attribute("src").as_string(), ctx.root_dir_) ;
    symbol_width_ =     p.attribute("symbol-width").as_uint() ;
    symbol_height_ =    p.attribute("symbol-width").as_uint() ;
    symbol_percent_ =   p.attribute("symbol-percent").as_uint() ;
    symbol_scaling_ =   parse_symbol_scale(p.attribute("symbol-scaling").as_string("default")) ;
    priority_ =         p.attribute("priority").as_int(0) ;
    type_ = Symbol ;

    if ( !id_.empty() ) ctx.imap_[id_] = this ;
    if ( src_.empty() ) return false ;
    return true ;
}

bool RenderInstruction::parse_path_text(pugi::xml_node &p, ThemeParseContext &ctx)
{
    cat_ =              p.attribute("cat").as_string() ;
    priority_ =         p.attribute("priority").as_int(0) ;
    display_ =          parse_display(p.attribute("display").as_string("ifspace")) ;
    key_ =              p.attribute("k").as_string() ;
    dy_ =               p.attribute("dy").as_float(0) ;
    font_family_ =      parse_font_family(p.attribute("font-family").as_string("default")) ;
    font_style_ =       parse_font_style(p.attribute("font-style").as_string("normal")) ;
    font_size_ =        p.attribute("font-size").as_float(0.0) ;
    fill_ =             parse_color(p.attribute("fill").as_string(), "#000000") ;
    stroke_ =           parse_color(p.attribute("stroke").as_string(), "#000000") ;
    stroke_width_ =     p.attribute("stroke-width").as_float(0.0) ;
    scale_ =            parse_scale(p.attribute("scale").as_string("stroke")) ;
    repeat_ =           p.attribute("repeat").as_bool(true) ;
    repeat_gap_ =       p.attribute("repeat-gap").as_float(50.0) ;
    repeat_start_ =     p.attribute("repeat_start").as_float(10.0) ;
    rotate_ =           p.attribute("rotate").as_bool("true") ;
    type_ = PathText ;

    if ( key_.empty() ) return false ;
    return true ;
}

bool Rule::match(const set<string> &categories, const Dictionary &tags, uint8_t zoom, bool is_closed, bool is_way,
                 std::vector<RenderInstructionPtr> &ris) const
{
    if ( !cat_.empty() && categories.count(cat_) == 0 ) return false;
    if ( zoom < zoom_min_ || zoom > zoom_max_ ) return false ;
    if ( ( closed_ == Rule::Yes && !is_closed ) || ( closed_ == Rule::No && is_closed ) ) return false;
    if ( ( e_ == Rule::Way && !is_way ) || ( e_ == Rule::Node && is_way ) ) return false ;

    bool kv_match = false ;

    for( const string &key: k_) {

        string val ;
        if ( key == "*" ) {
            kv_match = true ;
            break ;
        }
        if ( !tags.contains(key) ) val = "~" ;
        else val = tags.get(key) ;


        for( auto &v: v_ ) {
            if ( ( v == val ) || ( ( v == "*" ) && ( val != "~" ) ) ) {
                kv_match = true ;
                break ;
            }
        }

        if ( kv_match ) break ;
    }

    if ( !kv_match ) return false ;

    // we have done all tests, now add the instructions for this rule

    for( auto &ip: instructions_ ) {
        if ( ip->cat_.empty() || categories.count(ip->cat_) ) {
            ris.push_back(ip) ;
        }
    }

    // match all subrules

    for( auto &child: children_ ) {
        child->match(categories, tags, zoom, is_closed, is_way, ris)  ;
    }

    return !instructions_.empty() ;
}



#include "theme.hpp"


#include <cstring>

using namespace std ;
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

    string version = root.attribute("version").as_string() ;
    string map_bg = root.attribute("map-background").as_string("#ffffff") ;
    string map_bg_outside = root.attribute("map-background-outside").as_string("#ffffff") ;

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


RulePtr RenderTheme::parse_rule(pugi::xml_node &p)
{
    RulePtr rule(new Rule) ;

    string e = p.attribute("e").as_string("any") ;
    string k = p.attribute("k").as_string() ;
    string v = p.attribute("v").as_string() ;
    string closed = p.attribute("closed").as_string() ;
    uint8_t zoom_max = p.attribute("zoom-max").as_uint(255) ;
    uint8_t zoom_min = p.attribute("zoom-min").as_uint(0) ;

    if ( k.empty() || v.empty() ) return rule ;

    for( pugi::xml_node n: p.children("rule") ) {
        RulePtr child = parse_rule(n) ;
        if ( child ) {
            rules_.push_back(child) ;
            rule->children_.push_back(child) ;
            child->parent_ = rule ;
        }
    }

    pugi::xml_node inode = p.child("area") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<AreaInstruction>()) ;

    inode = p.child("line") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<LineInstruction>()) ;

    inode = p.child("caption") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<CaptionInstruction>()) ;

    inode = p.child("circle") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<CircleInstruction>()) ;

    inode = p.child("symbol") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<SymbolInstruction>()) ;

    inode = p.child("pathText") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<PathTextInstruction>()) ;

    inode = p.child("lineSymbol") ;
    if ( inode ) rule->instructions_.push_back(std::make_shared<LineSymbolInstruction>()) ;
}


}

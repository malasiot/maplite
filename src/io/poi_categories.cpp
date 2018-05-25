#include "poi_categories.hpp"

#include <stack>

using namespace std ;

POICategory::Collection POICategory::descendants() const {
    POICategory::Collection res ;
    stack<POICategory::Ptr> st ;
    POICategory::Ptr root ;
    st.push(root) ;

    while ( !st.empty()) {
        POICategory::Ptr e = st.top() ;
        st.pop() ;

        if ( e ) {
            res.push_back(e) ;

            for( const auto &c: e->children_ ) {
                st.push(c) ;
            }
        }
    }

    return res ;
}

bool POICategoryContainer::loadFromXML(const string &path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());

    if ( !result ) return false ;

    root_.reset(new POICategory()) ;
    return parseCategory(root_, doc.child("category")) ;
}

bool POICategoryContainer::loadFromXMLString(const string &str) {

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(str.c_str());

    if ( !result ) return false ;

    root_.reset(new POICategory()) ;
    return parseCategory(root_, doc.child("category")) ;

}

bool POICategoryContainer::loadFromPOIFile(SQLite::Connection &db)
{
    SQLite::Query q(db, "SELECT * FROM poi_categories") ;

    map<string, string> parent_ids ;

    for( const SQLite::Row &r: q.exec() ) {
        string id = r[0].as<string>() ;
        string name = r[1].as<string>() ;
        string parent_id = r[2].as<string>() ;

        if ( id.empty() ) return false ;

        POICategory::Ptr cat(new POICategory) ;
        cat->id_ = id ;
        cat->title_ = name ;

        categories_[id] = cat ;
        parent_ids[id] = parent_id ;
    }

    // rebuild hierarchy

    for( const auto &pp: parent_ids ) {
        POICategory::Ptr c = getByID(pp.first) ;

        if ( pp.second.empty() )
            root_ = c ;
        else {
            POICategory::Ptr parent = getByID(pp.second) ;
            if ( !parent ) return false ;
            c->parent_ = parent.get() ;
            parent->children_.emplace_back(c) ;
        }
    }

    return true ;
}

POICategory::Ptr POICategoryContainer::getByID(const string &id) const
{
    auto it = categories_.find(id) ;
    if ( it != categories_.end() ) return it->second ;
    else return nullptr ;
}



bool POICategoryContainer::parseCategory(POICategory::Ptr &c, const pugi::xml_node &root)
{
    if ( !root ) return false ;

    c->id_ = root.attribute("id").as_string() ;
    c->title_ = root.attribute("title").as_string() ;

    if ( c->id_.empty() ) return false ;

    categories_[c->id_] = c ;

    for( pugi::xml_node p: root.children("category") ) {
        POICategory::Ptr child(new POICategory()) ;
        if ( parseCategory(child, p) ) {
            child->parent_ = c.get() ;
            c->children_.emplace_back(child) ;
        }
        else return false ;
    }
/*
    for( pugi::xml_node p: root.children("name") ) {
        string lang = p.attribute("lang").as_string("default") ;
        c->lang_names_[lang] = p.text().as_string() ;
    }
*/
    return true ;
}

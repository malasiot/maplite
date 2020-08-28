#include "osm_xml_reader.hpp"
#include <util/xml_pull_parser.hpp>

using namespace std ;

namespace OSM {

enum class ParseContext { Node, Way, Relation } ;

bool XMLReader::read(istream &strm, Storage &doc)
{
    XmlPullParser parser(strm) ;

    try {
        ParseContext context ;
        Node node ;
        Way way ;
        Relation relation ;

        doc.writeBegin();

        while ( parser.next() != XmlPullParser::END_DOCUMENT ) {
            switch ( parser.getEventType() ) {
            case XmlPullParser::START_TAG: {
                string tag = parser.getName() ;
                if ( tag == "node" ) {
                    context = ParseContext::Node ;
                    node.tags_.clear() ;
                    node.id_ = stoll(parser.getAttribute("id")) ;
                    string action = parser.getAttribute("action") ;
                    node.delete_ = ( action == "delete" );
                    if ( !node.delete_ ) {
                        node.delete_ = false ;
                        node.lat_ = stof(parser.getAttribute("lat").c_str()) ;
                        node.lon_ = stof(parser.getAttribute("lon").c_str()) ;
                    }
                } else if ( tag == "way" ) {
                    context = ParseContext::Way ;
                    way.tags_.clear() ;
                    way.nodes_.clear() ;
                    way.id_ = stoll(parser.getAttribute("id")) ;
                    string action = parser.getAttribute("action") ;
                    way.delete_ = ( action == "delete" );
                } else if ( tag == "relation" ) {
                    context = ParseContext::Relation ;
                    relation.id_ = stoll(parser.getAttribute("id")) ;
                    relation.tags_.clear() ;
                    relation.children_.clear() ;
                    relation.children_role_.clear() ;
                    relation.nodes_.clear() ;
                    relation.nodes_role_.clear() ;
                    relation.ways_.clear() ;
                    relation.ways_role_.clear() ;
                    string action = parser.getAttribute("action") ;
                    way.delete_ = ( action == "delete" );
                } else if ( tag == "nd" ) {
                    osm_id_t ref = stoll(parser.getAttribute("ref"))  ;
                    way.nodes_.push_back(ref) ;
                } else if ( tag == "tag" ) {
                    string key = parser.getAttribute("k")  ;
                    string val = parser.getAttribute("v") ;

                    if ( context == ParseContext::Node )
                        node.tags_[key] = val ;
                    else if ( context == ParseContext::Way ) {
                        way.tags_[key] = val ;
                    } else {
                        relation.tags_[key] = val ;
                    }
                } else if ( tag == "member"  ) {
                    string type = parser.getAttribute("type") ;
                    osm_id_t ref = stoll(parser.getAttribute("ref")) ;
                    string role = parser.getAttribute("role") ;

                    if ( type.empty() ) return false ;

                    if ( type == "node" ) {
                        relation.nodes_.push_back(ref) ;
                        relation.nodes_role_.push_back(role) ;
                    }
                    else if ( type == "way" ) {
                        relation.ways_.push_back(ref) ;
                        relation.ways_role_.push_back(role) ;
                    }
                    else if ( type == "relation" ) {
                        relation.children_.push_back(ref) ;
                        relation.children_role_.push_back(role) ;
                    }
                }
                break ;
            }
            case XmlPullParser::END_TAG: {
                string tag = parser.getName() ;
                if ( tag == "node" && !node.delete_  )
                    doc.writeNode(node) ;
                else if ( tag == "way" && !node.delete_)
                    doc.writeWay(way) ;
                else if ( tag == "relation" && !node.delete_ )
                    doc.writeRelation(relation) ;
                break ;
            }
            }

        };

        doc.writeEnd() ;
    }
    catch ( XmlPullParserException &e ) {
        return false ;
    }
    catch ( std::invalid_argument &e ) {
        cout << e.what() << endl ;
        return false ;
    }
    catch ( std::out_of_range & ) {
        return false ;
    }



    return true ;
}

}

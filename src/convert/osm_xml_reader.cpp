#include "osm_document.hpp"
#include "osm_accessor.hpp"
#include "xml_reader.hpp"

using namespace std ;


namespace OSM {

bool DocumentReader::readXML(istream &strm, DocumentAccessor &a)
{
    XmlReader rd(strm) ;

    if ( !rd.readNextStartElement("osm") ) return false ;


    try {
        a.beginWrite() ;
        while ( rd.read() )
        {
            if ( rd.readNextStartElement() )
            {
                if ( rd.nodeName() == "node" )
                {
                    Node node ;

                    node.id_ = stoll(rd.attribute("id")) ;
                    node.lat_ = stof(rd.attribute("lat").c_str()) ;
                    node.lon_ = stof(rd.attribute("lon").c_str()) ;

                    while ( rd.read() )
                    {
                        if ( rd.isStartElement("tag") )
                        {
                            string key = rd.attribute("k")  ;
                            string val = rd.attribute("v") ;

                            node.tags_[key] = val ;
                        }
                        else if ( rd.isEndElement("node" ) ) break ;
                    }

                    a.writeNode(node);
                }
                else if ( rd.nodeName() == "way" )
                {
                    Way way ;

                    way.id_ = stoll(rd.attribute("id")) ;

                    while ( rd.read() )
                    {
                        if ( rd.isStartElement("nd") ) {
                            osm_id_t ref = stoll(rd.attribute("ref"))  ;
                            way.nodes_.push_back(ref) ;
                        }
                        else if ( rd.isStartElement("tag")) {
                            string key = rd.attribute("k")  ;
                            string val = rd.attribute("v") ;

                            way.tags_[key] = val ;
                        }
                        else if ( rd.isEndElement("way" ) ) break ;
                    }

                    a.writeWay(way) ;

                }
                else if ( rd.nodeName() == "relation" )
                {
                    Relation relation ;

                    relation.id_ = stoll(rd.attribute("id")) ;

                    while ( rd.read() )
                    {
                        if ( rd.isStartElement("member") )
                        {
                            string type = rd.attribute("type") ;
                            osm_id_t ref = stoll(rd.attribute("ref")) ;
                            string role = rd.attribute("role") ;

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
                        else if ( rd.isStartElement("tag"))
                        {
                            string key = rd.attribute("k") ;
                            string val = rd.attribute("v") ;

                            relation.tags_[key] = val ;
                        }
                        else if ( rd.isEndElement("relation" ) ) break ;

                    }

                    a.writeRelation(relation) ;
                }
            }

        }
        a.endWrite() ;
    }
    catch ( std::invalid_argument & ) {
        return false ;
    }
    catch ( std::out_of_range & ) {
        return false ;
    }



    return true ;
}

}

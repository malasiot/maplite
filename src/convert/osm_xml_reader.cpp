#include "osm_document.hpp"
#include "xml_reader.hpp"

using namespace std ;

namespace OSM {

bool DocumentReader::readXML(istream &strm, DocumentAccessor &a)
{
    XmlReader rd(strm) ;

    map<string, uint> nodeMap, wayMap, relMap ;
    vector<  vector<string> > wayNodeMap, relNodeMap, relWayMap, relRelMap ;
    vector<  vector<string> > relNodeMapRole, relWayMapRole, relRelMapRole ;
#if 0
    if ( !rd.readNextStartElement("osm") ) return false ;

    while ( rd.read() )
    {
        if ( rd.readNextStartElement() )
        {
            if ( rd.nodeName() == "node" )
            {
                Node node ;

                node.id_ = rd.attribute("id") ;

                if ( node.id_.empty() ) return false ;

                node.lat_ = atof(rd.attribute("lat").c_str()) ;
                node.lon_ = atof(rd.attribute("lon").c_str()) ;

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

                nodeMap[node.id_] = nodes_.size() ;
                nodes_.push_back(node) ;

            }
            else if ( rd.nodeName() == "way" )
            {
                Way way ;

                way.id_ = rd.attribute("id") ;

                wayNodeMap.push_back(vector<string>()) ;
                vector<string> &map_item =  wayNodeMap.back() ;

                if ( way.id_.empty() ) return false ;

                while ( rd.read() )
                {
                    if ( rd.isStartElement("nd") )
                    {
                        string ref_ = rd.attribute("ref")  ;

                        if ( ref_.empty()  ) return false ;

                        map_item.push_back(ref_) ;
                    }
                    else if ( rd.isStartElement("tag"))
                    {
                        string key = rd.attribute("k")  ;
                        string val = rd.attribute("v") ;

                        way.tags_[key] = val ;
                    }
                    else if ( rd.isEndElement("way" ) ) break ;
                }

                wayMap[way.id_] = ways_.size() ;
                ways_.push_back(way) ;

            }
            else if ( rd.nodeName() == "relation" )
            {
                Relation relation ;

                relation.id_ = rd.attribute("id") ;

                if (relation.id_.empty() ) return false ;

                relNodeMap.push_back(vector<string>()) ;
                relWayMap.push_back(vector<string>()) ;
                relRelMap.push_back(vector<string>()) ;

                relNodeMapRole.push_back(vector<string>()) ;
                relWayMapRole.push_back(vector<string>()) ;
                relRelMapRole.push_back(vector<string>()) ;

                vector<string> &node_map_item = relNodeMap.back() ;
                vector<string> &way_map_item = relWayMap.back() ;
                vector<string> &rel_map_item = relRelMap.back() ;

                vector<string> &node_map_role = relNodeMapRole.back() ;
                vector<string> &way_map_role = relWayMapRole.back() ;
                vector<string> &rel_map_role = relRelMapRole.back() ;

                while ( rd.read() )
                {
                    if ( rd.isStartElement("member") )
                    {
                        string type = rd.attribute("type") ;
                        string ref = rd.attribute("ref") ;
                        string role = rd.attribute("role") ;

                        if ( ref.empty() || type.empty() ) return false ;

                        if ( type == "node" )
                        {
                            node_map_item.push_back(ref) ;
                            node_map_role.push_back(role) ;
                        }
                        else if ( type == "way" )
                        {
                            way_map_item.push_back(ref) ;
                            way_map_role.push_back(role) ;
                        }
                        else if ( type == "relation" )
                        {
                            rel_map_item.push_back(ref) ;
                            rel_map_role.push_back(role) ;
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

                relMap[relation.id_] = relations_.size() ;
                relations_.push_back(relation) ;

            }
        }

    }

    // establish feature dependencies

    for(uint i=0 ; i<ways_.size() ; i++ )
    {
        Way &way = ways_[i] ;

        vector<string> &node_refs = wayNodeMap[i] ;

        for(uint j=0 ; j<node_refs.size() ; j++ )
        {
            uint idx = nodeMap[node_refs[j]] ;
            way.nodes_.push_back(idx) ;

            nodes_[idx].ways_.push_back(i) ;
        }

    }

    for(uint i=0 ; i<relations_.size() ; i++ )
    {
        Relation &relation = relations_[i] ;

        vector<string> &node_refs = relNodeMap[i] ;
        vector<string> &node_roles = relNodeMapRole[i] ;

        for(uint j=0 ; j<node_refs.size() ; j++ )
        {
            auto it = nodeMap.find(node_refs[j]) ;

            if ( it != nodeMap.end() )
            {
                int idx = (*it).second ;
                relation.nodes_.push_back(idx) ;
                relation.nodes_role_.push_back(node_roles[j]) ;

                nodes_[idx].relations_.push_back(i) ;
            }
        }

    }

    for(uint i=0 ; i<relations_.size() ; i++ )
    {
        Relation &relation = relations_[i] ;

        vector<string> &way_refs = relWayMap[i] ;
        vector<string> &way_roles = relWayMapRole[i] ;

        for(uint j=0 ; j<way_refs.size() ; j++ )
        {
            auto it = wayMap.find(way_refs[j]) ;

            if ( it != wayMap.end() )
            {
                uint idx = (*it).second ;
                relation.ways_.push_back(idx) ;
                relation.ways_role_.push_back(way_roles[j]) ;

                ways_[idx].relations_.push_back(i) ;
            }
        }
    }

    for(uint i=0 ; i<relations_.size() ; i++ )
    {
        Relation &relation = relations_[i] ;

        vector<string> &rel_refs = relRelMap[i] ;
        vector<string> &rel_roles = relRelMapRole[i] ;

        for(uint j=0 ; j<rel_refs.size() ; j++ )
        {
            auto it = relMap.find(rel_refs[j]) ;

            if ( it != relMap.end() )
            {
                uint idx = relMap[rel_refs[j]] ;
                relation.children_.push_back(idx) ;
                relation.children_role_.push_back(rel_roles[j]) ;

                relations_[idx].parents_.push_back(i) ;
            }
        }
    }
#endif
    return true ;
}

}

#include "osm_document.hpp"
#include "osm_rule_parser.hpp"

#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <set>

#include "xml_reader.hpp"
#include "zfstream.hpp"

#include <boost/algorithm/string.hpp>

using namespace std ;

namespace OSM {

bool Document::readXML(istream &strm)
{
    XmlReader rd(strm) ;

    map<string, uint> nodeMap, wayMap, relMap ;
    vector<  vector<string> > wayNodeMap, relNodeMap, relWayMap, relRelMap ;
    vector<  vector<string> > relNodeMapRole, relWayMapRole, relRelMapRole ;

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

    return true ;
}


bool Document::read(const string &fileName)
{
    if ( boost::ends_with(fileName, ".osm.gz") )
    {
        gzifstream strm(fileName.c_str()) ;

        return readXML(strm) ;
    }
    else if ( boost::ends_with(fileName, ".osm") )
    {
        ifstream strm(fileName.c_str()) ;

        return readXML(strm) ;
    }
    else if ( boost::ends_with(fileName, ".pbf") )
    {
        return readPBF(fileName) ;
    }

    return false ;
}

void Document::write(const string &fileName)
{
    if ( boost::ends_with(fileName, ".osm.gz") )
    {
        gzofstream strm(fileName.c_str()) ;

        return writeXML(strm) ;
    }
    else if ( boost::ends_with(fileName, ".osm") )
    {
        ofstream strm(fileName.c_str()) ;

        return writeXML(strm) ;
    }


}

void Document::writeXML(ostream &strm)
{

    strm << "<?xml version='1.0' encoding='UTF-8'?>\n" ;
    strm << "<osm version='0.6' generator='JOSM'>\n" ;

    for(int i=0 ; i<nodes_.size() ; i++ )
    {
        const Node &node = nodes_[i] ;

        strm << '\t' << "<node id='" << node.id_ << "' visible='true' lat='" << setprecision(12) << node.lat_ <<
            "' lon='" << setprecision(12) << node.lon_  ;

        if ( node.tags_.empty() ) strm <<  "' />\n" ;
        else
        {
            strm << "' >\n" ;

            auto tags = node.tags_.keys() ;

            for( int j=0 ; j<tags.size() ; j++ )
                strm << "\t\t" << "<tag k='" << tags[j] << "' v='" << node.tags_.get(tags[j]) << "' />\n" ;

            strm << "\t</node>\n" ;
        }
    }

    for(int i=0 ; i<ways_.size() ; i++ )
    {
        const Way &way = ways_[i] ;

        strm << "\t<way id='" << way.id_ << "' action='modify' visible='true'>\n" ;

        auto tags = way.tags_.keys() ;

        for( int j=0 ; j<tags.size() ; j++ )
            strm << "\t\t" << "<tag k='" << tags[j] << "' v='" << way.tags_.get(tags[j]) << "' />\n" ;

        for(int j=0 ; j<way.nodes_.size() ; j++ )
        {
            strm << "\t\t<nd ref='" << nodes_[way.nodes_[j]].id_ << "'/>\n" ;
        }

        strm << "\t</way>\n" ;
    }

    for(int i=0 ; i<relations_.size() ; i++ )
    {
        const Relation &relation = relations_[i] ;

        strm << "\t<relation id='" << relation.id_ << "' action='modify' visible='true'>\n" ;

        auto tags = relation.tags_.keys() ;

        for( int j=0 ; j<tags.size() ; j++ )
            strm << "\t\t" << "<tag k='" << tags[j] << "' v='" << relation.tags_.get(tags[j]) << "' />\n" ;

        for(int j=0 ; j<relation.nodes_.size() ; j++ )
        {
            strm << "\t\t<member type='node' ref='" << nodes_[relation.nodes_[j]].id_ << "' role=''/>\n" ;
        }

        for(int j=0 ; j<relation.ways_.size() ; j++ )
        {
            strm << "\t\t<member type='way' ref='" << ways_[relation.ways_[j]].id_ << "' role=''/>\n" ;
        }

        strm << "\t</relation>\n" ;
    }


    strm << "</osm>" ;
}



} // namespace OSM

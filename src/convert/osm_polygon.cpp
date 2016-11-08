#include "osm_document.hpp"
#include "osm_accessor.hpp"

#include <list>

using namespace std ;

namespace OSM {

// the function will create linear rings from relation members ignoring inner, outer roles
// the topology will be fixed by spatialite function ST_BuildArea

bool DocumentReader::makePolygonsFromRelation(DocumentAccessor &doc, const Relation &rel, Polygon &polygon)
{
    vector<string> roles ;
    vector<Ring> &rings = polygon.rings_ ;

    list<uint> unassigned_ways ;

    // first create rings from closed ways

    for(uint i=0 ; i<rel.ways_.size() ; i++)
    {
        Way way ;
        if ( !doc.readWay(rel.ways_[i], way) ) continue ;

        const string &role = rel.ways_role_[i] ;

        if (  way.nodes_.front() == way.nodes_.back() )
        {
            Ring r ;
            r.nodes_.insert(r.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
            rings.push_back(r) ;
            roles.push_back(role) ;
        }
        else unassigned_ways.push_back(i) ;
    }

    while ( !unassigned_ways.empty() )
    {
        Ring current ;
        deque<osm_id_t> cnodes ;

        uint idx = unassigned_ways.front() ;

        Way way ;
        if ( !doc.readWay(rel.ways_[idx], way) ) {
            unassigned_ways.pop_front() ;
            continue ;
        }

        string current_role = rel.ways_role_[idx] ;

        cnodes.insert(cnodes.end(), way.nodes_.begin(), way.nodes_.end()) ;
        unassigned_ways.pop_front() ;

        // merge ways into circular rings

        bool finished ;

        do
        {
            finished = true ;

            auto itu = unassigned_ways.begin() ;

            while ( itu != unassigned_ways.end() )
            {
                int idx = *itu ;

                Way way ;
                if ( !doc.readWay(rel.ways_[idx], way) ) {
                    ++itu ; continue ;
                }

                string role = rel.ways_role_[idx] ;

                if ( role != current_role ) { ++itu ; continue ; }

                if ( cnodes.front() == way.nodes_.front() )
                {
                    auto it = way.nodes_.begin() ;
                    ++it ;
                    while ( it != way.nodes_.end() )
                       cnodes.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( cnodes.back() == way.nodes_.front() )
                {
                    cnodes.insert(cnodes.end(), way.nodes_.begin()+1, way.nodes_.end()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( cnodes.front() == way.nodes_.back() )
                {
                    auto it = way.nodes_.rbegin() ;
                    ++it ;
                    while ( it != way.nodes_.rend() )
                        cnodes.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( cnodes.back() == way.nodes_.back() )
                {
                    cnodes.insert(cnodes.end(), way.nodes_.rbegin()+1, way.nodes_.rend()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else  ++itu ;
            }
        } while ( !finished ) ;


        // we should have a closed way otherwise something is wrong
        if ( cnodes.front() != cnodes.back() ) return false ;

        current.nodes_.assign(cnodes.begin(), cnodes.end());

        rings.push_back(current) ;
        roles.push_back(current_role) ;
        current.nodes_.clear() ;
    }

    return true ;
}


bool DocumentReader::makeWaysFromRelation(DocumentAccessor &doc, const Relation &rel, std::vector<Way> &ways)
{
    vector<Ring> rings ;

    list<osm_id_t> unassigned_ways ;

    for(uint i=0 ; i<rel.ways_.size() ; i++)
         unassigned_ways.push_back(rel.ways_[i]) ;

    while ( !unassigned_ways.empty() )
    {
        Ring current ;
        deque<osm_id_t> cnodes ;

        osm_id_t idx = unassigned_ways.front() ;

        Way way ;
        if ( !doc.readWay(idx, way) ) {
            unassigned_ways.pop_front() ;
            continue ;
        }

        cnodes.insert(cnodes.end(), way.nodes_.begin(), way.nodes_.end()) ;
        unassigned_ways.pop_front() ;

        // merge ways into circular rings

        bool finished ;

        do
        {
            finished = true ;

            list<osm_id_t>::iterator itu = unassigned_ways.begin() ;

            while ( itu != unassigned_ways.end() )
            {
                osm_id_t idx = *itu ;

                Way way ;
                if ( !doc.readWay(idx, way) ) {
                    ++itu ;
                    continue ;
                }

                if ( cnodes.front() == way.nodes_.front() )
                {
                    auto it = way.nodes_.begin() ;
                    ++it ;
                    while ( it != way.nodes_.end() )
                       cnodes.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( cnodes.back() == way.nodes_.front() )
                {
                    cnodes.insert(cnodes.end(), way.nodes_.begin()+1, way.nodes_.end()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( cnodes.front() == way.nodes_.back() )
                {
                    auto it = way.nodes_.rbegin() ;
                    ++it ;
                    while ( it != way.nodes_.rend() )
                        cnodes.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( cnodes.back() == way.nodes_.back() )
                {
                    cnodes.insert(cnodes.end(), way.nodes_.rbegin()+1, way.nodes_.rend()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else  ++itu ;
            }
        } while ( !finished ) ;


        current.nodes_.assign(cnodes.begin(), cnodes.end()) ;
        rings.push_back(current) ;
    }

    for(int i=0 ; i< rings.size() ; i++ )
    {
        ways.push_back(Way()) ;
        Way &way = ways.back() ;

        way.nodes_.insert(way.nodes_.end(), rings[i].nodes_.begin(), rings[i].nodes_.end()) ;

        way.tags_ = rel.tags_ ;
    }

    return true ;

}

}

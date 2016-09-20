#include "osm_document.hpp"

#include <list>

using namespace std ;

namespace OSM {

// the function will create linear rings from relation members ignoring inner, outer roles
// the topology will be fixed by spatialite function ST_BuildArea

bool Document::makePolygonsFromRelation(const Document &doc, const Relation &rel, Polygon &polygon)
{
    vector<string> roles ;
    vector<Ring> &rings = polygon.rings_ ;

    list<uint> unassigned_ways ;

    // first create rings from closed ways

    for(uint i=0 ; i<rel.ways_.size() ; i++)
    {
        const Way &way = doc.ways_[rel.ways_[i]] ;
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

        uint idx = unassigned_ways.front() ;

        const Way &way = doc.ways_[rel.ways_[idx]] ;

        string current_role = rel.ways_role_[idx] ;

        current.nodes_.insert(current.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
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

                const Way &way = doc.ways_[rel.ways_[idx]] ;
                string role = rel.ways_role_[idx] ;

                if ( role != current_role ) { ++itu ; continue ; }

                if ( current.nodes_.front() == way.nodes_.front() )
                {
                    auto it = way.nodes_.begin() ;
                    ++it ;
                    while ( it != way.nodes_.end() )
                       current.nodes_.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( current.nodes_.back() == way.nodes_.front() )
                {
                    current.nodes_.insert(current.nodes_.end(), way.nodes_.begin()+1, way.nodes_.end()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( current.nodes_.front() == way.nodes_.back() )
                {
                    auto it = way.nodes_.rbegin() ;
                    ++it ;
                    while ( it != way.nodes_.rend() )
                        current.nodes_.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( current.nodes_.back() == way.nodes_.back() )
                {
                    current.nodes_.insert(current.nodes_.end(), way.nodes_.rbegin()+1, way.nodes_.rend()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else  ++itu ;
            }
        } while ( !finished ) ;


        // we should have a closed way otherwise something is wrong
        if ( current.nodes_.front() != current.nodes_.back() ) return false ;

        rings.push_back(current) ;
        roles.push_back(current_role) ;
        current.nodes_.clear() ;
    }

    return true ;
}


bool Document::makeWaysFromRelation(const Document &doc, const Relation &rel, std::vector<Way> &ways)
{
    vector<Ring> rings ;

    list<uint> unassigned_ways ;

    for(uint i=0 ; i<rel.ways_.size() ; i++)
         unassigned_ways.push_back(i) ;

    while ( !unassigned_ways.empty() )
    {
        Ring current ;

        uint idx = unassigned_ways.front() ;

        const Way &way = doc.ways_[rel.ways_[idx]] ;

        current.nodes_.insert(current.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
        unassigned_ways.pop_front() ;

        // merge ways into circular rings

        bool finished ;

        do
        {
            finished = true ;

            list<uint>::iterator itu = unassigned_ways.begin() ;

            while ( itu != unassigned_ways.end() )
            {
                int idx = *itu ;

                const Way &way = doc.ways_[rel.ways_[idx]] ;

                if ( current.nodes_.front() == way.nodes_.front() )
                {
                    auto it = way.nodes_.begin() ;
                    ++it ;
                    while ( it != way.nodes_.end() )
                       current.nodes_.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( current.nodes_.back() == way.nodes_.front() )
                {
                    current.nodes_.insert(current.nodes_.end(), way.nodes_.begin()+1, way.nodes_.end()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( current.nodes_.front() == way.nodes_.back() )
                {
                    auto it = way.nodes_.rbegin() ;
                    ++it ;
                    while ( it != way.nodes_.rend() )
                        current.nodes_.push_front(*it++) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else if ( current.nodes_.back() == way.nodes_.back() )
                {
                    current.nodes_.insert(current.nodes_.end(), way.nodes_.rbegin()+1, way.nodes_.rend()) ;
                    finished = false ;
                    itu = unassigned_ways.erase(itu) ;
                }
                else  ++itu ;
            }
        } while ( !finished ) ;


        rings.push_back(current) ;
        current.nodes_.clear() ;

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

#ifndef __OSM_DOCUMENT_H__
#define __OSM_DOCUMENT_H__

#include <string>
#include <map>
#include <vector>
#include <deque>

#include "dictionary.hpp"
#include <unordered_map>

typedef int64_t osm_id_t ;

namespace OSM {

struct Feature {

    Feature() {}

    osm_id_t id_ ; // feature id
    Dictionary tags_ ; // tags associated with this feature
};

struct Node: public Feature {

    Node(): Feature() {}

    double lat_, lon_ ;
} ;


struct Way: public Feature {

    Way(): Feature() {}

    std::vector<osm_id_t> nodes_ ;     // nodes corresponding to this way
} ;


struct Relation: public Feature {

    Relation(): Feature() {}

    std::vector<osm_id_t> nodes_ ;     // node members
    std::vector<osm_id_t> ways_ ;      // way members
    std::vector<osm_id_t> children_ ; // relation members

    std::vector<std::string> nodes_role_ ;
    std::vector<std::string> ways_role_ ;
    std::vector<std::string> children_role_ ;
};

struct Ring {
    std::vector<osm_id_t> nodes_ ;
};

struct Polygon: public Feature {

    Polygon(): Feature() {}

    std::vector<Ring> rings_ ;
};


class DocumentReader {
public:

    // empty document
    DocumentReader() {}

    // open an existing document
    DocumentReader(const std::string &fileName) ;

    // read OSM file into memory (format determined by extension)
    bool read(const std::string &fileName) ;

    // readers

    bool readWay(osm_id_t id, Way &way) {
        auto it = ways_.find(id) ;
        if ( it == ways_.end() ) return false ;
        way = it->second ;
        return true ;
    }

    void readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways) {
        for( osm_id_t id: ids ) {
            auto it = ways_.find(id) ;
            if ( it != ways_.end() )
            ways.push_back(it->second) ;
        }
    }

    void forAllWayCoords(osm_id_t wid, std::function<void(osm_id_t, double, double)> cf) {
        auto it = ways_.find(wid) ;
        if ( it != ways_.end() ) {
            for( osm_id_t id: it->second.nodes_ ) {
                auto nit = nodes_.find(id) ;
                if ( nit != nodes_.end() ) {
                    const Node &n = nit->second ;
                    cf(n.id_, n.lat_, n.lon_) ;
                }
            }
        }
    }

    void forAllNodeCoordList(const std::vector<osm_id_t> &ids, std::function<void(double lat, double lon)> cf) {
        for( auto &id: ids ) {
            auto nit = nodes_.find(id) ;
            if ( nit != nodes_.end() ) {
                const Node &n = nit->second ;
                cf(n.lat_, n.lon_) ;
            }
        }
    }

    void readParentRelations(osm_id_t id, std::vector<Relation> &parents) {
        auto pr = memberships_.equal_range(id) ;
        std::for_each(pr.first, pr.second, [this, &parents](const std::pair<osm_id_t, osm_id_t> &rp) {
            auto it = relations_.find(rp.second) ;
            if ( it != relations_.end() ) {
                Relation rel ;
                rel.id_ = it->second.id_ ;
                rel.tags_ = it->second.tags_ ;
                parents.push_back(rel) ;
            }
        }) ;
    }

    void forAllNodes(std::function<void(const Node &)> cf) {
        for( const auto &np: nodes_ ) cf(np.second) ;
    }

    void forAllWays(std::function<void(const Way &)> cf) {
        for( const auto &wp: ways_ ) cf(wp.second) ;
    }

    void forAllRelations(std::function<void(const Relation &)> cf) {
        for( const auto &rp: relations_ ) cf(rp.second) ;
    }

    bool makePolygonsFromRelation(const Relation &rel, Polygon &polygon) ;
    bool makeWaysFromRelation(const Relation &rel, std::vector<Way> &ways) ;

private:

    std::unordered_map<osm_id_t, Node> nodes_ ;
    std::unordered_map<osm_id_t, Way> ways_ ;
    std::unordered_map<osm_id_t, Relation> relations_ ;
    std::unordered_multimap<osm_id_t, osm_id_t> memberships_ ;

protected:

    friend class XMLReader ;
    friend class PBFReader ;

    bool readXML(std::istream &strm) ;
    void writeXML(std::ostream &strm);

    bool readPBF(const std::string &fileName) ;
    bool isPBF(const std::string &fileName) ;

    void writeNode(const Node &n) {
        nodes_.insert(std::make_pair(n.id_, n)) ;
    }

    void writeWay(const Way &way) {
        if ( !way.tags_.empty() && !way.nodes_.empty())
            ways_.insert(std::make_pair(way.id_, way)) ;
    }

    void writeRelation(const Relation &r ) {
        relations_.insert(std::make_pair(r.id_,r)) ;
        for( osm_id_t node_id: r.nodes_ )
            memberships_.insert(std::make_pair(node_id, r.id_)) ;
        for( osm_id_t way_id: r.ways_ )
            memberships_.insert(std::make_pair(way_id, r.id_)) ;
        for( osm_id_t rel_id: r.children_ )
            memberships_.insert(std::make_pair(rel_id, r.id_)) ;
    }



};


}

#endif

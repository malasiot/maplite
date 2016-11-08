#ifndef __OSM_MEMORY_ACCESSOR_HPP__
#define __OSM_MEMORY_ACCESSOR_HPP__

#include "osm_accessor.hpp"
#include <unordered_map>
#include <functional>

namespace OSM {

// Simple accessor with all data stored in memory
// No error checking is performed

class MemoryBasedAccessor: public DocumentAccessor {
public:
    MemoryBasedAccessor(): DocumentAccessor() {}
    virtual ~MemoryBasedAccessor() {}

    virtual bool readWay(osm_id_t id, Way &way) {
        auto it = ways_.find(id) ;
        if ( it == ways_.end() ) return false ;
        way = it->second ;
        return true ;
    }

    virtual void readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways) override {
        for( osm_id_t id: ids ) {
            auto it = ways_.find(id) ;
            if ( it != ways_.end() )
            ways.push_back(it->second) ;
        }
    }

    virtual void readWayNodes(osm_id_t wid, std::vector<Node> &nodes) override {
        auto it = ways_.find(wid) ;
        if ( it != ways_.end() ) {
            for( osm_id_t id: it->second.nodes_ ) {
                auto nit = nodes_.find(id) ;
                if ( nit != nodes_.end() )
                nodes.push_back(nit->second) ;
            }
        }
    }

    virtual void readParentRelations(osm_id_t id, std::vector<Relation> &parents) {
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

    virtual void forAllNodes(std::function<void(const Node &)> cf) {
        for( auto &np: nodes_ ) cf(np.second) ;
    }

    virtual void forAllWays(std::function<void(const Way &)> cf) {
        for( auto &wp: ways_ ) cf(wp.second) ;
    }

    virtual void forAllRelations(std::function<void(const Relation &)> cf) {
        for( auto &rp: relations_ ) cf(rp.second) ;
    }

    virtual void writeNode(const Node &n) override {
        nodes_.insert(std::make_pair(n.id_, n)) ;
    }

    virtual void writeWay(const Way &way) {
        ways_.insert(std::make_pair(way.id_, way)) ;
    }

    virtual void writeRelation(const Relation &r ) override {
        relations_.insert(std::make_pair(r.id_,r)) ;
        for( osm_id_t node_id: r.nodes_ )
            memberships_.insert(std::make_pair(node_id, r.id_)) ;
        for( osm_id_t way_id: r.ways_ )
            memberships_.insert(std::make_pair(way_id, r.id_)) ;
        for( osm_id_t rel_id: r.children_ )
            memberships_.insert(std::make_pair(rel_id, r.id_)) ;
    }

    virtual void beginWrite() override {}
    virtual void endWrite() override {}

private:

    std::unordered_map<osm_id_t, Node> nodes_ ;
    std::unordered_map<osm_id_t, Way> ways_ ;
    std::unordered_map<osm_id_t, Relation> relations_ ;
    std::unordered_multimap<osm_id_t, osm_id_t> memberships_ ;
};

}

#endif

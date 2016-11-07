#ifndef __OSM_MEMORY_ACCESSOR_HPP__
#define __OSM_MEMORY_ACCESSOR_HPP__

#include "osm_accessor.hpp"
#include <unordered_map>

namespace OSM {

// Simple accessor with all data stored in memory
// No error checking is performed

class MemoryBasedAccessor: public DocumentAccessor {
public:
    MemoryBasedAccessor(): DocumentAccessor() {}
    virtual ~MemoryBasedAccessor() {}

    virtual Way fetchWay(osm_id_t id) {
        return std::move(ways_[id]) ;
    }

    virtual std::vector<Way> fetchWays(const std::vector<osm_id_t> ids) override {
        std::vector<Way> ways ;
        for( osm_id_t id: ids ) {
            ways.push_back(ways_[id]) ;
        }
        return std::move(ways) ;
    }

    virtual Relation fetchRelation(osm_id_t id) override {
        return std::move(relations_[id]) ;
    }

    virtual std::vector<Relation> fetchRelations(const std::vector<osm_id_t> &ids) override {
        std::vector<Relation> relations ;
        for( osm_id_t id: ids ) {
            relations.push_back(relations_[id]) ;
        }
        return std::move(relations) ;
    }

    virtual std::vector<Node> fetchNodes(const std::vector<osm_id_t> &ids) override {
        std::vector<Node> nodes ;
        for( osm_id_t id: ids ) {
            nodes.push_back(nodes_[id]) ;
        }
        return std::move(nodes) ;
    }

    class NodeIterator: public NodeIteratorBase {
    public:
        NodeIterator(MemoryBasedAccessor &a): NodeIteratorBase(), doc_(a), it_(doc_.nodes_.begin()) {}
        bool isValid() const override { return it_ != doc_.nodes_.end() ; }
        void next() override { ++it_ ; }
        const Node &current() const override { return it_->second ; }

        private:
        MemoryBasedAccessor &doc_ ;
        std::unordered_map<int64_t, Node>::const_iterator it_ ;
    };

    class WayIterator: public WayIteratorBase {
    public:
        WayIterator(MemoryBasedAccessor &a): WayIteratorBase(), doc_(a), it_(doc_.ways_.begin()) {}
        bool isValid() const override { return it_ != doc_.ways_.end() ; }
        void next() override { ++it_ ; }
        const Way &current() const override { return it_->second ; }

        private:
        MemoryBasedAccessor &doc_ ;
        std::unordered_map<int64_t, Way>::const_iterator it_ ;
    };

    class RelationIterator: public RelationIteratorBase {
    public:
        RelationIterator(MemoryBasedAccessor &a): RelationIteratorBase(), doc_(a), it_(doc_.relations_.begin()) {}
        bool isValid() const override { return it_ != doc_.relations_.end() ; }
        void next() override { ++it_ ; }
        const Relation &current() const override { return it_->second ; }

        private:
        MemoryBasedAccessor &doc_ ;
        std::unordered_map<int64_t, Relation>::const_iterator it_ ;
    };

    virtual std::unique_ptr<NodeIteratorBase> nodes() override { return std::unique_ptr<NodeIteratorBase>(new NodeIterator(*this)) ; }
    virtual std::unique_ptr<WayIteratorBase> ways() override  { return std::unique_ptr<WayIteratorBase>(new WayIterator(*this)) ; }
    virtual std::unique_ptr<RelationIteratorBase> relations() override { return std::unique_ptr<RelationIteratorBase>(new RelationIterator(*this)) ; }

    virtual void writeNode(const Node &n) override {
        nodes_.insert(std::make_pair(n.id_, n)) ;
    }

    virtual void writeWay(const Way &way) {
        ways_.insert(std::make_pair(way.id_, way)) ;
    }

    virtual void writeRelation(const Relation &r ) override {
        relations_.insert(std::make_pair(r.id_,r)) ;
    }


private:

    std::unordered_map<int64_t, Node> nodes_ ;
    std::unordered_map<int64_t, Way> ways_ ;
    std::unordered_map<int64_t, Relation> relations_ ;
};

}

#endif

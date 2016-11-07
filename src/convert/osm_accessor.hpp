#ifndef __OSM_ACCESSOR_HPP__
#define __OSM_ACCESSOR_HPP__

#include "osm_document.hpp"
#include "geometry.hpp"

#include <memory>

// abstract class for mid-level storage of OSM data

namespace OSM {

template<typename T>
class IteratorBase {
public:

    explicit IteratorBase() {}

    const T &operator*() const { return current() ; }

    virtual void next() = 0 ;
    virtual bool isValid() const = 0 ;
    virtual const T &current() const = 0 ;
};

typedef IteratorBase<Node> NodeIteratorBase ;
typedef IteratorBase<Way> WayIteratorBase ;
typedef IteratorBase<Relation> RelationIteratorBase ;

class DocumentAccessor {
public:
    DocumentAccessor() {}
    virtual ~DocumentAccessor() {}

    // readers

    virtual Way fetchWay(osm_id_t id) = 0;
    virtual std::vector<Way> fetchWays(const std::vector<osm_id_t> ids) = 0;
    virtual Relation fetchRelation(osm_id_t id) = 0 ;
    virtual std::vector<Relation> fetchRelations(const std::vector<osm_id_t> &ids) = 0 ;
    virtual std::vector<Node> fetchNodes(const std::vector<osm_id_t> &ids) = 0 ;

    // writers

    virtual void writeNode(const Node &n) = 0 ;
    virtual void writeWay(const Way &way) = 0 ;
    virtual void writeRelation(const Relation &r ) = 0 ;

    // read-only iterators

    virtual std::unique_ptr<NodeIteratorBase> nodes() = 0 ;
    virtual std::unique_ptr<WayIteratorBase> ways() = 0 ;
    virtual std::unique_ptr<RelationIteratorBase> relations() = 0 ;
};



}
#endif

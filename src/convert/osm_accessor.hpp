#ifndef __OSM_ACCESSOR_HPP__
#define __OSM_ACCESSOR_HPP__

#include "osm_document.hpp"
#include "geometry.hpp"

#include <memory>

// abstract class for mid-level storage of OSM data

namespace OSM {

class DocumentAccessor {
public:
    DocumentAccessor() {}
    virtual ~DocumentAccessor() {}

    // readers

    virtual bool readWay(osm_id_t id, Way &) = 0;
    virtual void readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways) = 0;
    virtual void readWayNodes(osm_id_t wid, std::vector<Node> &nodes) = 0 ;
    virtual void readParentRelations(osm_id_t id, std::vector<Relation> &parents) = 0 ;

    // writers

    virtual void writeNode(const Node &n) = 0 ;
    virtual void writeWay(const Way &way) = 0 ;
    virtual void writeRelation(const Relation &r ) = 0 ;
    virtual void beginWrite() = 0 ;
    virtual void endWrite() = 0  ;

    // read-only iterators

    virtual void forAllNodes(std::function<void(const Node &)> cf) = 0 ;
    virtual void forAllWays(std::function<void(const Way &)> cf) = 0 ;
    virtual void forAllRelations(std::function<void(const Relation &)> cf) = 0 ;

};



}
#endif

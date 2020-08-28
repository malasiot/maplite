#ifndef __OSM_STORAGE_HPP__
#define __OSM_STORAGE_HPP__

#include "dictionary.hpp"
#include <unordered_map>
#include <map>

typedef int64_t osm_id_t ;
enum osm_feature_t { osm_node_t = 0, osm_way_t = 1, osm_relation_t = 2} ;

namespace OSM {

struct Feature {

    Feature() {}

    osm_id_t id_ ; // feature id
    Dictionary tags_ ; // tags associated with this feature
    bool delete_ = false ;
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

// Interface for in-memory or on-disk storage of OSM data
class Storage {

public:

    virtual ~Storage() {}
    virtual void writeBegin() {}
    virtual void writeEnd() {}
    virtual void writeNode(const Node &n) = 0;
    virtual void writeWay(const Way &way) = 0 ;
    virtual void writeRelation(const Relation &r ) = 0;

    // readers

    virtual bool readWay(osm_id_t id, Way &way) = 0;
    virtual void readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways) = 0 ;
    virtual void forAllWayCoords(osm_id_t wid, std::function<void(osm_id_t, double, double)> cf) = 0;
    virtual void forAllNodeCoordList(const std::vector<osm_id_t> &ids, std::function<void(double lat, double lon)> cf) = 0;
    virtual void readParentRelations(osm_id_t id, std::vector<Relation> &parents)  = 0;
    virtual void forAllNodes(std::function<void(const Node &)> cf) = 0;
    virtual void forAllWays(std::function<void(const Way &)> cf) = 0;
    virtual void forAllRelations(std::function<void(const Relation &)> cf) = 0;

    bool makePolygonsFromRelation(const Relation &rel, Polygon &polygon) ;
    bool makeWaysFromRelation(const Relation &rel, std::vector<Way> &ways) ;
};

}
#endif

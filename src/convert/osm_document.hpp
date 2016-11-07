#ifndef __OSM_DOCUMENT_H__
#define __OSM_DOCUMENT_H__

#include <string>
#include <map>
#include <vector>
#include <deque>

#include "dictionary.hpp"

typedef int64_t osm_id_t ;

namespace OSM {

struct Feature {

    enum Type { NodeFeature, WayFeature, RelationFeature, PolygonFeature } ;

    Feature(Type type): type_(type) {}

    osm_id_t id_ ; // feature id
    Dictionary tags_ ; // tags associated with this feature
    Type type_ ; // feature type ;
};

struct Node: public Feature {

    Node(): Feature(NodeFeature) {}

    double lat_, lon_ ;

    std::vector<osm_id_t> ways_ ;      // ways in which this nodes participates
    std::vector<osm_id_t> relations_ ; // relations that this node participates directly
} ;


struct Way: public Feature {

    Way(): Feature(WayFeature) {}

    std::vector<osm_id_t> nodes_ ;     // nodes corresponding to this way
    std::vector<osm_id_t> relations_ ; // relations that this way participates
} ;


struct Relation: public Feature {

    Relation(): Feature(RelationFeature) {}

    std::vector<osm_id_t> nodes_ ;     // node members
    std::vector<osm_id_t> ways_ ;      // way members
    std::vector<osm_id_t> children_ ; // relation members

    std::vector<std::string> nodes_role_ ;
    std::vector<std::string> ways_role_ ;
    std::vector<std::string> children_role_ ;

    std::vector<osm_id_t> parents_ ;    // parent relations
};

struct Ring {
    std::vector<osm_id_t> nodes_ ;
};

struct Polygon: public Feature {

    Polygon(): Feature(PolygonFeature) {}

    std::vector<Ring> rings_ ;
};

class DocumentAccessor ;

class DocumentReader {
public:

    // empty document
    DocumentReader() {}

    // open an existing document
    DocumentReader(const std::string &fileName) ;

    // read Osm file (format determined by extension)
    bool read(const std::string &fileName, DocumentAccessor &acc) ;

protected:

    bool readXML(std::istream &strm, DocumentAccessor &) ;
    bool readPBF(const std::string &fileName, DocumentAccessor &) ;
    bool isPBF(const std::string &fileName, DocumentAccessor &) ;

public:

    static bool makePolygonsFromRelation(DocumentAccessor &doc, const Relation &rel, Polygon &polygon) ;
    static bool makeWaysFromRelation(DocumentAccessor &doc, const Relation &rel, std::vector<Way> &ways) ;

};


}

#endif

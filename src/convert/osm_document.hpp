#ifndef __OSM_DOCUMENT_H__
#define __OSM_DOCUMENT_H__

#include <string>
#include <map>
#include <vector>
#include <deque>

#include "dictionary.hpp"

namespace OSM {

struct Feature {

    enum Type { NodeFeature, WayFeature, RelationFeature, PolygonFeature } ;

    Feature(Type type): type_(type), visited_(false) {}

    std::string id_ ; // feature id
    Dictionary tags_ ; // tags associated with this feature
    Type type_ ; // feature type ;
    bool visited_ ; // used by algorithms ;
};

class Way ;
class Relation ;

struct Node: public Feature {

    Node(): Feature(NodeFeature) {}

    double lat_, lon_ ;

    std::vector<uint> ways_ ;      // ways in which this nodes participates
    std::vector<uint> relations_ ; // relations that this node participates directly
} ;


struct Way: public Feature {

    Way(): Feature(WayFeature) {}

    std::vector<uint> nodes_ ;     // nodes corresponding to this way
    std::vector<uint> relations_ ; // relations that this way participates
} ;


struct Relation: public Feature {

    Relation(): Feature(RelationFeature) {}

    std::vector<uint> nodes_ ;     // node members
    std::vector<uint> ways_ ;      // way members
    std::vector<uint> children_ ; // relation members

    std::vector<std::string> nodes_role_ ;
    std::vector<std::string> ways_role_ ;
    std::vector<std::string> children_role_ ;

    std::vector<uint> parents_ ;    // parent relations
};

struct Ring {
    std::deque<uint> nodes_ ;
};

struct Polygon: public Feature {

    Polygon(): Feature(PolygonFeature) {}

    std::vector<Ring> rings_ ;
};


class Document {
public:

    // empty document
    Document() {}

    // open an existing document
    Document(const std::string &fileName) ;

    // create OSM document by filtering an existing one
    Document(const Document &other, const std::string &filter) ;

    // read Osm file (format determined by extension)
    bool read(const std::string &fileName) ;

    // write Osm file (format determined by extension)
    void write(const std::string &fileName) ;

public:

    std::vector<Node> nodes_ ;
    std::vector<Way> ways_ ;
    std::vector<Relation> relations_ ;

protected:

    bool readXML(std::istream &strm) ;
    void writeXML(std::ostream &strm);

    bool readPBF(const std::string &fileName) ;
    bool isPBF(const std::string &fileName) ;

public:

    static bool makePolygonsFromRelation(const Document &doc, const Relation &rel, Polygon &polygon) ;
    static bool makeWaysFromRelation(const Document &doc, const Relation &rel, std::vector<Way> &ways) ;

};


}

#endif

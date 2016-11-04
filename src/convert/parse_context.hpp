#ifndef __PARSE_CONTEXT_HPP__
#define __PARSE_CONTEXT_HPP__

#include <vector>
#include <string>

#include "dictionary.hpp"
#include "osm_document.hpp"

struct TagWriteAction {
    TagWriteAction(const std::string &key, const std::string &val, uint8_t zmin, uint8_t zmax, bool attached = false ):
        key_(key), val_(val), zoom_min_(zmin), zoom_max_(zmax), attached_(attached) {}

    std::string key_, val_ ;
    uint8_t zoom_min_, zoom_max_ ;
    bool attached_ ;
};

struct TagWriteList {

    std::vector<TagWriteAction> actions_ ;
};

namespace OSM {
    namespace Filter {

class Context {

public:

    enum FeatureType { Way, Node } ;

    Context() {}
    Context(const OSM::Node &node, uint64_t fid, Document *doc, TagWriteList *tags):
        tags_(node.tags_), id_(node.id_), type_(Node), fid_(fid), doc_(doc), tw_(tags) {}
    Context(const OSM::Way &way, uint64_t fid, Document *doc, TagWriteList *tags):
        tags_(way.tags_), id_(way.id_), type_(Way), fid_(fid), doc_(doc), tw_(tags) {}
    Context(const Dictionary &tags, const std::string &id, FeatureType t): tags_(tags), id_(id), type_(t) {}

    FeatureType type() const { return type_ ; }

    bool has_tag(const std::string &tag) const { return tags_.contains(tag); }
    std::string value(const std::string &key) const { return tags_.get(key); }
    std::string id() const { return id_ ; }

    Dictionary tags_ ;
    std::string id_ ;
    FeatureType type_ ;
    Document *doc_ ;
    uint64_t fid_ ;
    TagWriteList *tw_ ;
};

}
}

#endif

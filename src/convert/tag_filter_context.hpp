#ifndef __TAG_FILTER_CONTEXT_HPP__
#define __TAG_FILTER_CONTEXT_HPP__

#include "osm_document.hpp"
#include <string>

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

class TagFilterContext {

public:

    enum FeatureType { Way, Node, Relation } ;

    TagFilterContext() {}
    TagFilterContext(const OSM::Node &node, uint64_t fid, OSM::DocumentAccessor *doc):
        tags_(node.tags_), id_(node.id_), type_(Node), fid_(fid), doc_(doc) {}
    TagFilterContext(const OSM::Way &way, uint64_t fid, OSM::DocumentAccessor *doc):
        tags_(way.tags_), id_(way.id_), type_(Way), fid_(fid), doc_(doc) {}
    TagFilterContext(const Dictionary &tags, uint64_t id, FeatureType t): tags_(tags), id_(id), type_(t) {}

    FeatureType type() const { return type_ ; }

    bool has_tag(const std::string &tag) const { return tags_.contains(tag); }
    std::string value(const std::string &key) const { return tags_.get(key); }
    int64_t id() const { return id_ ; }

    Dictionary tags_ ;
    int64_t id_ ;
    FeatureType type_ ;
    OSM::DocumentAccessor *doc_ ;
    uint64_t fid_ ;
    TagWriteList tw_ ;
};

#endif

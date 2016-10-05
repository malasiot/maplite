#ifndef __IMPORT_CONFIG_H__
#define __IMPORT_CONFIG_H__

#include <vector>

#include "osm_rule_parser.hpp"
#include "osm_document.hpp"

struct TagWriteAction {
    TagWriteAction(const std::string &key, const std::string &val, uint8_t zmin, uint8_t zmax ):
        key_(key), val_(val), zoom_min_(zmin), zoom_max_(zmax) {}

    std::string key_, val_ ;
    uint8_t zoom_min_, zoom_max_ ;
};

struct TagWriteList {

    std::vector<TagWriteAction> actions_ ;
};

struct NodeRuleMap {
    int node_idx_ ;
    std::vector<OSM::Filter::RulePtr> matched_rules_ ;
};

struct FilterConfig {
    FilterConfig(){}

    std::deque<OSM::Filter::RulePtr> rules_ ;

    bool parse(const std::string &fileName) ;
};

#endif

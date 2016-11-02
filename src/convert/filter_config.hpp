#ifndef __IMPORT_CONFIG_H__
#define __IMPORT_CONFIG_H__

#include <vector>

#include "osm_rule_parser.hpp"
#include "osm_document.hpp"


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

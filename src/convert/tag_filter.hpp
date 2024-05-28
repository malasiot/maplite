#ifndef __TAG_FILTER_HPP__
#define __TAG_FILTER_HPP__

#include <vector>

#include "lua_context.hpp"
#include "tag_filter_rule.hpp"

class TagFilterContext ;

class TagFilter {
public:
    TagFilter() = default ;

    // parse the configuration file
    bool parse(const std::string &fileName) ;

    // match feature (defined by context) to all rules and collect the TagWriteActions
    // returns false if no actions are found
    bool match(TagFilterContext &ctx, uint8_t &zmin, uint8_t &zmax);

    bool matchPOI(TagFilterContext &ctx) ;

private:

    std::deque<tag_filter::RulePtr> rules_ ;
    LuaContext lua_ ;

private:

    bool matchRule(const tag_filter::RulePtr &rule, TagFilterContext &ctx, bool &cont);
} ;



#endif

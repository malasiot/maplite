#ifndef __TAG_FILTER_CONFIG_PARSER_HPP__
#define __TAG_FILTER_CONFIG_PARSER_HPP__

#include "tag_filter_config_scanner.hpp"
#include "osm_document.hpp"
#include "tag_filter.hpp"

#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <regex>

#include "parser/tag_filter_parser.hpp"

class LuaContext ;

namespace tag_filter {
    class BisonParser ;
    class Scanner ;
}

class TagFilterConfigParser {

public:

    TagFilterConfigParser(std::istream &strm, LuaContext &lua)  ;

    bool parse() ;

    void error(const tag_filter::Parser::location_type &loc,  const std::string& m) ;

    tag_filter::Scanner scanner_;
    tag_filter::Parser parser_;

    std::deque<tag_filter::RulePtr> rules_ ;

    std::string error_string_, script_ ;
    tag_filter::Parser::location_type loc_ ;
    LuaContext &lua_ ;
 } ;

#endif

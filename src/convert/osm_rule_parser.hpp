#ifndef __OSM_RULE_PARSER_H__
#define __OSM_RULE_PARSER_H__

#include "osm_rule_scanner.hpp"
#include "osm_document.hpp"
#include "filter_config.hpp"

#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <regex>

#include "osm_parser.hpp"

namespace OSM {

class FlexScanner ;
class BisonParser ;

namespace Filter {

class Parser {

public:

    Parser(std::istream &strm, LuaContext &lua)  ;

    bool parse() ;

    void error(const BisonParser::location_type &loc,  const std::string& m) ;

    FlexScanner scanner_;
    BisonParser parser_;

    std::deque<RulePtr> rules_ ;

    std::string error_string_, script_ ;
    OSM::BisonParser::location_type loc_ ;
    LuaContext &lua_ ;
 } ;


} // namespace Filter
} // namespace OSM

#endif

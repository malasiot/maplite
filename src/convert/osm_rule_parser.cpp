#include "osm_rule_scanner.hpp"
#include "osm_rule_parser.hpp"

#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <errno.h>
using namespace std;

namespace OSM {
namespace Filter {

Parser::Parser(std::istream &strm, LuaContext &lua) :
    scanner_(strm), lua_(lua),
    parser_(*this, loc_)
{}

bool Parser::parse() {
 //      parser_.set_debug_level(2);

    loc_.initialize() ;
    int res = parser_.parse();

    return ( res == 0 ) ;
}

void Parser::error(const OSM::BisonParser::location_type &loc,
                   const std::string& m)
{
    std::stringstream strm ;

    strm << m << " near " << loc ;
    error_string_ = strm.str() ;
}
//////////////////////////////////////////////////////////////////



} // namespace Filter
} // namespace OSM

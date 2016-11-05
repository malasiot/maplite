#include "tag_filter_config_parser.hpp"

#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <errno.h>

using namespace std;

TagFilterConfigParser::TagFilterConfigParser(std::istream &strm, LuaContext &lua) :
    scanner_(strm), lua_(lua),
    parser_(*this, loc_)
{}

bool TagFilterConfigParser::parse() {
 //      parser_.set_debug_level(2);

    loc_.initialize() ;
    int res = parser_.parse();

    return ( res == 0 ) ;
}

void TagFilterConfigParser::error(const tag_filter::Parser::location_type &loc, const std::string& m)
{
    std::stringstream strm ;

    strm << m << " near " << loc ;
    error_string_ = strm.str() ;
}



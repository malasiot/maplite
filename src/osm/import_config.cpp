#include "import_config.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iomanip>
#include <fstream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

#include "geom_helpers.hpp"

using namespace std ;

static void split_line(const string &src, vector<string> &tokens) {

    using boost::tokenizer;
    using boost::escaped_list_separator;

    typedef tokenizer<escaped_list_separator<char> > stokenizer;

    stokenizer tok(src, escaped_list_separator<char>("\\", " ,\t",   "\"'"));

    for( stokenizer::iterator beg = tok.begin(); beg!=tok.end(); ++beg)
        if ( !beg->empty() ) tokens.push_back(*beg) ;
}

bool ImportConfig::parse(const string &fileName)
{
    ifstream strm(fileName.c_str()) ;
    if ( !strm ) {
        cerr << "Cannot open config file: " << fileName << endl ;
        return false ;
    }

    OSM::Filter::Parser parser(strm) ;

    if ( ! parser.parse() )  {
        cerr << "Error parsing " << fileName << " : error in rule (line: " << ")" << endl ;
        cerr << parser.error_string_ << endl ;

        return false ;
    }

    layers_ = parser.layers_ ;

    return true ;
}


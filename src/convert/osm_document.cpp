#include "osm_document.hpp"
#include "tag_filter_config_parser.hpp"

#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <set>

#include "xml_reader.hpp"
#include "zfstream.hpp"

#include <boost/algorithm/string.hpp>

using namespace std ;

namespace OSM {


bool DocumentReader::read(const string &fileName, DocumentAccessor &a)
{
    if ( boost::ends_with(fileName, ".osm.gz") )
    {
        gzifstream strm(fileName.c_str()) ;

        return readXML(strm, a) ;
    }
    else if ( boost::ends_with(fileName, ".osm") )
    {
        ifstream strm(fileName.c_str()) ;

        return readXML(strm, a) ;
    }
    else if ( boost::ends_with(fileName, ".pbf") )
    {
        return readPBF(fileName, a) ;
    }

    return false ;
}



} // namespace OSM

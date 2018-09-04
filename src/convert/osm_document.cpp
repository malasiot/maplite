#include "osm_document.hpp"
#include "tag_filter_config_parser.hpp"

#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <set>

#include "xml_reader.hpp"
#include "zfstream.hpp"

#include "osm_xml_reader.hpp"
#include "osm_pbf_reader.hpp"

#include <boost/algorithm/string.hpp>

using namespace std ;

namespace OSM {

bool DocumentReader::read(const string &fileName, Storage &storage)
{
    if ( boost::ends_with(fileName, ".osm.gz") )
    {
        gzifstream strm(fileName.c_str()) ;

        XMLReader reader ;
        return reader.read(strm, storage) ;
    }
    else if ( boost::ends_with(fileName, ".osm") )
    {
        ifstream strm(fileName.c_str()) ;

        XMLReader reader ;
        return reader.read(strm, storage) ;
    }
    else if ( boost::ends_with(fileName, ".pbf") )
    {
        ifstream strm(fileName.c_str(), ios::binary) ;

        PBFReader reader ;
        return reader.read(strm, storage) ;
    }

    return false ;
}



} // namespace OSM

#ifndef __OSM_XML_READER_HPP__
#define __OSM_XML_READER_HPP__

#include "osm_document.hpp"

namespace OSM {

class XMLReader {
public:

    bool read(std::istream &strm, Storage &doc ) ;
} ;

} // osm

#endif

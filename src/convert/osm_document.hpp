#ifndef __OSM_DOCUMENT_H__
#define __OSM_DOCUMENT_H__

#include <string>
#include <map>
#include <vector>
#include <deque>

#include "osm_storage.hpp"
#include "dictionary.hpp"

#include <unordered_map>

using std::string ;

namespace OSM {

class DocumentReader {
public:

    // empty document
    DocumentReader() {}

    // read OSM file into storage (format determined by extension)
    bool read(const std::string &fileName, Storage &storage) ;

protected:

    friend class XMLReader ;
    friend class PBFReader ;

    bool readXML(std::istream &strm) ;
    void writeXML(std::ostream &strm);

    bool readPBF(const std::string &fileName) ;
    bool isPBF(const std::string &fileName) ;
};


}

#endif

#ifndef __OSM_PBF_READER_HPP__
#define __OSM_PBF_READER_HPP__

#include "osm_document.hpp"
#include "convert/fileformat.pb.h"
#include "convert/osmformat.pb.h"

namespace OSM {

class PBFReader {
public:

    bool read(std::istream &, DocumentReader &doc) ;
private:

    bool process_osm_data_nodes(DocumentReader &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table, double lat_offset, double lon_offset, double granularity);
    bool process_osm_data_dense_nodes(DocumentReader &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table, double lat_offset, double lon_offset, double granularity);
    bool process_osm_data_ways(DocumentReader &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table);
    bool process_osm_data_relations(DocumentReader &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table);
} ;

} // osm

#endif

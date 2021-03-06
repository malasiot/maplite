#ifndef __OSM_PBF_READER_HPP__
#define __OSM_PBF_READER_HPP__

#include "osm_document.hpp"
#include "convert/fileformat.pb.h"
#include "convert/osmformat.pb.h"

namespace OSM {

class PBFReader {
public:

    bool read(std::istream &, Storage &doc) ;
private:

    bool process_osm_data_nodes(Storage &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table, double lat_offset, double lon_offset, double granularity);
    bool process_osm_data_dense_nodes(Storage &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table, double lat_offset, double lon_offset, double granularity);
    bool process_osm_data_ways(Storage &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table);
    bool process_osm_data_relations(Storage &doc, const PBF::PrimitiveGroup &group, const PBF::StringTable &string_table);
} ;

} // osm

#endif

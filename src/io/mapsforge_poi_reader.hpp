#ifndef MAPSFORGE_POI_READER_HPP
#define MAPSFORGE_POI_READER_HPP

#include <string>
#include <memory>
#include <fstream>
#include <map>
#include <vector>

#include "dictionary.hpp"
#include "database.hpp"
#include "geometry.hpp"
#include "mapsforge_poi_file_info.hpp"
#include "poi_categories.hpp"

#include <pugixml.hpp>
#include <boost/optional/optional.hpp>
#include <boost/enable_shared_from_this.hpp>

struct POIData {
    using Collection = std::vector<POIData> ;

    double lat_, lon_ ;
    Dictionary tags_ ;
};



class POICategoryFilter {
public:
    void addCategory(const POICategory::Ptr &category);

    POICategory::Collection getCategories() const ;
    POICategory::Collection getTopLevelCategories() const ;

private:

    std::set<POICategory::Ptr> white_list_ ;
} ;

class POIReader
{
public:

    POIReader() {}

    void open(const std::string &file_path) ;

    POIData::Collection query(const POICategoryFilter &filter, const BBox &bbox, const std::string &pattern, uint max_results = 10) ;

    ~POIReader() {}

    const POICategoryContainer &categories() const { return categories_ ; }

private:

    POICategoryContainer categories_ ;
    SQLite::Connection db_ ;
};



#endif



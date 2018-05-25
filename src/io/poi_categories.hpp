#ifndef POI_CATEGORIES_HPP
#define POI_CATEGORIES_HPP

#include <memory>
#include <vector>
#include <map>

#include "pugixml.hpp"
#include "database.hpp"

class POICategory {
public:

    using Ptr = std::shared_ptr<POICategory> ;
    using Collection = std::vector<Ptr> ;

    Collection descendants() const;
    Collection children() const { return children_ ; }

    std::string ID() const { return id_ ; }
    std::string title() const { return title_ ; }

    POICategory *parent() const { return parent_ ; }

private:

    friend class POICategoryContainer ;

    std::vector<Ptr> children_ ;
    POICategory *parent_ = nullptr ;
    std::string title_, id_ ;

};

class POICategoryContainer {
  public:

    bool loadFromXML(const std::string &path) ;
    bool loadFromXMLString(const std::string &str) ;

    bool loadFromPOIFile(SQLite::Connection &db) ;

    POICategory::Ptr getByID(const std::string &id) const ;

    const std::map<std::string, POICategory::Ptr> &categories() const { return categories_ ; }

private:

    bool parseCategory(POICategory::Ptr &c, const pugi::xml_node &root) ;

    POICategory::Ptr root_ ;
    std::map<std::string, POICategory::Ptr> categories_ ;
};






#endif

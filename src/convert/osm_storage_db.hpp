#ifndef __OSM_STORAGE_DB_HPP__
#define __OSM_STORAGE_DB_HPP__

#include "osm_storage.hpp"

#include "database.hpp"
#include <unordered_map>
#include <map>

namespace OSM {

class DBStorage: public Storage {
public:

    DBStorage(SQLite::Connection &db): db_(db) {
        initTables() ;
    }

    virtual ~DBStorage() override {}

    void writeNode(const Node &n) override ;

    void writeWay(const Way &way) override ;

    void writeBegin() override {
       db_.exec("BEGIN") ;
    }

    void writeEnd() override {
        db_.exec("COMMIT") ;
    }

    void writeRelation(const Relation &r ) override ;

    // readers

    bool readWay(osm_id_t id, Way &way) override ;

    void readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways) override {
        for( osm_id_t id: ids ) {
            auto it = ways_.find(id) ;
            if ( it != ways_.end() )
            ways.push_back(it->second) ;
        }
    }

    void readParentRelations(osm_id_t id, std::vector<Relation> &parents) override {
        auto pr = memberships_.equal_range(id) ;
        std::for_each(pr.first, pr.second, [this, &parents](const std::pair<osm_id_t, osm_id_t> &rp)  {
            auto it = relations_.find(rp.second) ;
            if ( it != relations_.end() ) {
                Relation rel ;
                rel.id_ = it->second.id_ ;
                rel.tags_ = it->second.tags_ ;
                parents.push_back(rel) ;
            }
        }) ;
    }

    void forAllWayCoords(osm_id_t wid, std::function<void(osm_id_t, double, double)> cf) override {
        auto it = ways_.find(wid) ;
        if ( it != ways_.end() ) {
            for( osm_id_t id: it->second.nodes_ ) {
                auto nit = nodes_.find(id) ;
                if ( nit != nodes_.end() ) {
                    const Node &n = nit->second ;
                    cf(n.id_, n.lat_, n.lon_) ;
                }
            }
        }
    }

    void forAllNodeCoordList(const std::vector<osm_id_t> &ids, std::function<void(double lat, double lon)> cf) override {
        for( auto &id: ids ) {
            auto nit = nodes_.find(id) ;
            if ( nit != nodes_.end() ) {
                const Node &n = nit->second ;
                cf(n.lat_, n.lon_) ;
            }
        }
    }

    void forAllNodes(std::function<void(const Node &)> cf) override ;

    void forAllWays(std::function<void(const Way &)> cf) override {
        for( const auto &wp: ways_ ) cf(wp.second) ;
    }

    void forAllRelations(std::function<void(const Relation &)> cf) override {
        for( const auto &rp: relations_ ) cf(rp.second) ;
    }

private:

    void initTables() ;

    std::string serializeTags(const Dictionary &tags) ;
    Dictionary decodeTags(const char *data, size_t sz);

    SQLite::Connection &db_ ;
    std::unordered_map<osm_id_t, Node> nodes_ ;
    std::unordered_map<osm_id_t, Way> ways_ ;
    std::unordered_map<osm_id_t, Relation> relations_ ;
    std::unordered_multimap<osm_id_t, osm_id_t> memberships_ ;

};

}
#endif

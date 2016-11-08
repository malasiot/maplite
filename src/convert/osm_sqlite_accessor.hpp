#ifndef __OSM_SQLITE_ACCESSOR_HPP__
#define __OSM_SQLITE_ACCESSOR_HPP__

#include "osm_accessor.hpp"
#include "database.hpp"

namespace OSM {

// serialize osm data to sqlite tables

class SQLiteAccessor: public DocumentAccessor {
public:
    SQLiteAccessor(std::shared_ptr<SQLite::Database> db): DocumentAccessor(),session_(db.get()),
        con_(session_.handle())
    {
        createTables() ;



    }
    virtual ~SQLiteAccessor() {}

    virtual bool readWay(osm_id_t id, Way &way) override ;

    virtual void readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways) override ;

    virtual void readWayNodes(osm_id_t ids, std::vector<Node> &nodes) override ;

    virtual void readParentRelations(osm_id_t id, std::vector<Relation> &parents) ;

    virtual void forAllNodes(std::function<void(const Node &)> cf) ;

    virtual void forAllWays(std::function<void(const Way &)> cf) ;

    virtual void forAllRelations(std::function<void(const Relation &)> cf) ;

    virtual void writeNode(const Node &n) override ;

    virtual void writeWay(const Way &way) ;

    virtual void writeRelation(const Relation &r ) override ;

    virtual void beginWrite() override ;
    virtual void endWrite() override ;

private:

    void createTables() ;

    SQLite::Session session_ ;
    SQLite::Connection &con_ ;
};

}

#endif

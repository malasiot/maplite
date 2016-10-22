#ifndef __TILE_CACHE_HPP__
#define __TILE_CACHE_HPP__

#include "tile_key.hpp"
#include "database.hpp"

// Tile cache stores tiles in SQlite database.

class PersistentTileCache {
public:

    PersistentTileCache(uint64_t max_capacity, uint64_t eviction_size_ = 1000000) ;

    bool open(const std::string &root_folder) ;
    void save(const std::string &key, const std::string &bytes, std::time_t t) ;
    std::string load(const std::string &key, std::time_t t) ;

private:

    void evict() ;

    uint64_t max_capacity_, capacity_, eviction_size_ ;
    std::unique_ptr<SQLite::Database> db_ ;
};



#endif

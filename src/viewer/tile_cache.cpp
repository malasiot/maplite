#include "tile_cache.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

namespace fs = boost::filesystem ;
using namespace std ;

PersistentTileCache::PersistentTileCache(uint64_t capacity, uint64_t es): max_capacity_(capacity), eviction_size_(es), capacity_(0)
{

}

bool PersistentTileCache::open(const string &root_folder)
{
    if ( !fs::exists(root_folder) && !fs::create_directory(root_folder) ) return false ;

    fs::path db_path(root_folder) ;
    db_path /= "cache.sqlite" ;

    bool create_tables = !fs::exists(db_path) ;

    db_.reset(new SQLite::Database(db_path.native())) ;

    try {
        if ( create_tables ) { // create database for the first time

            SQLite::Session session(db_.get()) ;
            SQLite::Connection &con = session.handle() ;

            con.exec("CREATE TABLE tiles (key TEXT PRIMARY KEY, state INTEGER DEFAULT 0, created INTEGER NOT NULL, size INTEGER NOT NULL, content BLOB NOT NULL);\
                     CREATE INDEX tile_idx ON tiles(key); PRAGMA journal_mode=WAL;PRAGMA synchronous=0") ;
        }
        else { //  on load determine the current capacity
            SQLite::Session session(db_.get()) ;
            SQLite::Connection &con = session.handle() ;

            SQLite::Query q(con, "SELECT SUM(size) FROM tiles;") ;
            SQLite::QueryResult res = q.exec() ;

            if ( res )
                capacity_ = res.get<int>(0) ;
        }
        return true ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

}

void PersistentTileCache::save(const std::string &key, const std::string &bytes, std::time_t t)
{
    if ( !db_ ) return ;
    if ( bytes.size() + capacity_ > max_capacity_ ) evict() ;

    try {
        SQLite::Session session(db_.get()) ;
        SQLite::Connection &con = session.handle() ;

        SQLite::Command cmd(con, "INSERT OR REPLACE INTO tiles (key,  created, size, content) VALUES (?, ?, ?, ?)") ;

        cmd.bind(1, key) ;
        cmd.bind(2, (int)t) ;
        cmd.bind(3, (int)bytes.size()) ;
        cmd.bind(4, (const char *)&bytes[0], bytes.size()) ;

        cmd.exec() ;

        capacity_ += bytes.size() ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }

}

void PersistentTileCache::evict() {
    if ( !db_ ) return ;

    const int fetch_limit = 10 ;
    int n_rows = fetch_limit ;
    int offset = 0 ;

    try {
        SQLite::Session session(db_.get()) ;
        SQLite::Connection &con = session.handle() ;

        SQLite::Query cmd(con, "SELECT size FROM tiles ORDER BY created LIMIT ? OFFSET ?") ;

        uint64_t sz = 0 ;
        while ( sz < eviction_size_ ) {

            cmd.bind(1, n_rows) ;
            cmd.bind(2, offset) ;

            SQLite::QueryResult res = cmd.exec() ;

            uint count = 0 ;
            while ( res ) {
                sz += res.get<int>(0) ;
                res.next() ;
                ++count ;
            }

            offset += count ;

            cmd.clear() ;

            if ( count < n_rows ) break ;
        }

        SQLite::Command dcmd(con, "DELETE from TILES WHERE ROWID IN (SELECT ROWID FROM tiles ORDER BY created LIMIT ?") ;
        dcmd.bind(1, offset) ;
        dcmd.exec() ;

    }
    catch ( SQLite::Exception &e ) {

    }
}

string PersistentTileCache::load(const std::string &key, std::time_t t)
{
    if ( !db_ ) return string();

    try {
        SQLite::Session session(db_.get()) ;
        SQLite::Connection &con = session.handle() ;

        SQLite::Query cmd(con, "SELECT content FROM tiles WHERE key=? AND created < ? AND state = 0") ;

        cmd.bind(1, key) ;
        cmd.bind(2, (int)t) ;

        SQLite::QueryResult res = cmd.exec() ;
        if ( res ) {
            int bs ;
            const char *blob = res.getBlob(0, bs) ;
            string s(blob, blob + bs) ;
            return s ;
        }

        return string() ;
    }
    catch ( SQLite::Exception &e ) {
        return string() ;
    }
}

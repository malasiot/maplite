#ifndef __RASTER_TILE_CACHE_H__
#define __RASTER_TILE_CACHE_H__

#include <cassert>
#include <list>
#include <map>

#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>

#include "raster_tile_source.hpp"
#include "logger.hpp"
#include "tuple_hash.hpp"

class RasterTileCache
{
public:

    typedef std::tuple<uint32_t, uint32_t, RasterTileSource *> key_type; // the tile is encoded by its index and a dataset id
    typedef RasterTileData value_type;

    // Key access history, most recent at back
    typedef std::list<key_type> key_tracker_type;

    // Key to value and key history iterator
    typedef std::unordered_map<key_type, std::pair<value_type, typename key_tracker_type::iterator> > key_to_value_type;

    // Constuctor specifies the maximum capacity (bytes)
    RasterTileCache( uint64_t capacity): capacity_(capacity), size_(0) {
        assert( capacity_ !=0 );
    }

    // Obtain value of the cached function for k
    value_type fetch(uint32_t ti, uint32_t tj, RasterTileSource *decoder )
    {
        std::lock_guard<std::mutex> guard(mtx_) ; // TODO: this effectively serializes request, make better syncronization

        key_type key(ti, tj, decoder) ;
        // Attempt to find existing record

        key_to_value_type::const_iterator it = key_to_value_.find(key) ;

        if ( it == key_to_value_.end() )  {
            LOG_TRACE_STREAM("loading new tile from source: " << ti << ' ' << tj ) ;
            value_type val = decoder->read_tile(ti, tj) ;
            if ( !val.data_ ) return val ;

            return insert(key, val);

        } else {

            key_tracker_.splice( key_tracker_.end(), key_tracker_,  (*it).second.second ) ;

            // Return the retrieved value
            return (*it).second.first ;
        }
    }


private:

    // Record a fresh key-value pair in the cache
    value_type insert(const key_type& k,const value_type& v) {

        // Method is only called on cache misses
        assert( key_to_value_.find(k) == key_to_value_.end() );

        uint64_t sz = v.stride_ * v.height_ ;
        // Make space if necessary
        if ( size_ +  sz > capacity_ ) evict();

        // Record k as most-recently-used key
        typename key_tracker_type::iterator it  =key_tracker_.insert(key_tracker_.end(), k);

        // Create the key-value entry,
        // linked to the usage record.

        key_to_value_.insert( std::make_pair( k, std::make_pair(std::move(v), it) )  );

        size_ += sz ;

        return key_to_value_[k].first ;

        // No need to check return,
        // given previous assert.
    }

    // Purge the least-recently-used element in the cache
    void evict() {

        // Assert method is never called when cache is empty
        assert(!key_tracker_.empty());

        // Identify least recently used key
        const typename key_to_value_type::iterator it  = key_to_value_.find(key_tracker_.front());
        assert( it != key_to_value_.end());

        size_ -= it->second.first.height_ * it->second.first.stride_ ;

        // Erase both elements to completely purge record
        key_to_value_.erase(it);
        key_tracker_.pop_front();

    }

    // Maximum bytes to retain

    const size_t capacity_ ;
    size_t size_ ;
    key_tracker_type key_tracker_;
    key_to_value_type key_to_value_ ;
    std::mutex mtx_ ;

};

#endif

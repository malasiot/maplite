#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <cassert>
#include <list>
#include <map>

#include <memory>
#include <functional>
#include <unordered_map>
#include <boost/thread/mutex.hpp>
#include <iostream>

#include "tuple_hash.hpp"

template< class key_type, class value_type >
class Cache
{
public:

    // Key access history, most recent at back
    typedef std::list<key_type> key_tracker_type;

    // Key to value and key history iterator
    typedef std::unordered_map<key_type, std::tuple<value_type, typename key_tracker_type::iterator, uint64_t> > key_to_value_type;

    // Constuctor specifies the maximum capacity (bytes)
    Cache( uint64_t capacity): capacity_(capacity), size_(0) {
        assert( capacity_ !=0 );
    }

    // Obtain value of the cached function for k
    // if not exists then it calls the loader function that should load data with the given key and return also data size

    bool fetch(const key_type &key, std::function<uint64_t(const key_type &, value_type &)> loader, value_type &val)
    {
        boost::mutex::scoped_lock lock(mutex_);

        // Attempt to find existing record

        typename key_to_value_type::const_iterator it = key_to_value_.find(key) ;

        if ( it == key_to_value_.end() )  {
            uint64_t data_sz = loader(key, val) ;
            insert(key, val, data_sz) ;
            return true ;
        }
        else {

            key_tracker_.splice( key_tracker_.end(), key_tracker_,  std::get<1>((*it).second) ) ;

            // Return the retrieved value
            val = std::get<0>((*it).second) ;
            return true ;
        }
    }

    // Record a fresh key-value pair in the cache
    value_type insert(const key_type& k,const value_type& v, uint64_t cost) {


        assert( key_to_value_.find(k) == key_to_value_.end() );

        uint64_t sz = cost ;
        // Make space if necessary
        if ( size_ +  sz > capacity_ ) evict();

        // Record k as most-recently-used key
        typename key_tracker_type::iterator it  =key_tracker_.insert(key_tracker_.end(), k);

        // Create the key-value entry,
        // linked to the usage record.

        key_to_value_.insert( std::make_pair( k, std::make_tuple(std::move(v), it, sz) )  );

        size_ += sz ;

        return std::get<0>(key_to_value_[k]) ;

    }

private:

    // Purge the least-recently-used element in the cache
    void evict() {

        // Assert method is never called when cache is empty
        assert(!key_tracker_.empty());

        if ( key_tracker_.empty() ) return ;

        // Identify least recently used key
        const typename key_to_value_type::iterator it  = key_to_value_.find(key_tracker_.front());
        assert( it != key_to_value_.end());

        size_ -= std::get<2>(it->second) ;

        // Erase both elements to completely purge record
        key_to_value_.erase(it);
        key_tracker_.pop_front();
    }



    // Maximum bytes to retain

    const size_t capacity_ ;
    size_t size_ ;
    key_tracker_type key_tracker_;
    key_to_value_type key_to_value_ ;
    boost::mutex mutex_ ;


};

#endif

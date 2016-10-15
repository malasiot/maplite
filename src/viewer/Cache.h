#ifndef __CACHE_H__
#define	__CACHE_H__

#include <boost/unordered_map.hpp>
#include <list>
#include <cstddef>
#include <stdexcept>

template<typename key_t, typename value_t, typename item_size_t = size_t>
class Cache {
public:
    typedef typename std::pair<key_t, std::pair<value_t, item_size_t> > key_value_pair_t;
    typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

    Cache(item_size_t max_size) :
        _max_size(max_size), _capacity(0) {
    }

    void put(const key_t& key, const value_t& value, const item_size_t &size) {
        typename cache_items_map_t::const_iterator it = _cache_items_map.find(key);
        if (it != _cache_items_map.end()) {
            _cache_items_list.erase(it->second);
            _cache_items_map.erase(it);
            const item_size_t &sz = (*it).second->second.second;
            _capacity -= sz ;
        }

        _capacity += size ;

        _cache_items_list.push_front(key_value_pair_t(key, std::make_pair(value, size)));
        _cache_items_map[key] = _cache_items_list.begin();

        while (_capacity > _max_size) {
            typename cache_items_list_t::const_iterator last = _cache_items_list.end();
            last--;
            const item_size_t &sz = (*last).second.second ;
            _cache_items_map.erase(last->first);
            _cache_items_list.pop_back();
            _capacity -= sz ;
        }
    }

    const value_t& get(const key_t& key) {
        typename cache_items_map_t::const_iterator it = _cache_items_map.find(key);
        if (it == _cache_items_map.end()) {
            throw std::range_error("There is no such key in cache");
        } else {
            _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
            return it->second->second.first;
        }
    }

    bool exists(const key_t& key) const {
        return _cache_items_map.find(key) != _cache_items_map.end();
    }

    size_t size() const {
        return _cache_items_map.size();
    }

private:

    typedef typename boost::unordered_map<key_t, list_iterator_t> cache_items_map_t ;
    typedef typename std::list<key_value_pair_t> cache_items_list_t ;

    cache_items_list_t _cache_items_list;
    cache_items_map_t _cache_items_map;
    item_size_t _max_size, _capacity;
};


#endif


#ifndef __TILE_CACHE_HPP__
#define __TILE_CACHE_HPP__

#include "tile_key.hpp"
#include <boost/filesystem.hpp>

// Tile cache stores tile images under folder using simple scheme <root_folder>/<map_key>/<zoom>/<x>/<y>.png
// Does not support any eviction mechanism

class FileSystemTileCache {
public:

    FileSystemTileCache(const boost::filesystem::path &root) ;

    void save(const std::string &key, const TileKey &tk, const std::string &bytes, std::time_t t) ;
    std::string load(const std::string &key, const TileKey &tk, std::time_t t) ;

private:

    boost::filesystem::path root_ ;
    boost::filesystem::path getValidPath(const std::string &key, const TileKey &tk);
};



#endif

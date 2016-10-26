#include "fs_tile_cache.hpp"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <fstream>

namespace fs = boost::filesystem ;
using namespace std ;

FileSystemTileCache::FileSystemTileCache(const fs::path &root): root_(root) {

    // if the root is empty or invalid then the cache is disabled

   if ( !root_.empty() ) {
       boost::system::error_code ec ;
       fs::create_directories(root, ec) ;
       if ( ec ) root_ = fs::path() ;
   }
}

static bool tile_is_newer(const fs::path &fname, time_t tile_time)
{
    if ( !fs::exists(fname) ) return true ; // if it does not exist then tile is newer
    std::time_t lt = fs::last_write_time(fname) ;
    return tile_time > lt ;
}

fs::path FileSystemTileCache::getValidPath(const string &key, const TileKey &tk) {
    fs::path out = root_ / key ;
    fs::create_directory(out) ; out /= str(boost::format("%d") % (int)tk.z()) ;
    fs::create_directory(out) ; out /= str(boost::format("%d") % (int)tk.x()) ;
    fs::create_directory(out) ; out /= str(boost::format("%d.png") % (int)tk.y());

    return out ;
}

void FileSystemTileCache::save(const string &key, const TileKey &tk, const string &bytes, std::time_t t)
{
    if ( root_.empty() ) return ;

    fs::path fp = getValidPath(key, tk) ;

    if  ( tile_is_newer(fp, t) ) {
        ofstream strm(fp.native().c_str(), ios::binary) ;
        strm.write(bytes.data(), bytes.size()) ;
        strm.close() ;
        fs::last_write_time(fp, t) ;
    }
}

string FileSystemTileCache::load(const std::string &key, const TileKey &tk, std::time_t t)
{
    if ( root_.empty() ) return string() ;

    fs::path fp = getValidPath(key, tk) ;

    if ( tile_is_newer(fp, t) ) // cached tile does not exist or is old
        return string() ;
    else {
        std::ifstream strm(fp.native().c_str());
        std::string out((std::istreambuf_iterator<char>(strm)),  std::istreambuf_iterator<char>());
        return std::move(out) ;
    }
}

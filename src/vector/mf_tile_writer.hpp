#ifndef __MBTILE_WRITER_H__
#define __MBTILE_WRITER_H__

#include "map_file.hpp"
#include <boost/filesystem.hpp>

class MBTileWriter {
public:
    MBTileWriter(const std::string &fileName) ;

    bool writeTilesDB(const MapFile &map, MapConfig &cfg) ;
    bool writeTilesFolder(const MapFile &map, MapConfig &cfg) ;

    bool writeMetaData(const std::string &name, const std::string &val);

private:

    std::unique_ptr<SQLite::Database> db_ ;
    boost::filesystem::path tileset_ ;
};




















#endif

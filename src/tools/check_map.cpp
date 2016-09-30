#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "renderer.hpp"
#include "tile_key.hpp"

using namespace std ;
namespace fs = boost::filesystem ;
using namespace mapsforge ;

void printUsageAndExit()
{
    cerr << "Usage: mf_check_map <map_file>" << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string mapFile ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( mapFile.empty() )
            mapFile = arg ;
    }

    if ( mapFile.empty()  )
        printUsageAndExit() ;

    std::shared_ptr<mapsforge::TileIndex> ti(new mapsforge::TileIndex(1000000)) ;
    MapFile reader(ti) ;

    reader.open(mapFile) ;

    TileKey key(4580, 5106, 13, false) ;

    VectorTile tile = reader.readTile(key);

    mapsforge::RenderTheme theme ;
    theme.read("/home/malasiot/Downloads/elevate4/Elevate.xml") ;

    Renderer r(theme) ;

    ImageBuffer buf(256, 256) ;



    r.render(buf, tile, key, theme.defaultLayer(), 128) ;

    buf.saveToPNG("/tmp/render.png") ;


}

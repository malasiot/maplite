#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "mapsforge_database.hpp"
#include "theme.hpp"
#include "renderer.hpp"
#include "tile_key.hpp"

using namespace std ;
namespace fs = boost::filesystem ;

void printUsageAndExit()
{
    cerr << "Usage: render_tile --map <map_file> --theme <map_theme> --layer <layer_name> --out <out_file> <tx> <ty> <tz> " << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string mapFile, mapTheme, mapLayer, outFile ;

    int i = 1;
    for( ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--map" ) {
            if ( ++i < argc )
                mapFile = argv[i] ;
        }
        else if ( arg == "--theme" ) {
            if ( ++i < argc )
                mapTheme = argv[i] ;
        }
        else if ( arg == "--layer" ) {
            if ( ++i < argc )
                mapLayer = argv[i] ;
        }
        else if ( arg == "--out" ) {
            if ( ++i < argc )
                outFile = argv[i] ;
        }
        else break ;
    }

    if ( i != argc - 3 || mapFile.empty() || mapTheme.empty() || outFile.empty() )
        printUsageAndExit() ;

    uint tx = stoi(argv[i++]) ;
    uint ty = stoi(argv[i++]) ;
    uint tz = stoi(argv[i++]) ;

    std::shared_ptr<TileIndex> ti(new TileIndex(1000000)) ;
    MapFile reader ;

    try {
        reader.open(mapFile, ti) ;
    }
    catch ( std::runtime_error &e ) {
        cerr << e.what() << endl ;
        exit(1) ;
    }

    TileKey key(tx, ty, tz, false) ;

    VectorTile tile = reader.readTile(key);

    RenderTheme theme ;
    if ( !theme.read(mapTheme) ) {
        cerr << "Can't read theme file: " << mapTheme << endl ;
        exit(1) ;
    }
    //theme.read("/home/malasiot/source/mftools/build/data/maps/greece/themes/elevate4/Elevate.xml") ;

    Renderer r(theme, "el", false) ;

    ImageBuffer buf(256, 256) ;

    r.render(key, buf, tile, mapLayer.empty() ? theme.defaultLayer() : mapLayer) ;

    buf.saveToPNG(outFile) ;

    return 0 ;
}

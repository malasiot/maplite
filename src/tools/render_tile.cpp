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

void printUsageAndExit()
{
    cerr << "Usage: render_tile --map <map_file> --theme <map_theme> --layer <layer_name> --out <out_file> <tx> <ty> <tz> " << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string map_file, map_theme, mapLayer, out_file ;

    int i = 1;
    for( ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--map" ) {
            if ( ++i < argc )
                map_file = argv[i] ;
        }
        else if ( arg == "--theme" ) {
            if ( ++i < argc )
                map_theme = argv[i] ;
        }
        else if ( arg == "--layer" ) {
            if ( ++i < argc )
                mapLayer = argv[i] ;
        }
        else if ( arg == "--out" ) {
            if ( ++i < argc )
                out_file = argv[i] ;
        }
        else break ;
    }

    if ( i != argc - 3 || map_file.empty() || map_theme.empty() || out_file.empty() )
        printUsageAndExit() ;

    uint tx = stoi(argv[i++]) ;
    uint ty = stoi(argv[i++]) ;
    uint tz = stoi(argv[i++]) ;

//    MapFileReader::initTileCache(100000) ;
    MapFileReader reader ;

    try {
        reader.open(map_file) ;
    }
    catch ( std::runtime_error &e ) {
        cerr << e.what() << endl ;
        exit(1) ;
    }

    TileKey key(tx, ty, tz, false) ;

    VectorTile tile = reader.readTile(key, 1);


    std::shared_ptr<RenderTheme> theme(new RenderTheme) ;
    if ( !theme->read(map_theme) ) {
        cerr << "Can't read theme file: " << map_theme << endl ;
        exit(1) ;
    }

    Renderer r(theme, "en", false) ;

    ImageBuffer buf(256, 256) ;

    r.render(key, buf, tile, mapLayer.empty() ? theme->defaultLayer() : mapLayer, 256) ;

    buf.saveToPNG(out_file) ;

    return 0 ;
}

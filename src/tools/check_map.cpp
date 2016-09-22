#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "mapsforge_map_reader.hpp"
#include "theme.hpp"
#include "renderer.hpp"

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
    VectorTile tile = reader.readTile(1145, 771, 11);

    mapsforge::RenderTheme theme ;
    theme.read("/home/malasiot/Downloads/elevate4/Elevate.xml") ;

    Renderer r(theme) ;

    ImageBuffer buf(256, 256) ;

    BBox bbox ;
    tms::tileLatLonBounds(1145, (1 << 11 ) - 771 - 1, 11, bbox.miny_, bbox.minx_, bbox.maxy_, bbox.maxx_) ;

    r.render(buf, tile, bbox, 11, theme.defaultLayer(), 128) ;

    buf.saveToPNG("/tmp/render.png") ;


}

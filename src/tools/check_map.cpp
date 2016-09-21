#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "mapsforge_map_reader.hpp"
#include "theme.hpp"

using namespace std ;
namespace fs = boost::filesystem ;


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
    mapsforge::MapFile reader(ti) ;

    reader.open(mapFile) ;
    reader.readTile(1145, 771, 11);
     reader.readTile(1144, 771, 11);
      reader.readTile(1145, 772, 11);

     mapsforge::RenderTheme theme ;
     theme.read("/home/malasiot/Downloads/elevate4/Elevate.xml") ;

}

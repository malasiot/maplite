#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "mapsforge_map_reader.hpp"

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

    mapsforge::MapFile reader ;

    reader.open(mapFile) ;
    reader.readTile(142, 95, 8);

}

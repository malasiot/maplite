#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

#include "mapsforge_map_reader.hpp"

using namespace std ;
namespace fs = boost::filesystem ;

void printUsageAndExit()
{
    cerr << "Usage: map_info map_file " << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string map_file ;

    if ( argc < 2 )
        printUsageAndExit() ;

    map_file = argv[1] ;

    MapFileReader reader ;

    try {
        reader.open(map_file) ;
    }
    catch ( std::runtime_error &e ) {
        cerr << e.what() << endl ;
        exit(1) ;
    }

    const MapFileInfo &info = reader.getMapFileInfo() ;

    std::time_t btime_ = info.date_/1000.0 ;



       std::cout.imbue(std::locale(std::cout.getloc(), new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S")));
       std::cout << boost::posix_time::from_time_t(btime_) << "\n";


       return 0 ;
}

#include <fstream>

#include "map_config.hpp"
#include "map_file.hpp"
#include "mf_tile_writer.hpp"

#include <boost/filesystem.hpp>

using namespace std ;

void printUsageAndExit()
{
    cerr << "Usage: osm2mbtiles --import <config_file> --options <options_file> --out <tileset> <file_name>+" << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string mapFile, mapConfigFile, importConfigFile, tileSet ;
    vector<string> osmFiles ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--import" ) {
            if ( i++ == argc ) printUsageAndExit() ;
            importConfigFile = argv[i] ;
        }
        else if ( arg == "--options" ) {
            if ( i++ == argc ) printUsageAndExit() ;
            mapConfigFile = argv[i] ;
        }
        else if ( arg == "--out" ) {
            if ( i++ == argc ) printUsageAndExit() ;
            tileSet = argv[i] ;
        }

        else
            osmFiles.push_back(argv[i]) ;
    }

    if ( importConfigFile.empty() ||   osmFiles.empty() )
        printUsageAndExit() ;

    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path() ;
    boost::filesystem::path tmp_file = boost::filesystem::unique_path("%%%%%.sqlite");

    mapFile = ( tmp_dir / tmp_file ).native() ;

    cout << mapFile << endl ;
    MapFile gfile ;

    if ( !gfile.create(mapFile) ) {
        cerr << "can't open map file: " << mapFile << endl ;
        exit(1) ;
    }

   FilterConfig icfg ;
    if ( !icfg.parse(importConfigFile) ) {
        cerr << "Error parsing OSM import configuration file: " << importConfigFile << endl ;
        return 0 ;
    }


    if ( !gfile.processOsmFiles(osmFiles, icfg) ) {
        cerr << "Error while creating temporary spatialite database" << endl ;
        return 0 ;
    }


    return 1 ;

}

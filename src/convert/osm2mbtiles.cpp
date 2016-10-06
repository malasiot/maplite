#include <fstream>

#include "osm_processor.hpp"


#include <boost/filesystem.hpp>

using namespace std ;

void printUsageAndExit()
{
    cerr << "Usage: osm2mbtiles --import <config_file> --options <options_file> --out <tileset> <file_name>+" << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string spatialiteDbFile, filterConfigFile, mapConfigFile, tileSet ;
    vector<string> osmFiles ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--filter" ) {
            if ( i++ == argc ) printUsageAndExit() ;
            filterConfigFile = argv[i] ;
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

    if (filterConfigFile.empty() || osmFiles.empty() )
        printUsageAndExit() ;

    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path() ;
    boost::filesystem::path tmp_file = boost::filesystem::unique_path("%%%%%.sqlite");

    spatialiteDbFile = ( tmp_dir / tmp_file ).native() ;

    cout << spatialiteDbFile << endl ;

    OSMProcessor proc ;

    if ( !proc.create(spatialiteDbFile) ) {
        cerr << "can't create temporary spatialite database: " << spatialiteDbFile << endl ;
        exit(1) ;
    }

   FilterConfig fcfg ;
    if ( !fcfg.parse(filterConfigFile) ) {
        cerr << "Error parsing OSM tag filter configuration file: " << filterConfigFile << endl ;
        return 0 ;
    }

    if ( !proc.processOsmFiles(osmFiles, fcfg) ) {
        cerr << "Error while populating temporary spatialite database" << endl ;
        return 0 ;
    }


    return 1 ;

}

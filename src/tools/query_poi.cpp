#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "mapsforge_poi_reader.hpp"

using namespace std ;
namespace fs = boost::filesystem ;

void printUsageAndExit()
{
    cerr << "Usage: render_tile --map <map_file> --theme <map_theme> --layer <layer_name> --out <out_file> <tx> <ty> <tz> " << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    POIReader reader ;

    try {
        reader.open("/home/malasiot/Downloads/greece.poi") ;
    }
    catch ( std::runtime_error &e ) {
        cerr << e.what() << endl ;
        exit(1) ;
    }

    POICategoryFilter filter ;

    filter.addCategory(reader.categories().getByID("147")) ;

    string target ;
    while ( 1 ) {
        char c = getchar() ;
        if ( c == '\n' ) continue ;
        if ( c == '\b' ) target.pop_back() ;
        else target += c ;

        cout << target << endl ;
        reader.query(filter, BBox(20.89, 30.55, 30.02, 45.65),target + '%');
    }



    /*
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

    POIReader reader ;

    try {
        reader.open(map_file) ;
    }
    catch ( std::runtime_error &e ) {
        cerr << e.what() << endl ;
        exit(1) ;
    }
*/
    return 0 ;
}

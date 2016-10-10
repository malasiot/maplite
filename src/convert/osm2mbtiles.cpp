#include <fstream>

#include "osm_processor.hpp"
#include "mapsforge_database.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std ;

void print_usage_and_exit()
{
    cerr << "Usage: osm2mbtiles --import <config_file> --options <options_file> --out <tileset> <file_name>+" << endl ;
    exit(1) ;
}

void init_map_file_info(int argc, char *argv[], SQLite::Database &db, MapFile &map) {

    bool has_bbox = false ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--bbox" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            vector<string> tokens;
            boost::split(tokens, argv[i], boost::is_any_of(" ,"), boost::token_compress_on);

            if ( tokens.size() < 4 ) print_usage_and_exit() ;

            try {
                float min_lat = boost::lexical_cast<float>(tokens[0]) ;
                float min_lon = boost::lexical_cast<float>(tokens[1]) ;
                float max_lat  = boost::lexical_cast<float>(tokens[2]) ;
                float max_lon  = boost::lexical_cast<float>(tokens[3]) ;

                map.setBoundingBox(min_lat, min_lon, max_lat, max_lon) ;
                has_bbox = true ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }
        else if ( arg == "--map-start-position" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            vector<string> tokens;
            boost::split(tokens, argv[i], boost::is_any_of(" ,"), boost::token_compress_on);

            if ( tokens.size() < 2 ) print_usage_and_exit() ;

            try {
                float lat = boost::lexical_cast<float>(tokens[0]) ;
                float lon = boost::lexical_cast<float>(tokens[1]) ;

                map.setStartPosition(lat, lon) ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }
        else if ( arg == "--map-start-zoom" ) {

            if ( i++ == argc ) print_usage_and_exit() ;
            try {
                uint8_t zoom = boost::lexical_cast<uint8_t>(argv[i]) ;

                map.setStartZoom(zoom) ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }
        else if ( arg == "--preferred-languages" ) {

            if ( i++ == argc ) print_usage_and_exit() ;

            map.setPreferredLanguages(argv[i]) ;
        }
        else if ( arg == "--comment" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            map.setComment(argv[i]) ;
        }
    }

    if ( !has_bbox )
        map.setBoundingBoxFromGeometries(db);

    map.setCreator("osm2mf") ;

}

int main(int argc, char *argv[])
{
    string spatialiteDbFile, filterConfigFile, mapConfigFile, tileSet ;
    vector<string> osmFiles ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--filter" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            filterConfigFile = argv[i] ;
        }
        else if ( arg == "--options" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            mapConfigFile = argv[i] ;
        }
        else if ( arg == "--out" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            tileSet = argv[i] ;
        }

        else
            osmFiles.push_back(argv[i]) ;
    }

    if (filterConfigFile.empty() || osmFiles.empty() )
        print_usage_and_exit() ;

    MapFile mf ;

    mf.create(tileSet) ;

    SQLite::Database db("/tmp/2ed94.sqlite") ;
   // SQLite::Database db("/tmp/0a907.sqlite") ;

    init_map_file_info(argc, argv, db, mf) ;

    WriteOptions options ;
    options.debug_ = true ;
    options.simplification_factor_ = 0 ;
    mf.write(db, options) ;

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

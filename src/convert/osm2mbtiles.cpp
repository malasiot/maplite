#include <fstream>

#include "osm_processor.hpp"
#include "mapsforge_map_writer.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std ;

void print_usage_and_exit()
{
    cerr << "Usage: osm2mbtiles [options] --filter <config_file> --out <map file name> <OSM files>+" << endl ;
    exit(1) ;
}


static void delete_args(int &argc, char *argv[], int idx, int nc) {

    for( uint i=idx+nc ; i<argc ; i++ ) argv[i-nc] = argv[i] ;

    for( uint i=argc-nc ; i<argc ; i++ ) argv[i] = 0 ;

    argc -= nc ;
}

void init_map_file_info(int &argc, char *argv[], MapFileWriter &map, bool &has_bounding_box) {

    has_bounding_box = false ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--bbox" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            vector<string> tokens;
            boost::split(tokens, argv[i], boost::is_any_of(" ,"), boost::token_compress_on);

            if ( tokens.size() < 4 ) print_usage_and_exit() ;

            try {
                double min_lat = boost::lexical_cast<float>(tokens[0]) ;
                double min_lon = boost::lexical_cast<float>(tokens[1]) ;
                double max_lat  = boost::lexical_cast<float>(tokens[2]) ;
                double max_lon  = boost::lexical_cast<float>(tokens[3]) ;

                map.setBoundingBox({min_lon, min_lat, max_lon, max_lat}) ;
                has_bounding_box = true ;

                delete_args(argc, argv, i-1, 2) ;

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

                delete_args(argc, argv, i-1, 2) ;
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

                delete_args(argc, argv, i-1, 2) ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }
        else if ( arg == "--preferred-languages" ) {

            if ( i++ == argc ) print_usage_and_exit() ;

            map.setPreferredLanguages(argv[i]) ;

            delete_args(argc, argv, i-1, 2) ;
        }
        else if ( arg == "--comment" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            map.setComment(argv[i]) ;

            delete_args(argc, argv, i-1, 2) ;
        }
    }

    map.setCreator("osm2mf") ;

}

void init_write_options(int argc, char *argv[], WriteOptions &options) {


    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--zoom-interval-conf" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            options.zoom_interval_conf_.clear() ;

            vector<string> tokens;
            boost::split(tokens, argv[i], boost::is_any_of(" ,"), boost::token_compress_on);

            if ( tokens.size() % 3 != 0 ) print_usage_and_exit() ;

            try {
                for( uint j=0 ; j<tokens.size() ; j++ )
                    options.zoom_interval_conf_.push_back(boost::lexical_cast<uint8_t>(tokens[j])) ;

                delete_args(argc, argv, i-1, 2) ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }
        else if ( arg == "--bbox-enlargement" ) {
            if ( i++ == argc ) print_usage_and_exit() ;

            try {
                options.bbox_enlargement_ = boost::lexical_cast<float>(argv[i]) ;
                delete_args(argc, argv, i-1, 2) ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }
        else if ( arg == "--debug" ) {
            options.debug_ = true ;
            delete_args(argc, argv, i, 1) ;
        }
        else if ( arg == "--label-positions" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            string arg = argv[i] ;
            if ( arg == "true" || arg == "1" ) options.label_positions_ = true ;
            else options.label_positions_ = false ;
            delete_args(argc, argv, i-1, 2) ;
        }
        else if ( arg == "--polygon-clipping" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            string arg = argv[i] ;
            if ( arg == "true" || arg == "1" ) options.polygon_clipping_ = true ;
            else options.polygon_clipping_ = false ;
            delete_args(argc, argv, i-1, 2) ;
        }
        else if ( arg == "--way-clipping" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            string arg = argv[i] ;
            if ( arg == "true" || arg == "1" ) options.way_clipping_ = true ;
            else options.way_clipping_ = false ;
            delete_args(argc, argv, i-1, 2) ;
        }
        else if ( arg == "--simplification-factor" ) {

            if ( i++ == argc ) print_usage_and_exit() ;

            try {
                options.simplification_factor_ = boost::lexical_cast<float>(argv[i]) ;
                delete_args(argc, argv, i-1, 2) ;
            }
            catch ( ... ) {
                print_usage_and_exit() ;
            }
        }

    }
}


int main(int argc, char *argv[])
{
    string spatialite_db_file, filter_config_file, map_config_file, out_map_file ;
    string land_shp_file ;
    bool has_bbox ;

    MapFileWriter writer ;
    WriteOptions woptions ;

    init_map_file_info(argc, argv, writer, has_bbox);
    init_write_options(argc, argv, woptions) ;

    vector<string> osm_files ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--filter" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            filter_config_file = argv[i] ;
        }
        else if ( arg == "--options" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            map_config_file = argv[i] ;
        }
        else if ( arg == "--land-polygon" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            land_shp_file = argv[i] ;
        }
        else if ( arg == "--out" ) {
            if ( i++ == argc ) print_usage_and_exit() ;
            out_map_file = argv[i] ;
        }
        else
            osm_files.push_back(argv[i]) ;
    }

    if (filter_config_file.empty() || osm_files.empty() )
        print_usage_and_exit() ;

    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path() ;
    boost::filesystem::path tmp_file = boost::filesystem::unique_path("%%%%%.sqlite");

    spatialite_db_file = ( tmp_dir / tmp_file ).native() ;

    cout << spatialite_db_file << endl ;

    OSMProcessor proc ;

    if ( !proc.create(spatialite_db_file) ) {
        cerr << "can't create temporary spatialite database: " << spatialite_db_file << endl ;
        exit(1) ;
    }

    FilterConfig fcfg ;
    if ( !fcfg.parse(filter_config_file) ) {
        cerr << "Error parsing OSM tag filter configuration file: " << filter_config_file << endl ;
        return 0 ;
    }
/*
    for( const string &fp: osm_files ) {
        if ( !proc.processOsmFile(fp, fcfg) ) {
            cerr << "Error while populating temporary spatialite database" << endl ;
            return 0 ;
        }
    }

   SQLite::Database &db = proc.handle() ;

    if ( !has_bbox )
         writer.setBoundingBox(proc.getBoundingBoxFromGeometries());

    BBox box = writer.getBoundingBox();

    if ( !land_shp_file.empty() )
        proc.processLandPolygon(land_shp_file, box) ;
    else
        proc.addDefaultLandPolygon(box) ;

*/
    writer.create(out_map_file) ;

    //SQLite::Database db("/tmp/2ed94.sqlite") ;
    // SQLite::Database db("/tmp/0a907.sqlite") ;
     SQLite::Database db("/tmp/a4581.sqlite") ;

    cout << "encoding file" << endl ;

    writer.write(db, woptions) ;

    return 1 ;

}

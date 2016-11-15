#include <fstream>

#include "osm_processor.hpp"
#include "mapsforge_map_writer.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

using namespace std ;
namespace po = boost::program_options ;

int main(int argc, char *argv[])
{
    string spatialite_db_file, filter_config_file, out_map_file ;
    string land_shp_file ;
    bool has_bbox ;
    vector<string> osm_files ;

    MapFileWriter writer ;
    WriteOptions woptions ;

    po::options_description desc;
    desc.add_options()
            ("help", "produce help")
            ("filter", po::value<string>(&filter_config_file)->required()->value_name("path"), "osm tag filter configuration file")
            ("land-polygon", po::value<string>(&land_shp_file)->value_name("path"), "shapefile containg the land enclosing polygon")
            ("out", po::value<string>(&out_map_file)->required()->value_name("path"), "output map file path")
            ("bbox", po::value<string>()->notifier(
                 [&writer, &has_bbox](const string &value) {
                    BBox box ;
                    if ( box.fromString(value)) {
                        writer.setBoundingBox(box);
                        has_bbox = true ;
                    }
                    else throw po::validation_error(po::validation_error::invalid_option_value) ;
            })->value_name("<minx miny maxx maxy>"), "map bounding box")
            ("map-start-position", po::value<string>()->notifier(
                 [&writer](const string &value) {
                    LatLon coords ;
                    if ( coords.fromString(value) ) writer.setStartPosition(coords.lat_, coords.lon_);
                    else throw po::validation_error(po::validation_error::invalid_option_value) ;
            })->value_name("<lat lon>"), "hint for coordinates of map center")
            ("map-start-zoom", po::value<uint>()->notifier( [&writer](const uint &value) { writer.setStartZoom(value); })->value_name("zoom"), "hint for map start zoom")
            ("preferred-languages", po::value<string>()->notifier( [&writer](const string &value) { writer.setPreferredLanguages(value); }), "map prefered languages in ISO")
            ("comment", po::value<string>()->notifier( [&writer](const string &value) { writer.setComment(value); }), "a comment to write to the file")
            ("zoom-interval-conf", po::value<vector<uint8_t>>(&woptions.zoom_interval_conf_), "a list of base_zoom, min_zoom, max_zoom for all levels of the map")
            ("bbox-enlargement", po::value<float>(&woptions.bbox_enlargement_), "amount in meters to expand each tile when adding geometries")
            ("debug", po::value<bool>(&woptions.debug_)->implicit_value(true), "enable debug information in the map file")
            ("label-positions", po::value<bool>(&woptions.label_positions_)->default_value(true, "true"), "enable or disable computation of label positions for areas covering multiple tiles")
            ("polygon-clipping", po::value<bool>(&woptions.polygon_clipping_)->default_value(true, "true"), "clip polygons to tile boundaries")
            ("way-clipping", po::value<bool>(&woptions.way_clipping_)->default_value(true, "true"), "clip ways to tile boundaries")
            ("simplification-factor", po::value<float>(&woptions.simplification_factor_)->default_value(2.5, "2.5"), "geometry simplification factor as error in pixels at each zoom level")
            ;

    po::options_description hidden;
    hidden.add_options()
            ("in", po::value<std::vector<std::string>>(&osm_files), "Input files")
    ;

    po::options_description all_options;
    all_options.add(desc);
    all_options.add(hidden);

    po::positional_options_description pd;
    pd.add("in", -1);

    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(all_options).positional(pd).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << "Usage: osm2map [options] <files>*\n";
            cout << desc << endl ;
            return 1;
        }
    }
    catch( po::error &e )
    {
        cerr << e.what() << endl ;
        cout << "Usage: osm2map [options] <files>*\n";
        cerr << desc << endl ;
        return 0;
    }

    writer.setCreator("osm2map") ;

    boost::filesystem::path tmp_dir = boost::filesystem::temp_directory_path() ;
    boost::filesystem::path tmp_file = boost::filesystem::unique_path("%%%%%.sqlite");

    spatialite_db_file = ( tmp_dir / tmp_file ).native() ;

    cout << spatialite_db_file << endl ;

    OSMProcessor proc ;
/*
    if ( !proc.create(spatialite_db_file) ) {
        cerr << "can't create temporary spatialite database: " << spatialite_db_file << endl ;
        exit(1) ;
    }
*/
    TagFilter filter ;
    if ( !filter.parse(filter_config_file) ) {
        cerr << "Error parsing OSM tag filter configuration file: " << filter_config_file << endl ;
        return 0 ;
    }

    for( const string &fp: osm_files ) {
        if ( !proc.processOsmFile(fp, filter) ) {
            cerr << "Error while populating temporary spatialite database" << endl ;
            return 0 ;
        }
    }

    GeoDatabase &db = proc.getDatabase() ;

    if ( !has_bbox )
        writer.setBoundingBox(proc.getBoundingBoxFromGeometries());

    BBox box = writer.getBoundingBox();

    if ( !land_shp_file.empty() )
        proc.processLandPolygon(land_shp_file, box) ;
    else
        proc.addDefaultLandPolygon(box) ;

    writer.create(out_map_file) ;

    //SQLite::Database db("/tmp/2ed94.sqlite") ;
    // SQLite::Database db("/tmp/0a907.sqlite") ;

    //     SQLite::Database db("/tmp/a157a.sqlite") ;

    cout << "encoding file" << endl ;

    writer.write(db, woptions) ;

    return 1 ;

}

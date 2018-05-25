#include <fstream>

#include "osm_processor.hpp"
#include "mapsforge_poi_writer.hpp"
#include "poi_categories.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

using namespace std ;
namespace po = boost::program_options ;

int main(int argc, char *argv[])
{
    string filter_config_file, out_poi_file, poi_mapping_file ;
    bool has_bbox = false ;
    vector<string> osm_files ;

    POIWriter writer ;
    POIWriteOptions woptions ;

    po::options_description desc;
    desc.add_options()
            ("help", "produce help")
            ("filter", po::value<string>(&filter_config_file)->required()->value_name("path"), "osm tag filter configuration file")
            ("poi-mapping", po::value<string>(&poi_mapping_file)->required()->value_name("path"), "poi mapping file")
            ("out", po::value<string>(&out_poi_file)->required()->value_name("path"), "output POI file path")
            ("bbox", po::value<string>()->notifier(
                 [&writer, &has_bbox](const string &value) {
                    BBox box ;
                    if ( box.fromString(value)) {
                        writer.setBoundingBox(box);
                        has_bbox = true ;
                    }
                    else throw po::validation_error(po::validation_error::invalid_option_value) ;
            })->value_name("<minx miny maxx maxy>"), "map bounding box")
            ("preferred-languages", po::value<string>()->notifier( [&writer](const string &value) { writer.setPreferredLanguages(value); }), "map prefered languages in ISO")
            ("comment", po::value<string>()->notifier( [&writer](const string &value) { writer.setComment(value); }), "a comment to write to the file")
            ("debug", po::value<bool>(&woptions.debug_)->implicit_value(true), "enable debug information in the map file")
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
            cout << "Usage: osm2poi [options] <files>*\n";
            cout << desc << endl ;
            return 1;
        }
    }
    catch( po::error &e )
    {
        cerr << e.what() << endl ;
        cout << "Usage: osm2poi [options] <files>*\n";
        cerr << desc << endl ;
        return 0;
    }

    writer.setCreator("osm2poi") ;
    writer.setDate(time(nullptr)) ;

    POICategoryContainer categories ;
    categories.loadFromXML(poi_mapping_file) ;

    OSMProcessor proc ;

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

    SQLite::Connection &db = proc.db() ;

    if ( !has_bbox )
        writer.setBoundingBox(proc.getBoundingBoxFromGeometries());

    BBox box = writer.getBoundingBox();


    writer.create(out_poi_file) ;

    //SQLite::Database db("/tmp/2ed94.sqlite") ;
    // SQLite::Database db("/tmp/0a907.sqlite") ;

    //     SQLite::Database db("/tmp/a157a.sqlite") ;

    cout << "encoding file" << endl ;

    writer.write(proc, categories, woptions) ;

    return 1 ;

}

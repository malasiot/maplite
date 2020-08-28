#include <fstream>

#include "osm_converter.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>


using namespace std ;
namespace po = boost::program_options ;

int main(int argc, char *argv[])
{


    string filter_tags, out_map_file ;
    bool has_bbox = false ;
    vector<string> osm_files ;

    po::options_description desc;
    desc.add_options()
            ("help", "produce help")
            ("tags", po::value<string>(&filter_tags)->required()->value_name("tags"), "osm tag filter configuration file")
            ("out", po::value<string>(&out_map_file)->required()->value_name("path"), "output sqlite file path")
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
            cout << "Usage: osm2sqlite [options] <files>*\n";
            cout << desc << endl ;
            return 1;
        }
    }
    catch( po::error &e )
    {
        cerr << e.what() << endl ;
        cout << "Usage: osm2sqlite [options] <files>*\n";
        cerr << desc << endl ;
        return 0;
    }

    OSMConverter conv(out_map_file, filter_tags) ;

    for( const string &fp: osm_files ) {
        if ( !conv.processOsmFile(fp) ) {
            cerr << "Error while populating temporary spatialite database" << endl ;
            return 0 ;
        }
    }


    return 1 ;

}

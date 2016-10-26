#include "server/map_server.hpp"

#include <thread>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std ;
namespace po = boost::program_options ;

#include "logger.hpp"

using namespace std ;

class DefaultLogger: public Logger
{
public:
    DefaultLogger(const std::string &log_file, bool debug) {
        if ( debug ) addAppender(make_shared<LogStreamAppender>(Trace, make_shared<LogPatternFormatter>("%In function %c, %F:%l: %m"), std::cerr)) ;
        if ( boost::filesystem::exists(log_file) )
            addAppender(make_shared<LogFileAppender>(Info, make_shared<LogPatternFormatter>("%V: %d %r: %m"), log_file)) ;
    }
};

std::unique_ptr<DefaultLogger> g_server_logger ;

Logger &get_current_logger() {
    return *g_server_logger ;
}

int main(int argc, char *argv[]) {
    string server_config_file, server_ports, log_file, tile_cache_folder, server_host_name ;
    bool debug = false ;
    po::options_description desc;
    desc.add_options()
            ("help", "produce help")
            ("config-file", po::value<string>(&server_config_file)->required()->value_name("path"), "server configuration file")
            ("host", po::value<string>(&server_host_name)->required()->value_name("host name")->default_value("127.0.0.1"), "server host name")
            ("port", po::value<string>(&server_ports)->value_name("port")->default_value("5000"), "ports to listen to")
            ("log-file", po::value<string>(&log_file)->value_name("path"), "path for message logging")
            ("cache-dir", po::value<string>(&tile_cache_folder)->value_name("folder"), "path for caching tiles (no caching if missing)")
            ("debug", po::value<bool>(&debug)->implicit_value(true), "log also to standard error")
            ;
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << "Usage: tile_server [options]\n";
            cout << desc << endl ;
            return 1;
        }
    }
    catch( po::error &e )
    {
        cerr << e.what() << endl ;
        cout << "Usage: tile_server [options]\n";
        cerr << desc << endl ;
        return 0;
    }

    g_server_logger.reset(new DefaultLogger(log_file, debug)) ;
    std::shared_ptr<MapServer> srv(new MapServer(server_config_file, server_host_name, server_ports, tile_cache_folder)) ;

    LOG_INFO("Starting server");

    std::thread t(&MapServer::run, srv.get()) ;

    t.join() ;
}

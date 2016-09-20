#include "server/map_server.hpp"

#include <thread>
#include "logger.hpp"

using namespace std ;

class DefaultLogger: public Logger
{
public:
    DefaultLogger() {
        addAppender(make_shared<LogStreamAppender>(Trace, make_shared<LogPatternFormatter>("%In function %c, %F:%l: %m"), std::cerr)) ;
        addAppender(make_shared<LogFileAppender>(Info, make_shared<LogPatternFormatter>("%V: %d %r: %m"), "/tmp/tileserver.log")) ;
    }
};


Logger &get_current_logger() {
    static DefaultLogger g_server_logger_ ;
    return g_server_logger_ ;
}

int main(int argc, char *argv[]) {
    std::shared_ptr<MapServer> srv(new MapServer("/home/malasiot/source/mbtools/build/data/", "5000")) ;

    LOG_INFO("Starting server");

    std::thread t(&MapServer::run, srv.get()) ;

    t.join() ;
}

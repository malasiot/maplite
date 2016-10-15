#include "progress_stream.hpp"

using namespace std ;

ConsoleProgressPrinter::ConsoleProgressPrinter(std::ostream &strm): ProgressStream(), strm_(strm)
{

}

void ConsoleProgressPrinter::beginTask(const string &msg, unsigned int total_steps)
{
    cur_msg_ = msg ;
    total_steps_ = total_steps ;
    steps_ = 0 ;
    last_tick_ = -1 ;

    strm_ << endl ;
    strm_ << msg << endl ;

}

void ConsoleProgressPrinter::advance(unsigned int steps)
{
    if ( last_tick_ == 40 ) return ;

    int tick = 40 * steps/(float)total_steps_ ;

    tick = std::min(40, std::max(0, tick)) ;

    if ( tick < last_tick_ ) return ;

    while( tick > last_tick_ )
    {
        last_tick_++ ;
        if ( last_tick_ % 4 == 0 )
            strm_ << (last_tick_/4) * 10 ;
        else
            strm_ << '.' ;
    }

    if ( last_tick_ == 40 )
    {
        strm_ << " - done.\n" ;
        last_tick_ == -1 ;
    }

    strm_.flush() ;

}



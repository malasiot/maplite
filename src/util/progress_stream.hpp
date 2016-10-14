#ifndef __PROGRESS_STREAM_H__
#define __PROGRESS_STREAM_H__

#include <string>
#include <iostream>


class ProgressStream {
public:
    ProgressStream() {}

    // start a progress indicator with given message and the total number of steps that have to be taken
    virtual void beginTask(const std::string &msg, unsigned int total_steps) {}
    // advance indicator by the given number of steps
    virtual void advance(int steps) {}
};

class ConsoleProgressPrinter:  public ProgressStream {
    public:

        ConsoleProgressPrinter(std::ostream &strm = std::cout ) ;

        void beginTask(const std::string &msg, unsigned int total_steps) ;
        void advance(unsigned int steps) ;

private:

        unsigned int total_steps_ ;
        unsigned int steps_ ;
        std::string cur_msg_ ;
        std::ostream &strm_ ;
        int last_tick_ ;
};



#endif

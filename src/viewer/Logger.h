#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

struct LogContext
{
    LogContext(const char *file_, int line_, const char *context_):
        file(file_), line(line_), context(context_), thread_id(0) {}

    int line ;
    std::string file ;
    std::string context ;
    int thread_id ;
} ;

enum LogLevel { Trace = 0, Debug = 1, Info = 2, Warning = 3, Error = 4, Fatal = 5 };

class LogFormatter {
public:

    LogFormatter() {}

    virtual std::string format(LogLevel level, const LogContext &ctx, const std::string &message) = 0 ;
};

class LogPatternFormatter: public LogFormatter
{
public:
    LogPatternFormatter(const std::string &pattern) ;

    /*
          The pattern is a format string with special
          flags:
                %v: log level
                %V: log level uppercase
                %c: function name
                %C: function name stripped
                %d{format}: Date with given format as given (default is ISO)
                %f: file path
                %F: file name
                %l: line number
                %r: time stamp (milliseconds from start of process)
                %t: thread ID
                %m: message string
                %%: prints %
           Optionally a format modifier may be inserted after %. This has the form [-][minLength][.maxLength]
           where - stand for left align (see log4j PatternLayout class documentation)
    */

    static const std::string DefaultFormat ;

protected:

    virtual std::string format(LogLevel level, const LogContext &ctx, const std::string &message) ;

private:

    std::string pattern_ ;
};

class LogSimpleFormatter: public LogFormatter {
public:
    LogSimpleFormatter() {}

protected:
    std::string format(LogLevel level, const LogContext &ctx, const std::string &message) {
        return message ;
    }
};

class LogAppender {

public:
    LogAppender(LogLevel levelThreshold, LogFormatter *formatter):
        threshold_(levelThreshold), formatter_(formatter) {
        assert(formatter_) ;
    }

    virtual ~LogAppender() {
        delete formatter_ ;
    }

    void setFormatter(LogFormatter *formatter) {
        delete formatter_ ;
        formatter_ = formatter ;
    }

    bool canAppend(LogLevel level) const {
        return level <= threshold_ ;
    }

protected:

    std::string formattedMessage(LogLevel level, const LogContext &ctx, const std::string &message) {
        return formatter_->format(level, ctx, message) ;
    }


    friend class Logger ;
    virtual void append(LogLevel level, const LogContext &ctx, const std::string &message) = 0;

private:

    LogLevel threshold_ ;
    LogFormatter *formatter_ ;
};

class LogStreamAppender: public LogAppender {
public:
    LogStreamAppender(LogLevel levelThreshold, LogFormatter *formatter, std::ostream &strm) ;
    ~LogStreamAppender() {
        strm_.flush() ;
    }

protected:

    virtual void append(LogLevel level, const LogContext &ctx, const std::string &message) ;

private:

    std::ostream &strm_ ;
};

class LogFileAppender: public LogAppender {
public:
    LogFileAppender(LogLevel levelThreshold, LogFormatter *formatter,
                    const std::string &file_prefix, // path of file to write messages
                    size_t maxFileSize = 1024*1024, // max size of file after which rotation happens
                    int maxBackupFileIndex = 100,   // maximum number of rotated files to keep
                    bool append_ = true) ;          // append messages to current file instead of starting a new record for a new instance of the appender
    ~LogFileAppender() ;

protected:

    virtual void append(LogLevel level, const LogContext &ctx, const std::string &message) ;

private:

    unsigned int max_file_size_ ;
    int fd_ ;
    bool append_ ;
    std::string fileName_ ;
    int last_backup_file_index_, max_backup_index_;
};

class Logger
{

public:

    Logger();
    ~Logger();

    // write a log message

    void write(LogLevel level, const LogContext &ctx, const char *format, ...) ;

    void addAppender(LogAppender *appender);

protected:

    friend class LoggerStream ;

    void write_(LogLevel level, const LogContext &ctx, const std::string &message) ;

    boost::mutex lock_ ;
    std::vector<LogAppender *> appenders_ ;
};

class DebugLogger: public Logger
{
public:
    DebugLogger() {
        addAppender(new LogStreamAppender(Debug, new LogPatternFormatter("%d{%c}-%r-(%t) %f %l %c: %m"), std::cout)) ;
    }

};

// Helper class for encapsulated a single formatted message and implement stream like log output

class LoggerStream
{
public:

    LoggerStream(Logger &logger, LogLevel level, const LogContext &ctx): logger_(logger),
    ctx_(ctx), level_(level) {}

    template <typename T>
    LoggerStream &operator << (const T& data)
    {
        messageBuffer << data ;
        return *this ;
    }

    ~LoggerStream()  {
        logger_.write_(level_, ctx_, messageBuffer.str()) ;
    }

private:

    std::ostringstream messageBuffer ;

    Logger &logger_ ;
    const LogContext &ctx_ ;
    LogLevel level_ ;

} ;


extern DebugLogger debugLogger ;

#define LOG_TRACE_STREAM() LoggerStream(debugLogger, Trace, LogContext(__FILE__, __LINE__, __FUNCTION__))
#define LOG_TRACE_STREAM_IF(condition) if ( !condition ) ; else LOG_TRACE_STREAM()
#define LOG_TRACE(format, ...) debugLogger.write(Trace, LogContext(__FILE__, __LINE__, __FUNCTION__), format, ##__VA_ARGS__)

#define LOG_DEBUG_STREAM() LoggerStream(debugLogger, Debug, LogContext(__FILE__, __LINE__, __FUNCTION__))
#define LOG_DEBUG_STREAM_IF(condition) if ( !condition ) ; else LOG_DEBUG_STREAM()
#define LOG_DEBUG(format, ...) debugLogger.write(Debug, LogContext(__FILE__, __LINE__, __FUNCTION__), format, ##__VA_ARGS__)

#endif

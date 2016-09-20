#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>

#include <thread>
#include <mutex>
#include <cassert>
#include <vector>
#include <sstream>

// Context of application logging passed with its log message to the logger

struct LogContext
{
    LogContext(const char *file_, int line_, const char *func_):
        file_(file_), line_(line_), function_(func_), thread_id_( 0 ) {}

    int line_ ;
    std::string function_ ;
    std::string file_ ;
    uint64_t thread_id_ ; // currently not implemented since there is no portable way of getting a numerical representation
} ;

enum LogLevel { Trace = 0, Debug = 1, Info = 2, Warning = 3, Error = 4, Fatal = 5 };

// Abstract formatter of messages

class LogFormatter {
public:

    LogFormatter() {}

    virtual std::string format(LogLevel level, const LogContext &ctx, const std::string &message) = 0 ;
};

// A logj style formater

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

// a simple formatter that disregards context and logging level

class LogSimpleFormatter: public LogFormatter {
public:
    LogSimpleFormatter() {}

protected:
    std::string format(LogLevel level, const LogContext &ctx, const std::string &message) {
        return message ;
    }
};

// An appender sends a message to a device such as console or file
class LogAppender {

public:
    LogAppender(LogLevel levelThreshold, const std::shared_ptr<LogFormatter> &formatter):
        threshold_(levelThreshold), formatter_(formatter) {
        assert(formatter_) ;
    }

    virtual ~LogAppender() {
    }

    void setFormatter(const std::shared_ptr<LogFormatter> &formater) {
        formatter_ = formater;
    }

    bool canAppend(LogLevel level) const {
        return level >= threshold_ ;
    }

protected:

    std::string formattedMessage(LogLevel level, const LogContext &ctx, const std::string &message) {
        return formatter_->format(level, ctx, message) ;
    }


    friend class Logger ;
    virtual void append(LogLevel level, const LogContext &ctx, const std::string &message) = 0;

private:

    LogLevel threshold_ ;
    std::shared_ptr<LogFormatter> formatter_ ;
};

// Append to a stream object
class LogStreamAppender: public LogAppender {
public:
    LogStreamAppender(LogLevel levelThreshold, const std::shared_ptr<LogFormatter> &formatter, std::ostream &strm) ;
    ~LogStreamAppender() {
        strm_.flush() ;
    }

protected:

    virtual void append(LogLevel level, const LogContext &ctx, const std::string &message) ;

private:

    std::ostream &strm_ ;
};

// Append to file

class LogFileAppender: public LogAppender {
public:
    LogFileAppender(LogLevel levelThreshold, const std::shared_ptr<LogFormatter> &formatter,
                    const std::string &file_prefix, // path of file to write messages
                    size_t max_file_size = 1024*1024, // max size of file after which rotation happens
                    int max_backup_file_index = 100,   // maximum number of rotated files to keep
                    bool append = true) ;          // append messages to current file instead of starting a new record for a new instance of the appender
    ~LogFileAppender() ;

protected:

    virtual void append(LogLevel level, const LogContext &ctx, const std::string &message) ;

private:

    unsigned int max_file_size_ ;
    int fd_ ;
    bool append_ ;
    std::string file_name_ ;
    int last_backup_file_index_, max_backup_index_;
};

// Main logger class. Forwards messages to appenders.

class Logger
{

public:
     // write a log message

    void write(LogLevel level, const LogContext &ctx, const char *format, ...) ;
    void addAppender(const std::shared_ptr<LogAppender> &appender);

protected:

    friend class LoggerStream ;

    void write_impl(LogLevel level, const LogContext &ctx, const std::string &message) ;

    std::mutex lock_ ;
    std::vector<std::shared_ptr<LogAppender>> appenders_ ;
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
        message_buffer_ << data ;
        return *this ;
    }

    ~LoggerStream()  {
        logger_.write_impl(level_, ctx_, message_buffer_.str()) ;
    }

private:

    std::ostringstream message_buffer_ ;

    Logger &logger_ ;
    const LogContext &ctx_ ;
    LogLevel level_ ;
} ;

// this should be defined per application to return the current logger object
extern Logger &get_current_logger() ;

#define LOG_X_STREAM(logger, level, msg) LoggerStream(logger, level, LogContext(__FILE__, __LINE__, __FUNCTION__)) << msg ;
#define LOG_X_STREAM_IF(logger, level, condition, msg) if ( ! (condition) ) ; else LOG_X_STREAM(logger, level, msg) ;
#define LOG_X_FORMAT(logger, level, format, ...) logger.write(level, LogContext(__FILE__, __LINE__, __FUNCTION__), format, ##__VA_ARGS__) ;
#define LOG_X_FORMAT_IF(logger, level, condition, format, ...) if ( !(condition)) ; else logger.write(level, LogContext(__FILE__, __LINE__, __FUNCTION__), format, ##__VA_ARGS__) ;

#define LOG_X_STREAM_EVERY_N(logger, level, n, msg)\
do {\
  static int _log_occurences = 0, _log_occurences_mod_n = 0; \
  ++_log_occurences; \
  if ( ++_log_occurences_mod_n > n) _log_occurences_mod_n -= n; \
  if ( _log_occurences_mod_n == 1 ) \
    LOG_X_STREAM(logger, level, msg) ;\
} while (0) ;
#define LOG_X_STREAM_ONCE(logger, level, msg)\
do {\
  static bool _logged_already = false; \
  if ( !_logged_already ) \
    LOG_X_STREAM(logger, level, msg) ;\
  _logged_already = true ;\
} while (0) ;
#define LOG_X_STREAM_FIRST_N(logger, level, n, msg)\
do {\
    static int _log_occurences = 0; \
    if ( _log_occurences <= n ) ++_log_occurences ; \
    if ( _log_occurences <= n ) \
    LOG_X_STREAM(logger, level, msg) ;\
} while (0) ;
#define LOG_X_FORMAT_EVERY_N(logger, level, n, format, ...)\
do {\
  static int _log_occurences = 0, _log_occurences_mod_n = 0; \
  ++_log_occurences; \
  if ( ++_log_occurences_mod_n > n) _log_occurences_mod_n -= n; \
  if ( _log_occurences_mod_n == 1 ) \
    LOG_X_FORMAT(logger, level, format, ##__VA_ARGS__) ;\
} while (0) ;
#define LOG_X_FORMAT_ONCE(logger, level, format, ...)\
do {\
  static bool _logged_already = false; \
  if ( !_logged_already ) \
    LOG_X_FORMAT(logger, level, format, ##__VA_ARGS__) ;\
  _logged_already = true ;\
} while (0) ;
#define LOG_X_FORMAT_FIRST_N(logger, level, n, format, ...)\
do {\
    static int _log_occurences = 0; \
    if ( _log_occurences <= n ) ++_log_occurences ; \
    if ( _log_occurences <= n ) \
    LOG_X_FORMAT(logger, level, format, ##__VA_ARGS__) ;\
} while (0) ;
#define LOG_X_STREAM_EVERY_N_IF(logger, level, n, condition, msg)\
    if ( ! ( condition ) ) ; else LOG_X_STREAM_EVERY_N(logger, level, n, msg) ;
#define LOG_X_STREAM_ONCE_IF(logger, level, condition, msg)\
    if ( ! ( condition ) ) ; else LOG_X_STREAM_ONCE(logger, level, msg) ;
#define LOG_X_STREAM_FIRST_N_IF(logger, level, n, condition, msg)\
    if ( ! ( condition ) ) ; else LOG_X_STREAM_FIRST_N(logger, level, n, msg) ;
#define LOG_X_FORMAT_EVERY_N_IF(logger, level, n, condition, msg)\
    if ( ! ( condition ) ) ; else LOG_X_FORMAT_EVERY_N(logger, level, n, msg) ;
#define LOG_X_FORMAT_ONCE_IF(logger, level, condition, msg)\
    if ( ! ( condition ) ) ; else LOG_X_FORMAT_ONCE(logger, level, msg) ;
#define LOG_X_FORMAT_FIRST_N_IF(logger, level, n, condition, msg)\
    if ( ! ( condition ) ) ; else LOG_X_FORMAT_FIRST_N(logger, level, n, msg) ;

#ifndef NO_DEBUG_LOGGING
#define LOG_TRACE_STREAM(msg) LOG_X_STREAM(get_current_logger(), Trace, msg)
#define LOG_TRACE_STREAM_IF(condition, msg) LOG_X_STREAM_IF(get_current_logger(), Trace, condition, msg)
#define LOG_TRACE(format, ...) LOG_X_FORMAT(get_current_logger(), Trace, format, ##__VA_ARGS__)
#define LOG_TRACE_IF(condition, format, ...) LOG_X_FORMAT_IF(get_current_logger(), Trace, condition, format, ##__VA_ARGS__)
#define LOG_TRACE_STREAM_EVERY_N(n, msg) LOG_X_STREAM_EVERY_N(get_current_logger(), Trace, n, msg)
#define LOG_TRACE_STREAM_ONCE(msg) LOG_X_STREAM_ONCE(get_current_logger(), Trace, msg)
#define LOG_TRACE_STREAM_FIRST_N(n, msg) LOG_X_STREAM_FIRST_N(get_current_logger(), Trace, n, msg)
#define LOG_TRACE_EVERY_N(n, msg, ...) LOG_X_FORMAT_EVERY_N(get_current_logger(), Trace, n, msg)
#define LOG_TRACE_ONCE(msg, ...) LOG_X_FORMAT_ONCE(get_current_logger(), Trace, msg)
#define LOG_TRACE_FIRST_N(n, msg, ...) LOG_X_FORMAT_FIRST_N(get_current_logger(), Trace, n, msg)
#define LOG_TRACE_STREAM_EVERY_N_IF(n, conditions, msg) LOG_X_STREAM_EVERY_N_IF(get_current_logger(), Trace, n, condition, msg)
#define LOG_TRACE_STREAM_ONCE_IF(condition, msg) LOG_X_STREAM_ONCE_IF(get_current_logger(), Trace, condition, msg)
#define LOG_TRACE_STREAM_FIRST_N_IF(n, condition, msg) LOG_X_STREAM_FIRST_N_IF(get_current_logger(), Trace, n, condition, msg)
#define LOG_TRACE_EVERY_N_IF(n, condition, msg, ...) LOG_X_FORMAT_EVERY_N_IF(get_current_logger(), Trace, n, condition, msg)
#define LOG_TRACE_ONCE_IF(condition, msg, ...) LOG_X_FORMAT_ONCE_IF(get_current_logger(), Trace, condition, msg)
#define LOG_TRACE_FIRST_N_IF(n, condition, msg, ...) LOG_X_FORMAT_FIRST_N_IF(get_current_logger(), Trace, n, condition, msg)

#define LOG_DEBUG_STREAM(msg) LOG_X_STREAM(get_current_logger(), Debug, msg)
#define LOG_DEBUG_STREAM_IF(condition, msg) LOG_X_STREAM_IF(get_current_logger(), Debug, condition, msg)
#define LOG_DEBUG(format, ...) LOG_X_FORMAT(get_current_logger(), Debug, format, ##__VA_ARGS__)
#define LOG_DEBUG_IF(condition, format, ...) LOG_X_FORMAT_IF(get_current_logger(), Debug, condition, format, ##__VA_ARGS__)
#define LOG_DEBUG_STREAM_EVERY_N(n, msg) LOG_X_STREAM_EVERY_N(get_current_logger(), Debug, n, msg)
#define LOG_DEBUG_STREAM_ONCE(msg) LOG_X_STREAM_ONCE(get_current_logger(), Debug, msg)
#define LOG_DEBUG_STREAM_FIRST_N(n, msg) LOG_X_STREAM_FIRST_N(get_current_logger(), Debug, n, msg)
#define LOG_DEBUG_EVERY_N(n, msg) LOG_X_FORMAT_EVERY_N(get_current_logger(), Debug, n, msg)
#define LOG_DEBUG_ONCE(msg) LOG_X_FORMAT_ONCE(get_current_logger(), Debug, msg)
#define LOG_DEBUG_FIRST_N(n, msg) LOG_X_FORMAT_FIRST_N(get_current_logger(), Debug, n, msg)
#define LOG_DEBUG_STREAM_EVERY_N_IF(n, conditions, msg) LOG_X_STREAM_EVERY_N_IF(get_current_logger(), Debug, n, condition, msg)
#define LOG_DEBUG_STREAM_ONCE_IF(condition, msg) LOG_X_STREAM_ONCE_IF(get_current_logger(), Debug, condition, msg)
#define LOG_DEBUG_STREAM_FIRST_N_IF(n, condition, msg) LOG_X_STREAM_FIRST_N_IF(get_current_logger(), Debug, n, condition, msg)
#define LOG_DEBUG_EVERY_N_IF(n, condition, msg) LOG_X_FORMAT_EVERY_N_IF(get_current_logger(), Debug, n, condition, msg)
#define LOG_DEBUG_ONCE_IF(condition, msg) LOG_X_FORMAT_ONCE_IF(get_current_logger(), Debug, condition, msg)
#define LOG_DEBUG_FIRST_N_IF(n, condition, msg) LOG_X_FORMAT_FIRST_N_IF(get_current_logger(), Debug, n, condition, msg)
#else // debug and trace messages are compiled out
#define LOG_TRACE_STREAM(msg)
#define LOG_TRACE_STREAM_IF(condition, msg)
#define LOG_TRACE(format, ...)
#define LOG_TRACE_IF(condition, format, ...)
#define LOG_TRACE_STREAM_EVERY_N(n, msg)
#define LOG_TRACE_STREAM_ONCE(msg)
#define LOG_TRACE_STREAM_FIRST_N(n, msg)
#define LOG_TRACE_EVERY_N(n, msg, ...)
#define LOG_TRACE_ONCE(msg, ...)
#define LOG_TRACE_FIRST_N(n, msg, ...)
#define LOG_TRACE_STREAM_EVERY_N_IF(n, conditions, msg)
#define LOG_TRACE_STREAM_ONCE_IF(condition, msg)
#define LOG_TRACE_STREAM_FIRST_N_IF(n, condition, msg)
#define LOG_TRACE_EVERY_N_IF(n, condition, msg, ...)
#define LOG_TRACE_ONCE_IF(condition, msg, ...)
#define LOG_TRACE_FIRST_N_IF(n, condition, msg, ...)

#define LOG_DEBUG_STREAM(msg)
#define LOG_DEBUG_STREAM_IF(condition, msg)
#define LOG_DEBUG(format, ...)
#define LOG_DEBUG_IF(condition, format, ...)
#define LOG_DEBUG_STREAM_EVERY_N(n, msg)
#define LOG_DEBUG_STREAM_ONCE(msg)
#define LOG_DEBUG_STREAM_FIRST_N(n, msg)
#define LOG_DEBUG_EVERY_N(n, msg, ...)
#define LOG_DEBUG_ONCE(msg, ...)
#define LOG_DEBUG_FIRST_N(n, msg, ...)
#define LOG_DEBUG_STREAM_EVERY_N_IF(n, conditions, msg)
#define LOG_DEBUG_STREAM_ONCE_IF(condition, msg)
#define LOG_DEBUG_STREAM_FIRST_N_IF(n, condition, msg)
#define LOG_DEBUG_EVERY_N_IF(n, condition, msg, ...)
#define LOG_DEBUG_ONCE_IF(condition, msg, ...)
#define LOG_DEBUG_FIRST_N_IF(n, condition, msg, ...)
#endif

#define LOG_INFO_STREAM(msg) LOG_X_STREAM(get_current_logger(), Info, msg)
#define LOG_INFO_STREAM_IF(condition, msg) LOG_X_STREAM_IF(get_current_logger(), Info, condition, msg)
#define LOG_INFO(format, ...) LOG_X_FORMAT(get_current_logger(), Info, format, ##__VA_ARGS__)
#define LOG_INFO_IF(condition, format, ...) LOG_X_FORMAT_IF(get_current_logger(), Info, condition, format, ##__VA_ARGS__)
#define LOG_INFO_STREAM_EVERY_N(n, msg) LOG_X_STREAM_EVERY_N(get_current_logger(), Info, n, msg)
#define LOG_INFO_STREAM_ONCE(msg) LOG_X_STREAM_ONCE(get_current_logger(), Info, msg)
#define LOG_INFO_STREAM_FIRST_N(n, msg) LOG_X_STREAM_FIRST_N(get_current_logger(), Info, n, msg)
#define LOG_INFO_EVERY_N(n, msg, ...) LOG_X_FORMAT_EVERY_N(get_current_logger(), Info, n, msg, ##__VA_ARGS__)
#define LOG_INFO_ONCE(msg, ...) LOG_X_FORMAT_ONCE(get_current_logger(), Info, msg, ##__VA_ARGS__)
#define LOG_INFO_FIRST_N(n, msg, ...) LOG_X_FORMAT_FIRST_N(get_current_logger(), Info, n, msg, ##__VA_ARGS__)
#define LOG_INFO_STREAM_EVERY_N_IF(n, conditions, msg) LOG_X_STREAM_EVERY_N_IF(get_current_logger(), Info, n, condition, msg)
#define LOG_INFO_STREAM_ONCE_IF(condition, msg) LOG_X_STREAM_ONCE_IF(get_current_logger(), Info, condition, msg)
#define LOG_INFO_STREAM_FIRST_N_IF(n, condition, msg) LOG_X_STREAM_FIRST_N_IF(get_current_logger(), Info, n, condition, msg)
#define LOG_INFO_EVERY_N_IF(n, condition, msg, ...) LOG_X_FORMAT_EVERY_N_IF(get_current_logger(), Info, n, condition, msg, ##__VA_ARGS__)
#define LOG_INFO_ONCE_IF(condition, msg, ...) LOG_X_FORMAT_ONCE_IF(get_current_logger(), Info, condition, msg, ##__VA_ARGS__)
#define LOG_INFO_FIRST_N_IF(n, condition, msg, ...) LOG_X_FORMAT_FIRST_N_IF(get_current_logger(), Info, n, condition, msg, ##__VA_ARGS__)

#define LOG_WARN_STREAM(msg) LOG_X_STREAM(get_current_logger(), Warning, msg)
#define LOG_WARN_STREAM_IF(condition, msg) LOG_X_STREAM_IF(get_current_logger(), Warning, condition, msg)
#define LOG_WARN(format, ...) LOG_X_FORMAT(get_current_logger(), Warning, format, ##__VA_ARGS__)
#define LOG_WARN_IF(condition, format, ...) LOG_X_FORMAT_IF(get_current_logger(), Warning, condition, format, ##__VA_ARGS__)
#define LOG_WARN_STREAM_EVERY_N(n, msg) LOG_X_STREAM_EVERY_N(get_current_logger(), Warning, n, msg)
#define LOG_WARN_STREAM_ONCE(msg) LOG_X_STREAM_ONCE(get_current_logger(), Warning, msg)
#define LOG_WARN_STREAM_FIRST_N(n, msg) LOG_X_STREAM_FIRST_N(get_current_logger(), Warning, n, msg)
#define LOG_WARN_EVERY_N(n, msg, ...) LOG_X_FORMAT_EVERY_N(get_current_logger(), Warning, n, msg)
#define LOG_WARN_ONCE(msg, ...) LOG_X_FORMAT_ONCE(get_current_logger(), Warning, msg, ##__VA_ARGS__)
#define LOG_WARN_FIRST_N(n, msg, ...) LOG_X_FORMAT_FIRST_N(get_current_logger(), Warning, n, msg, ##__VA_ARGS__)
#define LOG_WARN_STREAM_EVERY_N_IF(n, conditions, msg) LOG_X_STREAM_EVERY_N_IF(get_current_logger(), Warning, n, condition, msg)
#define LOG_WARN_STREAM_ONCE_IF(condition, msg) LOG_X_STREAM_ONCE_IF(get_current_logger(), Warning, condition, msg)
#define LOG_WARN_STREAM_FIRST_N_IF(n, condition, msg) LOG_X_STREAM_FIRST_N_IF(get_current_logger(), Warning, n, condition, msg)
#define LOG_WARN_EVERY_N_IF(n, condition, msg, ...) LOG_X_FORMAT_EVERY_N_IF(get_current_logger(), Warning, n, condition, msg, ##__VA_ARGS__)
#define LOG_WARN_ONCE_IF(condition, msg, ...) LOG_X_FORMAT_ONCE_IF(get_current_logger(), Warning, condition, msg, ##__VA_ARGS__)
#define LOG_WARN_FIRST_N_IF(n, condition, msg, ...) LOG_X_FORMAT_FIRST_N_IF(get_current_logger(), Warning, n, condition, msg, ##__VA_ARGS__)

#define LOG_ERROR_STREAM(msg) LOG_X_STREAM(get_current_logger(), Error, msg)
#define LOG_ERROR_STREAM_IF(condition, msg) LOG_X_STREAM_IF(get_current_logger(), Error, condition, msg)
#define LOG_ERROR(format, ...) LOG_X_FORMAT(get_current_logger(), Error, format, ##__VA_ARGS__)
#define LOG_ERROR_IF(condition, format, ...) LOG_X_FORMAT_IF(get_current_logger(), Error, condition, format, ##__VA_ARGS__)
#define LOG_ERROR_STREAM_EVERY_N(n, msg) LOG_X_STREAM_EVERY_N(get_current_logger(), Error, n, msg)
#define LOG_ERROR_STREAM_ONCE(msg) LOG_X_STREAM_ONCE(get_current_logger(), Error, msg)
#define LOG_ERROR_STREAM_FIRST_N(n, msg) LOG_X_STREAM_FIRST_N(get_current_logger(), Error, n, msg)
#define LOG_ERROR_EVERY_N(n, msg, ...) LOG_X_FORMAT_EVERY_N(get_current_logger(), Error, n, msg, ##__VA_ARGS__)
#define LOG_ERROR_ONCE(msg, ...) LOG_X_FORMAT_ONCE(get_current_logger(), Error, msg, ##__VA_ARGS__)
#define LOG_ERROR_FIRST_N(n, msg, ...) LOG_X_FORMAT_FIRST_N(get_current_logger(), Error, n, msg, ##__VA_ARGS__)
#define LOG_ERROR_STREAM_EVERY_N_IF(n, conditions, msg) LOG_X_STREAM_EVERY_N_IF(get_current_logger(), Error, n, condition, msg)
#define LOG_ERROR_STREAM_ONCE_IF(condition, msg) LOG_X_STREAM_ONCE_IF(get_current_logger(), Error, condition, msg)
#define LOG_ERROR_STREAM_FIRST_N_IF(n, condition, msg) LOG_X_STREAM_FIRST_N_IF(get_current_logger(), Error, n, condition, msg)
#define LOG_ERROR_EVERY_N_IF(n, condition, msg, ...) LOG_X_FORMAT_EVERY_N_IF(get_current_logger(), Error, n, condition, msg, ##__VA_ARGS__)
#define LOG_ERROR_ONCE_IF(condition, msg, ...) LOG_X_FORMAT_ONCE_IF(get_current_logger(), Error, condition, msg, ##__VA_ARGS__)
#define LOG_ERROR_FIRST_N_IF(n, condition, msg, ...) LOG_X_FORMAT_FIRST_N_IF(get_current_logger(), Error, n, condition, msg, ##__VA_ARGS__)

#define LOG_FATAL_STREAM(msg) LOG_X_STREAM(get_current_logger(), Fatal, msg)
#define LOG_FATAL_STREAM_IF(condition, msg) LOG_X_STREAM_IF(get_current_logger(), Fatal, condition, msg)
#define LOG_FATAL(format, ...) LOG_X_FORMAT(get_current_logger(), Fatal, format, ##__VA_ARGS__)
#define LOG_FATAL_IF(condition, format, ...) LOG_X_FORMAT_IF(get_current_logger(), Fatal, condition, format, ##__VA_ARGS__)
#define LOG_FATAL_STREAM_EVERY_N(n, msg) LOG_X_STREAM_EVERY_N(get_current_logger(), Fatal, n, msg)
#define LOG_FATAL_STREAM_ONCE(msg) LOG_X_STREAM_ONCE(get_current_logger(), Fatal, msg)
#define LOG_FATAL_STREAM_FIRST_N(n, msg) LOG_X_STREAM_FIRST_N(get_current_logger(), Fatal, n, msg)
#define LOG_FATAL_EVERY_N(n, msg, ...) LOG_X_FORMAT_EVERY_N(get_current_logger(), Fatal, n, msg, ##__VA_ARGS__)
#define LOG_FATAL_ONCE(msg, ...) LOG_X_FORMAT_ONCE(get_current_logger(), Fatal, msg, ##__VA_ARGS__)
#define LOG_FATAL_FIRST_N(n, msg, ...) LOG_X_FORMAT_FIRST_N(get_current_logger(), Fatal, n, msg, ##__VA_ARGS__)
#define LOG_FATAL_STREAM_EVERY_N_IF(n, conditions, msg) LOG_X_STREAM_EVERY_N_IF(get_current_logger(), Fatal, n, condition, msg)
#define LOG_FATAL_STREAM_ONCE_IF(condition, msg) LOG_X_STREAM_ONCE_IF(get_current_logger(), Fatal, condition, msg)
#define LOG_FATAL_STREAM_FIRST_N_IF(n, condition, msg) LOG_X_STREAM_FIRST_N_IF(get_current_logger(), Fatal, n, condition, msg)
#define LOG_FATAL_EVERY_N_IF(n, condition, msg, ...) LOG_X_FORMAT_EVERY_N_IF(get_current_logger(), Fatal, n, condition, msg, ##__VA_ARGS__)
#define LOG_FATAL_ONCE_IF(condition, msg, ...) LOG_X_FORMAT_ONCE_IF(get_current_logger(), Fatal, condition, msg, ##__VA_ARGS__)
#define LOG_FATAL_FIRST_N_IF(n, condition, msg, ...) LOG_X_FORMAT_FIRST_N_IF(get_current_logger(), Fatal, n, condition, msg, ##__VA_ARGS__)


#endif

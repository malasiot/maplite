#include "Logger.h"

#include <iostream>

#include <limits.h>
#include <stdio.h>

#include <cstdarg>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <fcntl.h>
#include <cstdio>
#include <zlib.h>

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif


using namespace std;

DebugLogger debugLogger ;

const std::string LogPatternFormatter::DefaultFormat = "%d{%c}-%r-(%t) %f %l %c:";

static string levelToString(LogLevel ltype)
{
    switch (ltype)
    {
        case Error:
            return "Error";
        case Warning:
            return "Warning";
        case Debug:
            return "Debug";
        case Info:
            return "Info";
        case Trace:
            return "Trace";
        default:
            return string();
    }
}

#define LITERAL_STATE 0
#define CONVERTER_STATE 1
#define DOT_STATE 2
#define MIN_STATE 3
#define MAX_STATE 4
#define COMMAND_STATE 5

static void printFormatedString(std::ostream &strm, const string &buffer, bool leftAlign, int minLength, int maxLength)
{
    int rawLength = buffer.length() ;

    if ( rawLength > maxLength ) {
        strm.write(buffer.c_str() + rawLength - maxLength, maxLength) ;
    } else if (rawLength < minLength) {
        if ( leftAlign ) {
            strm.write(buffer.c_str(), rawLength) ;
            for(int i=0 ; i<minLength - rawLength ; i++ ) strm.put(' ') ;
        } else {

            for(int i=0 ; i<minLength - rawLength ; i++ ) strm.put(' ') ;
            strm.write(buffer.c_str(), rawLength) ;
        }
    }
    else strm.write(buffer.c_str(), rawLength) ;

}

static void formatMessage(std::ostream &strm, const string &pattern, LogLevel level, const string &message, int threadId, const string &fileName = string(), int currentLine = 0,
                               const string &context = string())

{
    int state = LITERAL_STATE;
    int i = 0;
    int patternLength = pattern.length() ;
    bool leftAlign ;
    int minLength, maxLength ;

    while ( i < patternLength )
    {
        char c = pattern[i++];

        switch (state)
        {
            case LITERAL_STATE:
                leftAlign = false ;
                minLength = 0 ;
                maxLength = INT_MAX ;

                // In literal state, the last char is always a literal.
                if (i == patternLength) {
                    strm.put(c) ;
                    continue;
                }

                if ( c == '%' ) {
                    // peek at the next char.
                    if ( pattern[i] == '%' ) {
                        strm.put(c) ;
                        i++;
                    } else {
                        state = CONVERTER_STATE;
                    }
                } else {
                    strm.put(c);
                }

                break;

            case CONVERTER_STATE:
                switch (c)
                {
                    case  '-':
                        leftAlign = true ;
                        break;
                    case '.':
                        state = DOT_STATE;
                        break;
                    default:
                        if ((c >= '0') && (c <= '9'))
                        {
                            state = MIN_STATE ;
                            minLength = c - '0' ;
                        } else {
                            --i ;
                            state = COMMAND_STATE;
                        }
                }

                break;

            case MIN_STATE:
                if ((c >= '0') && (c <= '9'))
                {
                    minLength = minLength * 10 + c - '0' ;
                }
                else if (c == '.')
                {
                    state = DOT_STATE;
                } else {
                    --i ;
                    state = COMMAND_STATE;
                }

                break;

            case DOT_STATE:
                if ((c >= '0') && (c <= '9'))
                {
                    maxLength = c  - '0' ;
                    state = MAX_STATE;
                } else {
                    //  Error in pattern, was expecting digit.";
                    strm.put(c) ;
                    state = LITERAL_STATE;
                }
                break;

            case MAX_STATE:
                if ((c >= '0') && (c <= '9')) {
                    maxLength = maxLength * 10 + c - '0' ;
                }
                else {
                    --i ;
                    state = COMMAND_STATE;
                }
                break;

            case COMMAND_STATE:
                switch (c)
                {
                case 'v': //log level
                {
                    string levelStr = levelToString(level) ;
                    printFormatedString(strm, levelStr, leftAlign, minLength, maxLength) ;
                    break ;
                }
                case 'V': //log level uppercase
                {
                    string levelStr = levelToString(level) ;
                    printFormatedString(strm, boost::to_upper_copy(levelStr), leftAlign, minLength, maxLength) ;
                    break ;
                }
                case 'c': //function name
                    if ( !context.empty() )
                            printFormatedString(strm, context, leftAlign, minLength, maxLength) ;
                            break ;
                case 'f': // %f: file path
                    if ( !fileName.empty() )
                        printFormatedString(strm, fileName, leftAlign, minLength, maxLength) ;
                    break ;
                case 'F': // %F: file name
                    if ( !fileName.empty() )
                    {
                        string file_name_ = boost::filesystem::path(fileName).filename().string() ;
                        printFormatedString(strm, file_name_, leftAlign, minLength, maxLength) ;
                    }
                    break ;

                case 'l': // %l: line number
                    {
                        if ( currentLine )
                        {
                            std::ostringstream sb ;
                            sb << currentLine ;
                            printFormatedString(strm, sb.str().c_str(), leftAlign, minLength, maxLength) ;
                        }
                    }
                    break ;
                    case 'd': //%d{format}: Date with given format as given (default is %H:%M:%S)
                        {
                            struct tm *newtime ;
                            time_t ltime ;
                            time(&ltime) ;

                            //newtime = gmtime( &ltime ); // C4996
                            newtime = localtime(&ltime) ;

                            char buf[80] ;
                            int bufLen ;

                            if ( pattern[i] == '{' )
                            {
                                char format[80], *p = format ;

                                while ( pattern[++i] != '}' ) *p++ = pattern[i] ;
                                *p = 0 ; ++i ;

                                bufLen = strftime(buf, 80, format, newtime) ;

                            }
                            else
                                bufLen = strftime(buf, 80, "%H:%M:%S", newtime) ;

                            strm.write(buf, bufLen) ;


                        }

                        break ;

                    case 'r': // time stamp
                        {
                            unsigned long cc = clock() * 1000.0 / CLOCKS_PER_SEC ;

                            std::ostringstream sb ;
                            sb << cc ;
                            printFormatedString(strm, sb.str(), leftAlign, minLength, maxLength) ;
                        }
                        break ;
                    case 't': // thread ID
                        {
                            unsigned long cc = threadId ;

                            std::ostringstream sb ;
                            sb << cc ;
                            printFormatedString(strm, sb.str(), leftAlign, minLength, maxLength) ;
                        }
                        break ;
                    case 'm': // message
                        printFormatedString(strm, message, leftAlign, minLength, maxLength) ;
                        break ;

                    default:
                        //  Error in pattern, invalid command specifier ;
                        strm.put(c) ;
                        state = LITERAL_STATE;
                        continue ;

                }
                state = LITERAL_STATE ;
                break ;
        } // switch
    }


}

LogPatternFormatter::LogPatternFormatter(const string &pattern): LogFormatter(), pattern_(pattern) {

}

string LogPatternFormatter::format(LogLevel level, const LogContext &ctx, const string &message)
{
    stringstream strm ;

    formatMessage(strm, pattern_, level, message, ctx.thread_id, ctx.file, ctx.line, ctx.context) ;

    return strm.str() ;
}

////////////////////////////////////////////////////////////////////////////////
//  Logger
////////////////////////////////////////////////////////////////////////////////

Logger::Logger() {}

Logger::~Logger()
{
    for( int i=0 ; i<appenders_.size() ; i++ ) delete appenders_[i] ;
}


void Logger::write(LogLevel level, const LogContext &ctx, const char *format, ...)
{
    va_list vl;
    va_start(vl, format);

    char buffer[1024] ;
    int nc = vsnprintf(buffer, 1024, format, vl) ;

    write_(level, ctx, buffer) ;
}

void Logger::addAppender(LogAppender *appender)
{
    boost::mutex::scoped_lock lock(lock_) ;

    appenders_.push_back(appender) ;
}

void Logger::write_(LogLevel level, const LogContext &ctx, const string &message)
{
    boost::mutex::scoped_lock lock(lock_) ;

    for(int i=0 ; i<appenders_.size() ; i++)
        appenders_[i]->append(level, ctx, message) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

LogStreamAppender::LogStreamAppender(LogLevel levelThreshold, LogFormatter *formatter, ostream &strm):
    LogAppender(levelThreshold, formatter), strm_(strm) {

}

void LogStreamAppender::append(LogLevel level, const LogContext &ctx, const string &message)
{
    if ( canAppend(level) )
        strm_ << formattedMessage(level, ctx, message) << endl ;
}

#define MAX_BACKUP_INDEX 20

LogFileAppender::LogFileAppender(LogLevel levelThreshold, LogFormatter *formatter,
                                 const string &fileName, size_t maxFileSize, int maxBackupIndex, bool _append): LogAppender(levelThreshold, formatter), fileName_(fileName),
    max_file_size_(maxFileSize), max_backup_index_(maxBackupIndex), append_(_append)
{
    unsigned int flags = O_CREAT | O_APPEND | O_WRONLY ;
    if ( !append_ ) flags |= O_TRUNC;

    fd_ = ::open(fileName.c_str(), flags, 00644);

    // find the index of the last backup file

    int last ;
    for( last=0 ; last <= MAX_BACKUP_INDEX ; last++ )
    {
        string log_file = str(boost::format("%s.%d.gz") % fileName_.c_str() % (last+1) ) ;
        if ( !boost::filesystem::exists(log_file) ) break ;
    }

    last_backup_file_index_ = last ;

}

LogFileAppender::~LogFileAppender()
{
    ::close(fd_) ;
}


#define CHUNK 16384

static bool compress(const string &srcFile, const string &outFile)
{
    FILE *source = fopen(srcFile.c_str(), "r") ;
    gzFile dest = gzopen(outFile.c_str(),"wb");

    unsigned char in[CHUNK] ;
    int avail_in = CHUNK ;

    do {
        avail_in = fread(in, 1, CHUNK, source) ;

        if ( ferror(source) || gzwrite(dest, (void *)in, avail_in) != avail_in ) {
            fclose(source) ;
            gzclose(dest) ;
            return false;
        }
    } while ( avail_in == CHUNK ) ;

    gzflush(dest, Z_FINISH ) ;

    /* clean up and return */
    fclose(source) ;
    gzclose(dest) ;

    return true ;
}

void LogFileAppender::append(LogLevel level, const LogContext &ctx, const string &message)
{
    if ( !canAppend(level) ) return ;

    string s = formattedMessage(level, ctx, message) + '\n';
    ::write(fd_, s.data(), s.length()) ;

    off_t offset = ::lseek(fd_, 0, SEEK_END);
    if ( offset < 0 || static_cast<size_t>(offset) < max_file_size_ ) return ;

    // backup current file and open a new one

    ::close(fd_);

    // remove last file if too many backup files

    string last_log_file = str(boost::format("%s.%d.gz") % fileName_.c_str() % (last_backup_file_index_) ) ;

    if ( last_backup_file_index_ == max_backup_index_ )
    {
        boost::filesystem::remove(last_log_file) ;
        last_backup_file_index_ -- ;
    }

    // rename old backup files

    last_log_file = str(boost::format("%s.%d.gz") % fileName_.c_str() % (last_backup_file_index_ + 1) ) ;

    for( int i=last_backup_file_index_ ; i>=1; i-- )
    {
        string log_file = str(boost::format("%s.%d.gz") % fileName_.c_str() % i ) ;
        boost::filesystem::rename(log_file, last_log_file) ;
        last_log_file = log_file ;
    }

    // backup and compress current file

    compress(fileName_, fileName_ + ".1.gz") ;
    boost::filesystem::remove(fileName_) ;

    last_backup_file_index_ ++ ;

    // open new file

    unsigned int flags = O_CREAT | O_APPEND | O_WRONLY ;
    if ( !append_ ) flags |= O_TRUNC;

    fd_ = ::open(fileName_.c_str(), flags, 00644);
}

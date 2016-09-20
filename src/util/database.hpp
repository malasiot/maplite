#ifndef __DATABASE_H__
#define __DATABASE_H__

// Simple C++ API for SQlite database

#include <sqlite3.h>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>


namespace SQLite {

class Statement ;
class QueryResult ;
class Exception ;
class NullType;
class Connection ;
class ConnectionPool ;

extern NullType Nil;
typedef boost::shared_ptr<Connection> ConnectionPtr ;

class Database: boost::noncopyable {
public:

    Database(const std::string &fname, size_t pool_size = 1) ;
    ~Database() ;

    Connection *connect();
    void release(Connection *con) ;

private:

    std::string fileName ;
    std::map<Connection *, bool> pool_ ;
    boost::mutex mutex_ ;
};

class Session {
public:

    Session(Database *db):
        db_(db) {
        con_ = db_->connect() ;
    }

    ~Session() {
        db_->release(con_) ;
    }

    Connection &handle() const { return *con_ ; }

private:

    Connection *con_ ;
    Database *db_ ;
};

class Connection: boost::noncopyable {

    public:

    explicit Connection();
    virtual ~Connection();

    /**
     * @brief Establish a connection with the database
     * @param name      Name of the file to open
     * @param create    If set to false it will throw an exception if the file does not exist otherwise a new file will be created and opened for read/write
     * @return
     */

    virtual void open(const std::string &name, bool create = true);

    /**
     * @brief Close the connection
     */
    virtual void close() ;

    /**
     * @brief Helper for executing an sql statement, including a colon separated list of statements
     * @param sql Format string similar to printf. Use %q for arguments that need quoting (see sqlite3_mprintf documentation)
     */
    void exec(const std::string &sql, ...) ;

    sqlite3_int64 last_insert_rowid() {
        return sqlite3_last_insert_rowid(handle_);
    }

    sqlite3 *handle() { return handle_ ; }
protected:

    friend class Statement ;
    friend class Transaction ;

    void check() ;

    sqlite3 *handle_ ;
};




class Exception: public std::runtime_error
{
public:
    Exception(const std::string &msg) ;
    Exception(sqlite3 *handle) ;
};



/**
 * @brief The Statement class is a wrapper for prepared statements
 */

class Statement: boost::noncopyable
{
public:

    /**
     * @brief make a new prepared statement from the SQL string and the current connection
     */
    Statement(Connection& con, const std::string & sql) ;

    virtual ~Statement();

    /** \brief clear is used if you'd like to reuse a command object
    */
    void clear();

    /** \brief Bind value with corresponding placeholder index
     */

    Statement &bind(int idx, const NullType &p);
    Statement &bind(int idx, int v);
    Statement &bind(int idx, long long int v);
    Statement &bind(int idx, double v);
    Statement &bind(int idx, std::string const & v);
    Statement &bind(int idx, void const * buf, size_t buf_size);

    /** \brief Bind value with corresponding placeholder parameter name
     */

    Statement &bindp(const std::string &name, NullType const & p);
    Statement &bindp(const std::string &name, int p);
    Statement &bindp(const std::string &name, long long int p);
    Statement &bindp(const std::string &name, double p);
    Statement &bindp(const std::string &name, const std::string &p);
    Statement &bindp(const std::string &name, void const * buf, size_t buf_size);

    /** \brief Bind value with placeholder index automatically assigned based on the order of the calls
     */

    Statement &bind(NullType const & p);
    Statement &bind(int p);
    Statement &bind(long long int p);
    Statement &bind(double p);
    Statement &bind(const std::string &p);
    Statement &bind(void const * buf, size_t buf_size);


    sqlite3_stmt *handle() const { return handle_ ; }
protected:

    void check();
    bool step();

private:

    void prepare();
    void finalize();

private:

    Connection &con_ ;
    std::string sql_ ;

protected:

    friend class QueryResult ;

    sqlite3_stmt *handle_;

private:

    int last_arg_idx;

};


class Command: public Statement {
public:
    Command(Connection &con, const std::string &sql) ;

    void exec() ;
};

class Query: public Statement {
public:
    Query(Connection &con, const std::string &sql) ;

    QueryResult exec() ;

private:
    friend class QueryResult ;

    int columnIdx(const std::string &name) const ;
    std::map<std::string, int> field_map ;
};

typedef std::vector<unsigned char> Blob ;

class QueryResult
{

public:

    operator int () { return !empty_ ; }

    void next() ;

    int columns() const;
    int columnType(int idx) const;
    const char *columnName(int idx) const ;
    int columnIdx(const std::string &name) const ;
    int columnBytes(int idx) const ;

    template <class T> T get(int idx) const {
       return get(idx, T());
    }

    template <class T> T get(const std::string &name) const {
        int idx = columnIdx(name) ;
        return get(idx, T()) ;
    }

    void getBlob(int idx, std::vector<unsigned char> &blob) ;
    void getBlob(const std::string &name, std::vector<unsigned char> &blob) ;

    const char *getBlob(int idx, int &blob_size);
    const char *getBlob(const std::string &name, int &blob_size);

    template <class T1>
    boost::tuple<T1> getColumns(int idx1) const {
        return boost::make_tuple(get(idx1, T1()));
    }

    template <class T1, class T2>
    boost::tuple<T1, T2> getColumns(int idx1, int idx2) const {
        return boost::make_tuple(get(idx1, T1()), get(idx2, T2()));
    }

    template <class T1, class T2, class T3>
    boost::tuple<T1, T2, T3> getColumns(int idx1, int idx2, int idx3) const {
        return boost::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()));
    }

    template <class T1, class T2, class T3, class T4>
    boost::tuple<T1, T2, T3, T4> getColumns(int idx1, int idx2, int idx3, int idx4) const {
        return boost::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()));
    }

    template <class T1, class T2, class T3, class T4, class T5>
    boost::tuple<T1, T2, T3, T4, T5> getColumns(int idx1, int idx2, int idx3, int idx4, int idx5) const {
        return boost::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()));
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    boost::tuple<T1, T2, T3, T4, T5, T6> getColumns(int idx1, int idx2, int idx3, int idx4, int idx5, int idx6) const {
        return boost::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()), get(idx6, T6()));
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    boost::tuple<T1, T2, T3, T4, T5, T6, T7> getColumns(int idx1, int idx2, int idx3, int idx4, int idx5, int idx6, int idx7) const {
       return boost::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()), get(idx6, T6()), get(idx7, T7()));
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    boost::tuple<T1, T2, T3, T4, T5, T6, T7, T8> getColumns(int idx1, int idx2, int idx3, int idx4, int idx5, int idx6, int idx7, int idx8) const {
        return boost::make_tuple(get(idx1, T1()), get(idx2, T2()), get(idx3, T3()), get(idx4, T4()), get(idx5, T5()), get(idx6, T6()), get(idx7, T7()), get(idx8, T8()));
    }



private:

    friend class Query ;

    QueryResult(Query &cmd);

    int get(int idx, int) const;
    double get(int idx, double) const;
    long long int get(int idx, long long int) const;
    char const* get(int idx, char const*) const;
    std::string get(int idx, std::string) const;
    void const* get(int idx, void const*) const;
    NullType get(int idx, const NullType &) const;


private:

    Query &cmd_ ;
    bool empty_ ;

} ;

class Transaction : boost::noncopyable
{

public:

    Transaction(Connection &con_); // the construcctor starts the constructor

    // you should explicitly call commit or rollback to close it
   void commit();
   void rollback();

private:

   Connection &con_ ;
 };




} // namespace SQLite




#endif

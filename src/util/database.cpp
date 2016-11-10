#include "database.hpp"

#include <boost/format.hpp>


using namespace std ;

namespace SQLite {

class NullType {} ;

NullType Nil;

Connection::Connection(): handle_(NULL) {

}

void Connection::open(const std::string &db, bool create) {
    int flags = ( create ) ? ( SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ) : SQLITE_OPEN_READWRITE ;


    if ( sqlite3_open_v2(db.c_str(), &handle_, flags, NULL)  != SQLITE_OK )
         throw Exception("Could not open database");

}

 void Connection::close() {
     check() ;

     if( sqlite3_close(handle_) != SQLITE_OK )
         throw Exception(sqlite3_errmsg(handle_));

     handle_ = NULL ;
 }

void Connection::exec(const string &sql, ...)
{
    va_list arguments ;
    va_start(arguments, sql);

    char *sql_ = sqlite3_vmprintf(sql.c_str(), arguments) ;

    char *err_msg ;
    if ( sqlite3_exec(handle_, sql_, NULL, NULL, &err_msg) != SQLITE_OK )
    {
        string msg(err_msg) ;
        sqlite3_free(err_msg) ;

        throw Exception(msg) ;

    }

    sqlite3_free(sql_) ;

    va_end(arguments);
}

void Connection::check() {
     if( !handle_ )
         throw Exception("Database is not open.");
}

Connection::~Connection()
{
    close() ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Exception::Exception(sqlite3 *handle): std::runtime_error(sqlite3_errmsg(handle)) {}
Exception::Exception(const std::string &msg): std::runtime_error(msg) {}

Statement::Statement(Connection &con, const string &sql): con_(con), sql_(sql), last_arg_idx(0)
{
    con_.check() ;

    const char * tail = 0;
    sqlite3_stmt *handle ;
    if ( sqlite3_prepare_v2(con.handle(), sql.c_str(), -1, &handle ,&tail) != SQLITE_OK )
          throw Exception(con.handle()) ;
    handle_.reset(handle, &sqlite3_finalize) ;

}

Statement::~Statement() {
    try {
        finalize();
    }
    catch(...) {
    }
}

void Statement::clear()
{
     check();
     if ( sqlite3_reset(handle_.get()) != SQLITE_OK ) throw Exception(con_.handle());

     if ( sqlite3_clear_bindings(handle_.get()) != SQLITE_OK ) throw Exception(con_.handle());

     last_arg_idx = 0 ;
}

void Statement::finalize() {
    check();
    if ( sqlite3_finalize(handle_.get()) != SQLITE_OK ) throw Exception(con_.handle());
    handle_ = 0;
}

void Statement::check() {
    con_.check() ;
    if( !handle_ ) throw Exception("Statement has not been compiled.");
}

/* returns true if the command returned data */

bool Statement::step() {
    check() ;

    switch( sqlite3_step(handle_.get()) ) {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            throw Exception(con_.handle());
    }
    return false;
}


Statement &Statement::bind(int idx, const NullType &) {
    check();
    if ( sqlite3_bind_null(handle_.get(), idx) != SQLITE_OK )
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(int idx, int v) {
    check();
    if ( sqlite3_bind_int(handle_.get(), idx, v) != SQLITE_OK )
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(int idx, long long int v){
    check();
    if ( sqlite3_bind_int64(handle_.get(), idx, v) != SQLITE_OK )
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(int idx, unsigned long long int v){
    check();
    if ( sqlite3_bind_int64(handle_.get(), idx, v) != SQLITE_OK )
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(int idx, double v){
    check() ;
    if ( sqlite3_bind_double(handle_.get(), idx, v) != SQLITE_OK )
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(int idx, const string  &v){
    check() ;
    if ( sqlite3_bind_text(handle_.get(), idx, v.c_str(), int(v.size()), SQLITE_TRANSIENT ) != SQLITE_OK )
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(int idx, void const * v , size_t vn){
    check();
    if ( sqlite3_bind_blob(handle_.get(), idx, v, int(vn), SQLITE_TRANSIENT) != SQLITE_OK)
        throw Exception(con_.handle());
    return *this ;
}

Statement &Statement::bind(const NullType &v) {
    return bind(++last_arg_idx, v) ;
}

Statement &Statement::bind(int v) {
    return bind(++last_arg_idx, v) ;
}

Statement &Statement::bind(long long int v){
    return bind(++last_arg_idx, v) ;
}

Statement &Statement::bind(unsigned long long int v){
    return bind(++last_arg_idx, v) ;
}

Statement &Statement::bind(double v){
    return bind(++last_arg_idx, v) ;
}

Statement &Statement::bind(const string  &v){
    return bind(++last_arg_idx, v) ;
}

Statement &Statement::bind(void const * v , size_t vn){
    return bind(++last_arg_idx, v, vn) ;
}

Statement &Statement::bindp(const string &name, const NullType &v) {
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

Statement &Statement::bindp(const string &name, int v) {
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

Statement &Statement::bindp(const string &name, long long int v){
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

Statement &Statement::bindp(const string &name, unsigned long long int v){
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

Statement &Statement::bindp(const string &name, double v){
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

Statement &Statement::bindp(const string &name, const string  &v){
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

Statement &Statement::bindp(const string &name, void const * v , size_t vn){
    int idx = sqlite3_bind_parameter_index(handle_.get(), name.c_str() );
    if ( idx ) return bind(idx, v, vn) ;
    else throw Exception(name + " is not a valid statement placeholder") ;
}

///////////////////////////////////////////////////////////////////////////////////////

Command::Command(Connection &con, const string &sql):
    Statement(con, sql) {

}


void Command::exec() {
    step() ;
}


////////////////////////////////////////////////////////////////////////////////////////
Query::Query(Connection &con, const string &sql):
    Statement(con, sql) {

    int num_fields = sqlite3_column_count(handle_.get());

    for( int index = 0; index < num_fields; index++ )
    {
        const char* field_name = sqlite3_column_name(handle_.get(), index);
        field_map[field_name] = index ;
    }
}

int Query::columnIdx(const string &name) const {
    map<string, int>::const_iterator it = field_map.find(name) ;
    if ( it != field_map.end() ) return (*it).second ;
    else throw Exception("There is no column '" + name + "' in the results table") ;
    return 0 ;
}

QueryResult Query::exec() {
    return QueryResult(*this) ;
}


///////////////////////////////////////////////////////////////////////////////////////

QueryResult::QueryResult(Query &cmd): cmd_(cmd) {
    next() ;
}

int QueryResult::columns() const {
    cmd_.check() ;
    return ( sqlite3_data_count(cmd_.handle().get()) ) ;
}

const char *QueryResult::columnName(int idx) const {
   cmd_.check() ;
   const char *name = sqlite3_column_name(cmd_.handle().get(), idx)  ;
   if ( name == NULL ) throw Exception(str(boost::format("There is no column with index %d") % idx)) ;
   else return name ;
}

int QueryResult::columnIdx(const string &name) const {
    cmd_.check() ;
    return cmd_.columnIdx(name) ;
}

void QueryResult::next() {
    empty_ = !cmd_.step() ;
}

int QueryResult::columnType(int idx) const
{
    cmd_.check() ;
    return sqlite3_column_type(cmd_.handle().get(), idx);
}

int QueryResult::columnBytes(int idx) const
{
    cmd_.check() ;
    return sqlite3_column_bytes(cmd_.handle(), idx);
}

int QueryResult::get(int idx, int) const
{
    cmd_.check() ;
    return sqlite3_column_int(cmd_.handle(), idx);
}

double QueryResult::get(int idx, double) const
{
    cmd_.check() ;
    return sqlite3_column_double(cmd_.handle(), idx);
}

long long int QueryResult::get(int idx, long long int) const
{
    cmd_.check() ;
    return sqlite3_column_int64(cmd_.handle(), idx);
}

unsigned long long int QueryResult::get(int idx, unsigned long long int) const
{
    cmd_.check() ;
    return sqlite3_column_int64(cmd_.handle(), idx);
}

char const* QueryResult::get(int idx, char const*) const
{
    return reinterpret_cast<char const*>(sqlite3_column_text(cmd_.handle(), idx));
}

std::string QueryResult::get(int idx, std::string) const
{
    const char *res = get(idx, (char const*)0);
    if ( res == 0 ) return string() ;
    else return res ;
}

void QueryResult::getBlob(int idx, std::vector<unsigned char> &blob)
{
    cmd_.check() ;
    const void *data = sqlite3_column_blob(cmd_.handle(), idx);
    int bytes = sqlite3_column_bytes(cmd_.handle(), idx) ;

    if ( data )
        std::copy((unsigned char *)data, (unsigned char *)data + bytes, back_inserter(blob));

}

const char *QueryResult::getBlob(int idx, int &blob_size)
{
    cmd_.check() ;
    const void *data = sqlite3_column_blob(cmd_.handle(), idx);
    blob_size = sqlite3_column_bytes(cmd_.handle(), idx) ;

    return (const char *)data ;
}

void QueryResult::getBlob(const string &name, std::vector<unsigned char> &blob)
{
    int idx = columnIdx(name) ;
    getBlob(idx, blob) ;
}

const char *QueryResult::getBlob(const string &name, int &blob_size)
{
    int idx = columnIdx(name) ;
    return getBlob(idx, blob_size) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


Database::Database(const std::string &fname, size_t pool_size):fileName(fname) {
    for(int i=0 ; i<pool_size ; i++ )
    {
        Connection *con = new Connection() ;
        con->open(fname) ;
        pool_.insert(std::make_pair(con, true)) ;
    }

}

Database::~Database() {

    map<Connection *, bool>::iterator it = pool_.begin() ;

    for( ; it != pool_.end() ; ++it )
    {
        delete (*it).first ;
    }

}

Connection *Database::connect()
{

    boost::mutex::scoped_lock  lock(mutex_);

    map<Connection *, bool>::iterator it = pool_.begin() ;

    // try opened connections

    for( ; it != pool_.end() ; ++it )
    {
        if ( (*it).first )
        {
            (*it).second = false ;
            return (*it).first ;
        }

    }

    // otherwise open a new connection outside of pool

    Connection *con = new Connection() ;
    con->open(fileName) ;
    return con ;

}

void Database::release(Connection *con)
{
    boost::mutex::scoped_lock  lock(mutex_);

    map<Connection *, bool>::iterator it = pool_.find(con) ;

    if ( it == pool_.end() ) {
        delete con ;
    }
    else
        (*it).second = true ;
}

Transaction::Transaction(Connection &con): con_(con)
{
    char *err_msg ;

    if ( sqlite3_exec (con_.handle(), "BEGIN", NULL, NULL, &err_msg) != SQLITE_OK )
    {
        throw Exception(err_msg);
        sqlite3_free (err_msg);
    }

}

void Transaction::commit()
{
    char *err_msg ;

    if ( sqlite3_exec (con_.handle(), "COMMIT", NULL, NULL, &err_msg) != SQLITE_OK )
    {
        throw Exception(err_msg);
        sqlite3_free (err_msg);
    }

}

void Transaction::rollback()
{
    char *err_msg ;

    if ( sqlite3_exec (con_.handle(), "ROLLBACK", NULL, NULL, &err_msg) != SQLITE_OK )
    {
        throw Exception(err_msg);
        sqlite3_free (err_msg);
    }

}

}

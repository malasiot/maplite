#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <zlib.h>

#include "database.hpp"
#include "dictionary.hpp"

#include <boost/filesystem.hpp>

using namespace std ;
namespace fs = boost::filesystem ;


using namespace std ;
namespace fs = boost::filesystem ;

#define windowBits 15
#define GZIP_ENCODING 16

static string read_file( const string &fileName, size_t &sizeOrig, bool compressFile = true)
{
    ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate );

    sizeOrig = ifs.tellg();
    ifs.seekg(0, ios::beg);

    string bytes((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>());
    if ( compressFile )
    {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));


        if ( deflateInit2 (&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                             windowBits | GZIP_ENCODING, 8,
                             Z_DEFAULT_STRATEGY) != Z_OK )
            return string() ;

        zs.next_in = (Bytef*)bytes.c_str() ;
        zs.avail_in = sizeOrig ;

        int ret;
        char outbuffer[32768];
        std::string outstring;

        // retrieve the compressed bytes blockwise
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);

            ret = deflate(&zs, Z_FINISH);

            if (outstring.size() < zs.total_out) {
                // append the block to the output string
                outstring.append(outbuffer,
                                 zs.total_out - outstring.size());
            }
        } while (ret == Z_OK);

        deflateEnd(&zs);

        return outstring ;
    }
    else return bytes ;
}

void get_file_list(const string &key, const fs::path &filePath, Dictionary &files)
{

    string prefix ;

    if ( !key.empty() ) prefix = key + '/' ;

    if ( fs::is_regular_file(filePath) )
    {
        files.add(prefix + filePath.filename().string(), filePath.string()) ;
    }
    else {
        fs::directory_iterator end_itr;

        for (fs::directory_iterator itr(filePath); itr != end_itr; ++itr)
        {
            if (fs::is_regular_file(itr->path())) {
                string current_file = itr->path().string();
                files.add(prefix + itr->path().filename().string(), current_file) ;
            }
            else get_file_list(prefix + itr->path().stem().string(), itr->path(), files) ;
        }
    }

}

void printUsageAndExit()
{
    cerr << "Usage: makeres [--overwrite] [--append] <resource_database> <files>+" << endl ;
    exit(1) ;
}

int main(int argc, char *argv[])
{
    string dataFile, prefix ;
    vector<string> resFiles ;
    bool append = false ;

    for( int i=1 ; i<argc ; i++ )
    {
        string arg = argv[i] ;

        if ( arg == "--append" )   // if false it will delete contents of the table before inserting new data
            append = true ;
        else if ( arg == "--prefix" ) {
            if ( ++i < argc )
                prefix = arg ;
        }
        else if ( dataFile.empty() )
            dataFile = argv[i] ;
        else
            resFiles.push_back(argv[i]) ;
    }

    if ( dataFile.empty() ||  resFiles.empty() )
        printUsageAndExit() ;

    std::unique_ptr<SQLite::Database> db ;

    db.reset(new SQLite::Database(dataFile)) ;

    SQLite::Session session(db.get()) ;
    SQLite::Connection &con = session.handle() ;

    try {
        con.exec("CREATE TABLE IF NOT EXISTS resources (name TEXT PRIMARY KEY, sz INT, data BLOB);") ;
        if ( !append ) con.exec("DELETE FROM resources;") ;

        SQLite::Command stmt(con, "REPLACE INTO resources (name,sz,data) VALUES(?,?,?)") ;

        SQLite::Transaction trans(con) ;

        Dictionary files ;

        for( const fs::path &file : resFiles) {
                get_file_list("", file, files) ;
        }

        DictionaryIterator it(files) ;

        while (it)
        {
            string key = it.key() ;
            string file_path = it.value() ;

            cout << file_path << endl ;

            size_t sz ;
            string content = read_file(file_path, sz) ;

            stmt.bind(key) ;
            stmt.bind((int)sz) ;
            stmt.bind(content.data(), content.size()) ;

            stmt.exec() ;
            stmt.clear() ;

            ++it ;
        }

        trans.commit() ;
    }
    catch ( SQLite::Exception &e )
    {
        cerr << e.what() << endl ;
        return 0 ;
    }
}

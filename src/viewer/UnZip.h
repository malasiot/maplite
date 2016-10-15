#ifndef __UNZIP_H__
#define __UNZIP_H__

#include <string>
#include <vector>

#include "minizip/unzip.h"

class UnZip
{
private:

    std::string filename_;
    unzFile uf;

public:
    UnZip();
    ~UnZip();

public:

    bool open( const std::string &zip_filename );
    void close( void );

public:

    bool listContents( std::vector<std::string> & files );

    std::string readFile(const std::string &fileName, const std::string &password = std::string()) ;

};


#endif

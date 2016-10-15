#include "UnZip.h"

#include <stdio.h>
#include <string.h>

#define UNZIP_BUFFER_SIZE (8192)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "minizip/iowin32.h"
#endif

using namespace std ;


UnZip::UnZip(): uf(0) {
}

UnZip::~UnZip()
{
    if ( uf )
        unzClose(uf) ;
}


bool UnZip::open(const string &filename)
{

#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
#endif

#ifdef USEWIN32IOAPI
    fill_win32_filefunc64A(&ffunc);
    uf = unzOpen2_64(filename.c_str(), &ffunc);
#else
    uf = unzOpen64(filename.c_str());
#endif

    if (uf == NULL)
    {
#ifdef USEWIN32IOAPI
        uf = unzOpen2_64((filename + ".zip").c_str(), &ffunc);
#else
        uf = unzOpen64((filename + ".zip").c_str());
#endif
    }

    if ( uf == NULL ) return false ;

    filename_ = filename ;

    return true ;

}

bool UnZip::listContents(std::vector<string> &files)
{
    unz_global_info64 gi;
    int err;

    if ( unzGetGlobalInfo64(uf, &gi) != UNZ_OK ) return false ;

    //printf("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
    //printf("  ------  ------     ---- -----   ----    ----   ------     ----\n");

    for ( int i=0; i<gi.number_entry ; i++ )
    {
        char filename_inzip[256];
        unz_file_info64 file_info;

        if ( unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL,0,NULL,0) != UNZ_OK )  return false ;

        files.push_back(filename_inzip) ;

        if ( (i+1)<gi.number_entry && unzGoToNextFile(uf) != UNZ_OK ) return false ;
     }

    return true ;

}

std::string UnZip::readFile(const string &filename, const string &password)
{
    string res ;
    if ( unzLocateFile(uf, filename.c_str(), 0 ) != UNZ_OK) return string() ;

    char buf[UNZIP_BUFFER_SIZE];

    unz_file_info64 file_info;

 //   if ( unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL,0,NULL,0) != UNZ_OK ) return string() ;

    if ( password.empty() )
    {
        if ( unzOpenCurrentFile(uf) != UNZ_OK ) return string() ;
    }
    else
    {
        if ( unzOpenCurrentFilePassword(uf, password.c_str()) != UNZ_OK ) return string() ;
    }

    int ret ;
    while ( ( ret = unzReadCurrentFile(uf, buf, UNZIP_BUFFER_SIZE) ) > 0 )
    {
        res.append(buf, ret) ;
    }

    unzCloseCurrentFile (uf);

    return res ;
}

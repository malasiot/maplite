#ifndef __BINARY_STREAM_HPP__
#define __BINARY_STREAM_HPP__

#include <iostream>

class MapFileISerializer
{
public:

    // Initialize using a stream. The stream should be open in binary mode.

    MapFileISerializer(std::istream &strm): strm_(strm) {}

    // read from stream
    uint32_t read_uint32() ;
    uint64_t read_uint64() ;
    int32_t  read_int32() ;
    int64_t  read_int64() ;
    int16_t  read_int16() ;
    uint16_t read_uint16() ;
    uint8_t  read_uint8() ;
    int8_t   read_int8() ;
    std::string read_utf8() ;
    uint64_t read_var_uint64() ;
    int64_t  read_var_int64() ;
    int64_t  read_offset();

    void read_bytes(uint8_t *buf, uint32_t n) ;

private:

    std::istream &strm_ ;

} ;

class MapFileOSerializer
{
public:

    // Initialize using a stream. The stream should be open in binary mode.

    MapFileOSerializer(std::ostream &strm): strm_(strm) {}


    // write to stream
    void write_uint32(uint32_t val) ;
    void write_uint64(uint64_t val) ;
    void write_int32(int32_t val) ;
    void write_int64(int64_t val) ;
    void write_int16(int16_t val) ;
    void write_uint16(uint16_t val) ;
    void write_uint8(uint8_t val) ;
    void write_int8(int8_t val) ;
    void write_utf8(const std::string &val) ;
    void write_var_uint64(uint64_t val) ;
    void write_var_int64(int64_t val) ;
    void write_offset(int64_t val);
    void write_bytes(uint8_t *bytes, uint32_t len);
    void write_bytes(const std::string &bytes);

private:

    std::ostream &strm_ ;

} ;
#endif

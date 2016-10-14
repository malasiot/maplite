#include "serializer.hpp"
#include <stdexcept>

using namespace std ;

enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul
};

static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order =
{ { 0, 1, 2, 3 } };

static const bool platform_is_little_endian = ( o32_host_order.value == 0x03020100ul ) ;

static void byte_swap_32(uint32_t &data)
{
    union u {uint32_t v; uint8_t c[4];};
    u un, vn;
    un.v = data ;
    vn.c[0]=un.c[3];
    vn.c[1]=un.c[2];
    vn.c[2]=un.c[1];
    vn.c[3]=un.c[0];
    data = vn.v ;
}

static void byte_swap_64(uint64_t &data)
{
    union u {uint64_t v; uint8_t c[8];};
    u un, vn;
    un.v = data ;
    vn.c[0]=un.c[7];
    vn.c[1]=un.c[6];
    vn.c[2]=un.c[5];
    vn.c[3]=un.c[4];
    vn.c[4]=un.c[3];
    vn.c[5]=un.c[2];
    vn.c[6]=un.c[1];
    vn.c[7]=un.c[0];
    data = vn.v ;
}

static void byte_swap_16(uint16_t &nValue)
{
    nValue = ((( nValue>> 8)) | (nValue << 8));
}


class WriteException: public runtime_error {
public:
    WriteException():
        runtime_error("Error writing map file") {}
};


void MapFileOSerializer::write_uint32(uint32_t val)
{
    if ( platform_is_little_endian )
        byte_swap_32(val) ;

    if ( !strm_.write((char *)&val, 4) )
        throw WriteException() ;
}

void MapFileOSerializer::write_int32(int32_t val) {
    write_uint32(val) ;
}


void MapFileOSerializer::write_uint64(uint64_t val)
{
    if ( platform_is_little_endian ) byte_swap_64(val) ;

    if ( !strm_.write((char *)&val, 8) )
        throw WriteException() ;
}

void MapFileOSerializer::write_int64(int64_t val) {
    write_uint64(val) ;
}

void MapFileOSerializer::write_uint16(uint16_t val)
{
    if ( platform_is_little_endian )
        byte_swap_16(val) ;

    if ( !strm_.write((char *)&val, 2) )
        throw WriteException() ;
}

void MapFileOSerializer::write_int16(int16_t val) {
    return write_uint16(val) ;
}

void MapFileOSerializer::write_uint8(uint8_t val) {

    if ( !strm_.write((char *)&val, 1) )
        throw WriteException() ;
}

void MapFileOSerializer::write_int8(int8_t val) {
    write_uint8(val) ;
}

void MapFileOSerializer::write_var_uint64(uint64_t val)
{
    uint8_t byte ;

    do {
      byte = val & 0x7full ;
      val >>= 7;
      if ( val ) byte |= 0x80 ;

      if ( !strm_.put(byte) )
          throw WriteException() ;
    } while (val);
}

void MapFileOSerializer::write_var_int64(int64_t val)
{

    uint64_t value = abs(val) ;
    uint8_t byte ;

    while ( value > 0x3f ) {
        byte = ( value & 0x7fll ) | 0x80 ;
        value >>= 7;


      if ( !strm_.put(byte ) )
          throw WriteException() ;

    }

    byte = ( value & 0x3fll ) ;
    if ( val < 0 ) byte |= 0x40 ;

    if( !strm_.put(byte) )
        throw WriteException() ;

}

void MapFileOSerializer::write_offset(int64_t offset) {

    uint64_t val ;

    if ( platform_is_little_endian ) {
        val = offset << 24;
        byte_swap_64(val) ;
    }
    else val = offset ;

    if ( !strm_.write((char *)&val, 5) )
        throw WriteException() ;
}

void MapFileOSerializer::write_bytes(uint8_t *bytes, uint32_t len)
{
    if ( !strm_.write((char *)bytes, len) )
        throw WriteException() ;
}

void MapFileOSerializer::write_bytes(const std::string &bytes)
{
    if ( !strm_.write((char *)&bytes[0], bytes.size()) )
        throw WriteException() ;
}

void MapFileOSerializer::write_utf8(const string &str)
{
    write_var_uint64(str.length()) ;
    if ( !strm_.write(&str[0], str.length()) )
        throw WriteException() ;
}


class ReadException: public runtime_error {
public:
    ReadException():
        runtime_error("Error reading map file") {}
};


uint32_t MapFileISerializer::read_uint32()
{
    uint32_t val ;
    if ( !strm_.read((char *)&val, 4) )
        throw ReadException() ;

    if ( platform_is_little_endian )
        byte_swap_32(val) ;

    return val ;
}

int32_t MapFileISerializer::read_int32() {
    return read_uint32() ;
}


uint64_t MapFileISerializer::read_uint64()
{
    uint64_t val ;

    if ( !strm_.read((char *)&val, 8) )
        throw ReadException() ;

    if ( platform_is_little_endian )
        byte_swap_64(val) ;

    return val ;
}

int64_t MapFileISerializer::read_int64() {
    return read_uint64() ;
}

uint16_t MapFileISerializer::read_uint16()
{
    uint16_t val ;
    if ( !strm_.read((char *)&val, 2) )
        throw ReadException() ;

    if ( platform_is_little_endian )
        byte_swap_16(val) ;

    return val ;
}

int16_t MapFileISerializer::read_int16() {
    return read_uint16() ;
}

uint8_t MapFileISerializer::read_uint8()
{
    uint8_t val ;
    if ( !strm_.read((char *)&val, 1) )
        throw ReadException() ;

    return val ;
}

int8_t MapFileISerializer::read_int8() {
    return read_uint8() ;
}

uint64_t MapFileISerializer::read_var_uint64()
{
    uint64_t val = UINT64_C(0);
    unsigned int shift = 0;
    uint8_t byte ;

    while ( 1 )
    {
        if ( !strm_.get((char &)byte) ) throw ReadException() ;

        val |= (( byte & 0x7full ) << shift) ;
        shift += 7 ;

        if ( ( byte & 0x80 ) == 0 ) break ;

        if ( shift > 63 )
            throw ReadException() ;
    }

    return val ;
}

int64_t MapFileISerializer::read_var_int64()
{
    int64_t val = INT64_C(0);
    unsigned int shift = 0;
    uint8_t byte ;

    while (1)
    {
        if ( !strm_.get((char &)byte) ) throw ReadException() ;

        if ( ( byte & 0x80) == 0 ) break ;

        val |= ( byte & 0x7full ) << shift;

        shift += 7 ;
    }


    if ( ( byte & 0x40 ) != 0 )
        val = -(val | ((byte & 0x3full) << shift)) ;
    else
        val |= ( byte & 0x3fll ) << shift ;


    return val ;
}


int64_t MapFileISerializer::read_offset() {
    char buffer[5] ;

    if ( !strm_.read(buffer, 5) )
        throw ReadException() ;

    int64_t v = (buffer[0] & 0xffL) << 32 | (buffer[1] & 0xffL) << 24 | (buffer[2] & 0xffL) << 16 | (buffer[3] & 0xffL) << 8 | (buffer[4] & 0xffL) ;

    return v ;
}

std::string MapFileISerializer::read_utf8()
{
    string str ;
    uint64_t len = read_var_uint64() ;
    str.resize(len) ;
    if ( !strm_.read(&str[0], len) )
        throw ReadException() ;

    return str ;
}

void MapFileISerializer::read_bytes(uint8_t *bytes, uint32_t len)  {
    if ( !strm_.read((char *)bytes, len) )
        throw ReadException() ;
}

#include "jp2_decoder.hpp"
#include "logger.hpp"

#include <openjpeg.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <boost/filesystem.hpp>

using namespace std;

static void error_callback(const char *msg, void *user) {
    string *fname = (string *)user ;
    LOG_ERROR_STREAM(msg << ":" << *fname) ;
}

static void warning_callback(const char *msg, void *user) {
    string *fname = (string *)user ;
    LOG_WARN_STREAM(msg << ":" << *fname) ;
}

static void info_callback(const char *msg, void *user) {
    string *fname = (string *)user ;
 //   LOG_INFO_STREAM(msg << ":" << *fname) ;
}

static const unsigned char jpc_header[] = {0xff,0x4f};
static const unsigned char jp2_box_jp[] = {0x6a,0x50,0x20,0x20}; /* 'jP  ' */

#define CPL_SWAP32(x) \
        ((uint32_t)( \
            (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
            (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
            (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
            (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24) ))

#define CPL_SWAP64(x) \
        ((uint64_t)( \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) << 8) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >> 8) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
            (uint64_t)(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) ))

static bool read_box(FILE *fp, string &type, uint64_t &len, uint64_t &box_offset, uint64_t &data_offset, char uuid[16]) {

    // seek to box start
     fseek( fp, box_offset, SEEK_SET ) ;

     char type_str[5] ;

     uint32_t slen ;
     if ( fread( &slen, 4, 1, fp ) != 1 ) return false ;
     if ( fread( type_str, 4, 1, fp ) != 1 ) return false ;

     type_str[4] = '\0';
     type = type_str ;

     len = CPL_SWAP32( slen ); // swap if little endian

     if ( slen == 1 ) {

         /* do we have a "special very large box ?" */
         /* read then the XLBox */

        uint64_t xlen ;
        if ( fread( &xlen, 8, 1, fp ) != 1 ) return false;

        len = CPL_SWAP64(xlen) ;

        data_offset = box_offset + 16;
     }
     else
         data_offset = box_offset + 8;

     if ( len == 0 ) // last block?
     {
         if( fseek( fp, 0, SEEK_END ) != 0 ) return false;
         len = ftell( fp ) - box_offset ;

         if( fseek( fp, data_offset, SEEK_SET ) != 0 )
             return false;
     }

     if( type == "uuid" ) {
         if( fread( uuid, 16, 1, fp ) != 1 ) return false;
         data_offset += 16;
     }

     return true ;
}

static const unsigned char msi_uuid2[16] =
{0xb1,0x4b,0xf8,0xbd,0x08,0x3d,0x4b,0x43,
 0xa5,0xae,0x8c,0xd7,0xd5,0xa6,0xce,0x03};

static const unsigned char msig_uuid[16] =
{ 0x96,0xA9,0xF1,0xF1,0xDC,0x98,0x40,0x2D,
  0xA7,0xAE,0xD6,0x8E,0x34,0x45,0x18,0x09 };

static const unsigned char xmp_uuid[16] =
{ 0xBE,0x7A,0xCF,0xCB,0x97,0xA9,0x42,0xE8,
  0x9C,0x71,0x99,0x94,0x91,0xE3,0xAF,0xAC};

static uint64_t jp2_find_code_stream( FILE* fp, uint64_t &s_length )
{
    uint64_t code_stream_start = 0;
    uint64_t code_stream_length = 0;

    fseek(fp, 0, SEEK_SET);

    uint8_t header[16];
    fread(header, 1, 16, fp);

    if ( memcmp( header, jpc_header, sizeof(jpc_header) ) == 0) // JPC format
    {
        fseek(fp, 0, SEEK_END);
        code_stream_length = ftell(fp) ;
    }
    else if (memcmp( header + 4, jp2_box_jp, sizeof(jp2_box_jp) ) == 0) // JP2 format
    {
        string btype ;
        uint64_t box_len, box_offset = 0, data_offset ;
        char uuid[16] ;

        while ( read_box(fp, btype, box_len, box_offset, data_offset, uuid) ) {

//            cout << btype << endl ;
            if( btype == "asoc" || btype == "jp2h" || btype == "res " ) {

                string sbtype ;
                uint64_t sbox_len, sbox_offset = data_offset, sbox_data_offset ;
                char sbox_uuid[16] ;

                while ( read_box(fp, sbtype, sbox_len, sbox_offset, sbox_data_offset, sbox_uuid) )
                {
                    if ( sbtype == "uuid" && memcmp(sbox_uuid, msi_uuid2, 16 ) == 0) {
//                        cout << "msi_uuid2" << endl ;
                    }
                    else if ( sbtype == "uuid" && memcmp(sbox_uuid, msig_uuid, 16 ) == 0) {
//                        cout << "msig_uuid" << endl ;
                    }

                    if ( sbox_offset >= box_offset + box_len ) break ;
                    else sbox_offset += sbox_len ;
                }
            }
            else if ( btype == "uuid" && memcmp(uuid, msi_uuid2, 16 ) == 0) {
//                cout << "msi_uuid2" << endl ;
            }
            else if ( btype == "uuid" && memcmp(uuid, msig_uuid, 16 ) == 0) {
//                cout << "msig_uuid" << endl ;
            }
            else if ( btype == "uuid" && memcmp(uuid, xmp_uuid, 16 ) == 0) {
//                cout << "xmp_uuid" << endl ;
            }
            else if ( btype == "jp2c" ) {
                code_stream_start = data_offset;
                code_stream_length = box_len - (data_offset - box_offset);
                break ;
            }
            box_offset += box_len ;
        }
    }
    s_length = code_stream_length ;
    return code_stream_start ;
}

typedef struct
{
    FILE*    fp_;
    uint64_t offset_;
} JP2OpenJPEGFile;


static OPJ_SIZE_T jpf_read(void* buffer, OPJ_SIZE_T nbytes, void *user_data)
{
    JP2OpenJPEGFile* jpf = (JP2OpenJPEGFile* )user_data ;
    int ret = static_cast<int>(fread(buffer, 1, nbytes, jpf->fp_));

    if (ret == 0) ret = -1;
    return ret;
}

static OPJ_OFF_T jpf_skip(OPJ_OFF_T nbytes, void *user_data)
{
    JP2OpenJPEGFile* jpf = (JP2OpenJPEGFile* )user_data ;
    uint64_t offset = ftell(jpf->fp_);
    offset += nbytes;
    fseek(jpf->fp_, offset, SEEK_SET);
    return nbytes;
}

static OPJ_BOOL jpf_seek(OPJ_OFF_T nbytes, void *user_data)
{
    JP2OpenJPEGFile* jpf = (JP2OpenJPEGFile* )user_data ;
    return fseek(jpf->fp_, jpf->offset_ + nbytes, SEEK_SET) == 0 ;
}

opj_stream_t* create_read_stream(JP2OpenJPEGFile &jpf, uint64_t sz)
{
    opj_stream_t * stream = opj_stream_create(1024, true); // Default 1MB is way too big for some datasets

    if ( stream == nullptr ) return nullptr ;

    fseek(jpf.fp_, jpf.offset_, SEEK_SET);
    opj_stream_set_user_data_length(stream, sz);

    opj_stream_set_read_function(stream, jpf_read);
    opj_stream_set_seek_function(stream, jpf_seek);
    opj_stream_set_skip_function(stream, jpf_skip);
    opj_stream_set_user_data(stream, &jpf, NULL);

    return stream;
}

namespace fs = boost::filesystem ;

JP2Decoder::JP2Decoder(RasterTileCache *cache): RasterTileSource(cache)
{

}

struct opj_context {
    FILE *fp_ ;
    opj_codec_t*    codec_ = nullptr;
    opj_stream_t *  stream_ = nullptr;
    opj_image_t *   image_ = nullptr;

    ~opj_context() {
        if ( fp_ ) fclose(fp_) ;
        if ( codec_ && stream_ ) opj_end_decompress(codec_, stream_);
        if ( stream_ ) opj_stream_destroy(stream_) ; stream_ = nullptr ;
        if ( codec_ ) opj_destroy_codec(codec_) ; codec_ = nullptr ;
        if ( image_ ) opj_image_destroy(image_) ; image_ = nullptr ;
    }

};

bool JP2Decoder::open(const string &file_name)
{
    opj_context ctx ;

    ctx.fp_ = fopen(file_name.c_str(), "rb") ;

    if ( !ctx.fp_ ) {
        LOG_FATAL_STREAM("cannot open JP2 file: " << file_name) ;
        return false;
    }

    uint64_t code_stream_length = 0 ;
    uint64_t code_stream_start = jp2_find_code_stream(ctx.fp_, code_stream_length) ;

    JP2OpenJPEGFile jfp ;

    jfp.fp_ = ctx.fp_ ;
    jfp.offset_ = code_stream_start ;

    ctx.stream_ = create_read_stream(jfp, code_stream_length);

    if( ctx.stream_ == NULL ) {
        LOG_FATAL_STREAM("create read stream failed: " << file_name) ;
        return false ;
    }

    ctx.codec_ = opj_create_decompress(OPJ_CODEC_J2K);

    if (  ctx.codec_ == nullptr )  {
        LOG_FATAL_STREAM("opj_create_decompress() failed: " << file_name) ;
        return false ;
    }

    opj_set_info_handler(ctx.codec_, info_callback, (void *)&file_name);
    opj_set_warning_handler(ctx.codec_, warning_callback, (void *)&file_name);
    opj_set_error_handler(ctx.codec_, error_callback, (void *)&file_name);

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);

    if (!opj_setup_decoder(ctx.codec_, &parameters))
    {
        LOG_FATAL_STREAM("opj_setup_decoder() failed: " << file_name) ;
        return false ;
    }

    if( !opj_read_header(ctx.stream_, ctx.codec_, &ctx.image_) )
    {
        LOG_FATAL_STREAM("opj_read_header() failed" << file_name) ;
        return false ;
    }

    /* Extract some info from the code stream */
    opj_codestream_info_v2_t *jp_info = opj_get_cstr_info(ctx.codec_);

    resolutions_ = jp_info->m_default_tile_info.tccp_info[0].numresolutions;
    tile_width_ =jp_info->tdx ;
    tile_height_ = jp_info->tdy ;
    n_tiles_x_ = jp_info->tw ;
    n_tiles_y_ = jp_info->th ;
    OPJ_UINT32 tox = jp_info->tx0, toy = jp_info->ty0 ; // tile offset

    opj_destroy_cstr_info(&jp_info);

    image_width_ = ctx.image_->x1 - ctx.image_->x0 ;
    image_height_ = ctx.image_->y1 - ctx.image_->y0 ;

    if ( tox != 0 || toy != 0 || ( ctx.image_->numcomps != 1 && ctx.image_->numcomps != 3 ) || ctx.image_->comps[0].prec != 8 ) {
        LOG_FATAL_STREAM("not supported image format: " << file_name) ;
        return false ;
    }

    // read extern world file (created using GDAL option -co "WORLDFILE=ON"

    fs::path p(file_name) ;
    fs::path wp = p.parent_path() / ( p.stem().native() + ".wld") ;

    if ( !fs::exists(wp) ) {
        LOG_FATAL_STREAM("Missing world file: " << wp.native()) ;
        return false ;
    }

    ifstream strm(wp.native().c_str()) ;
    for(size_t i=0 ; i<6 ; i++ ) strm >> georef_[i] ;

    is_valid_ = true ;
    file_name_ = file_name ;
    return true ;
}

RasterTileData JP2Decoder::read_tile(uint32_t ti, uint32_t tj)
{
    opj_context ctx ;

    RasterTileData empty ;

    if ( !is_valid_ ) return empty ;

    ctx.fp_ = fopen(file_name_.c_str(), "rb") ;

    if ( !ctx.fp_ ) {
        LOG_FATAL_STREAM("cannot open JP2 file: " << file_name_) ;
        return empty;
    }

    uint64_t code_stream_length = 0 ;
    uint64_t code_stream_start = jp2_find_code_stream(ctx.fp_, code_stream_length) ;

    JP2OpenJPEGFile jfp ;

    jfp.fp_ = ctx.fp_ ;
    jfp.offset_ = code_stream_start ;

    ctx.stream_ = create_read_stream(jfp, code_stream_length);

    if( ctx.stream_ == NULL ) {
        LOG_FATAL_STREAM("create read stream failed: " << file_name_) ;
        return empty ;
    }

    ctx.codec_ = opj_create_decompress(OPJ_CODEC_J2K);

    if( ctx.codec_ == nullptr )  {
        LOG_FATAL_STREAM("opj_create_decompress() failed: " << file_name_) ;
        return empty ;
    }

    opj_set_info_handler(ctx.codec_, info_callback, (void *)&file_name_);
    opj_set_warning_handler(ctx.codec_, warning_callback, (void *)&file_name_);
    opj_set_error_handler(ctx.codec_, error_callback, (void *)&file_name_);

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);

    if (! opj_setup_decoder(ctx.codec_, &parameters))
    {
        LOG_FATAL_STREAM("opj_setup_decoder() failed: " << file_name_) ;
        return empty ;
    }

    if( !opj_read_header(ctx.stream_, ctx.codec_, &ctx.image_) )
    {
        LOG_FATAL_STREAM("opj_read_header() failed" << file_name_) ;
        return empty ;
    }

    uint64_t tile = ti * n_tiles_x_ + tj ;

    if ( !opj_get_decoded_tile(ctx.codec_, ctx.stream_, ctx.image_, tile) )
    {
        LOG_FATAL_STREAM("opj_get_decoded_tile() failed" << file_name_) ;
        return empty ;
    }

    uint32_t tw = ctx.image_->comps[0].w  ;
    uint32_t th = ctx.image_->comps[0].h  ;

    uint32_t stride = tile_width_ * 4 ;
    uint64_t ts = stride * tile_height_ ;
    RasterTileData tile_data{ std::shared_ptr<uint8_t>(new uint8_t [ts], std::default_delete<uint8_t []>()), stride, tile_height_} ;

    uint8_t *buffer = tile_data.data_.get() ;
    memset(buffer, 0, ts) ;

    if ( ctx.image_->numcomps == 1) {
        for( uint32_t i=0 ; i<th ; i++)
        {
            OPJ_INT32 *src_ptr = ctx.image_->comps[0].data + i * tw ;
            uint8_t *dst_ptr = (uint8_t *)buffer + i * stride ;

            for( uint32_t j=0 ; j<tw ; j++ ) {
                uint8_t g = (uint8_t)*src_ptr++ ;
                *dst_ptr++ = g ;
                *dst_ptr++ = g ;
                *dst_ptr++ = g ;
                *dst_ptr++ = 255 ;
            }
        }
    }
    else if ( ctx.image_->numcomps == 3 ) {
        for( int k=0 ; k<3 ; k++) {
            for( uint32_t i=0 ; i<th ; i++)
            {
                OPJ_INT32 *src_ptr = ctx.image_->comps[k].data + i * tw ;
                uint8_t *dst_ptr = (uint8_t *)buffer + i * stride + k ;

                for( uint32_t j=0 ; j<tw ; j++ ) {
                    uint8_t g = (uint8_t)*src_ptr++ ;
                    *dst_ptr = g ;
                    dst_ptr += 4 ;
                }
            }
        }

        // alpha channel
        for( uint32_t i=0 ; i<th ; i++)
        {
            uint8_t *dst_ptr = (uint8_t *)buffer + i * stride + 3 ;

            for( uint32_t j=0 ; j<tw ; j++ ) {
                *dst_ptr = 255 ;
                dst_ptr += 4 ;
            }
        }
    }

    return tile_data ;

}


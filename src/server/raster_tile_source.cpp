#include "raster_tile_source.hpp"
#include "raster_tile_cache.hpp"
#include "logger.hpp"
#include "rectangle.hpp"

#include <cstring>
#include <cmath>

using namespace std;

static void copy_pixels(const uint8_t *src_buf, uint32_t src_stride,
                        uint32_t srcox, uint32_t srcoy, uint32_t srcwx, uint32_t srcwy,
                        uint8_t *dst_buf, uint32_t dst_stride, uint32_t dstox, uint32_t dstoy) {

    for(uint32_t i=0 ; i<srcwy ; i++)
    {
        const uint8_t *src_ptr = src_buf + (i + srcoy) * src_stride + 4*srcox ;
        uint8_t *dst_ptr = dst_buf + (i + dstoy) * dst_stride + dstox*4 ;

        memcpy(dst_ptr, src_ptr, 4*srcwx) ;
    }
}

#define round(x) ((x>0)?((x)+0.5):((x)-0.5))

static short cubic_kernel[] = {
    3641,3641,3640,3640,3639,3638,3636,3635,3633,3631,3629,3626,3623,3620,3617,3614,
    3610,3606,3602,3598,3593,3588,3583,3578,3573,3567,3561,3555,3549,3543,3536,3529,
    3522,3515,3508,3500,3492,3484,3476,3468,3459,3450,3441,3432,3423,3414,3404,3394,
    3384,3374,3364,3354,3343,3332,3321,3310,3299,3288,3276,3264,3252,3240,3228,3216,
    3204,3191,3178,3165,3152,3139,3126,3113,3099,3086,3072,3058,3044,3030,3016,3001,
    2987,2972,2957,2943,2928,2913,2898,2882,2867,2852,2836,2820,2805,2789,2773,2757,
    2741,2725,2708,2692,2676,2659,2643,2626,2609,2592,2576,2559,2542,2525,2507,2490,
    2473,2456,2438,2421,2403,2386,2368,2351,2333,2315,2298,2280,2262,2244,2226,2208,
    2190,2172,2154,2136,2118,2100,2082,2064,2045,2027,2009,1991,1972,1954,1936,1918,
    1899,1881,1863,1845,1826,1808,1790,1771,1753,1735,1717,1698,1680,1662,1644,1626,
    1608,1589,1571,1553,1535,1517,1499,1481,1463,1446,1428,1410,1392,1375,1357,1339,
    1322,1304,1287,1269,1252,1235,1218,1200,1183,1166,1149,1132,1115,1099,1082,1065,
    1049,1032,1016,1000, 984, 967, 951, 935, 920, 904, 888, 872, 857, 842, 826, 811,
    796, 781, 766, 751, 737, 722, 708, 694, 679, 665, 651, 637, 624, 610, 597, 583,
    570, 557, 544, 531, 519, 506, 494, 482, 470, 458, 446, 434, 423, 411, 400, 389,
    378, 368, 357, 347, 337, 327, 317, 307, 297, 288, 279, 270, 261, 252, 244, 236,
    228, 220, 212, 204, 196, 189, 181, 174, 167, 160, 153, 146, 139, 132, 126, 119,
    112, 106, 100,  94,  88,  82,  76,  70,  64,  59,  53,  48,  42,  37,  32,  27,
    22,  17,  12,   7,   3,  -2,  -6, -11, -15, -19, -24, -28, -32, -36, -39, -43,
    -47, -51, -54, -58, -61, -64, -68, -71, -74, -77, -80, -83, -85, -88, -91, -93,
    -96, -98,-101,-103,-105,-108,-110,-112,-114,-116,-118,-120,-121,-123,-125,-126,
    -128,-129,-131,-132,-133,-134,-136,-137,-138,-139,-140,-141,-142,-142,-143,-144,
    -144,-145,-146,-146,-147,-147,-147,-148,-148,-148,-148,-148,-149,-149,-149,-149,
    -148,-148,-148,-148,-148,-148,-147,-147,-147,-146,-146,-145,-145,-144,-143,-143,
    -142,-142,-141,-140,-139,-139,-138,-137,-136,-135,-134,-133,-132,-131,-130,-129,
    -128,-127,-126,-125,-123,-122,-121,-120,-119,-117,-116,-115,-113,-112,-111,-109,
    -108,-107,-105,-104,-102,-101,-100, -98, -97, -95, -94, -92, -91, -89, -88, -86,
    -85, -83, -82, -80, -79, -77, -76, -74, -73, -71, -70, -68, -66, -65, -63, -62,
    -60, -59, -57, -56, -54, -53, -52, -50, -49, -47, -46, -44, -43, -42, -40, -39,
    -38, -36, -35, -34, -32, -31, -30, -28, -27, -26, -25, -24, -23, -21, -20, -19,
    -18, -17, -16, -15, -14, -13, -12, -12, -11, -10,  -9,  -8,  -8,  -7,  -6,  -6,
    -5,  -4,  -4,  -3,  -3,  -2,  -2,  -2,  -1,  -1,  -1,  -1,   0,   0,   0,   0,   0
} ;

// 1D cubic interpolation. x coordinate is quantized to 256 bins and spline coefficients to 12 bits

static void scale_line_cubic(uint8_t *src, float offset, float scale, uint8_t *dst, uint32_t dst_len)
{
    uint64_t val ;
    int32_t ii = offset * 256 ;
    int32_t dii = scale * 256 ;

    for( uint32_t x=0 ; x<dst_len ; x++ )
    {
        val = (uint64_t)src[-1]*cubic_kernel[256+ii] ;
        val += (uint64_t)src[0]*cubic_kernel[ii] ;
        val += (uint64_t)src[1]*cubic_kernel[256-ii] ;
        val += (uint64_t)src[2]*cubic_kernel[512-ii] ;
        if ( ii == 0 ) val += (uint64_t)src[-2]*cubic_kernel[512+ii] ;

        val = (val + 2048) >> 12 ;
        if ( val<0 ) val = 0 ;
        if ( val>255 ) val = 255 ;
        dst[x] = val ;

        if ((ii+=dii) >= 256)
        {
            do {
                ii -= 256 ; src++ ;
            } while ( ii>=256 ) ;
        }
    }
}

// fast bicubic rescaling by means of separable image sampling

void scale_image(uint8_t *src_buffer, float src_offset_x, float src_offset_y, float scale_x, float scale_y, uint32_t src_w, uint32_t src_h, uint32_t src_stride,
                 uint8_t *dst_buffer, uint32_t dst_w, uint32_t dst_h, uint32_t dst_stride)
{
    int i,j ;

    uint8_t *buffer1 = new uint8_t [dst_stride * src_h] ; // hold the intermediate buffer
    uint8_t *q = new uint8_t [4*src_w], *ptr = q ;
    uint8_t *qr = ptr ; ptr += src_w ;
    uint8_t *qg = ptr ; ptr += src_w ;
    uint8_t *qb = ptr ; ptr += src_w ;
    uint8_t *qa = ptr ;

    uint8_t *p = new uint8_t [dst_w * 4] ; ptr = p ;
    uint8_t *pr = ptr ; ptr += dst_w ;
    uint8_t *pg = ptr ; ptr += dst_w ;
    uint8_t *pb = ptr ; ptr += dst_w ;
    uint8_t *pa = ptr ; ptr += dst_w ;

    // rescale across rows
    for( i=0 ; i<src_h ; i++ )
    {
        ptr = src_buffer + i * src_stride ;

        for( j = 0 ; j<src_w ; j++ ) {
            qr[j] =  *ptr++ ; qg[j] =  *ptr++ ; qb[j] =  *ptr++ ; qa[j] =  *ptr++ ;
        }

        scale_line_cubic(qr+3, src_offset_x, scale_x, pr, dst_w) ;
        scale_line_cubic(qg+3, src_offset_x, scale_x, pg, dst_w) ;
        scale_line_cubic(qb+3, src_offset_x, scale_x, pb, dst_w) ;
        scale_line_cubic(qa+3, src_offset_x, scale_x, pa, dst_w) ;

        ptr = buffer1 + i * dst_stride ;

        for( j=0 ; j<dst_w ; j++ ) {
            *ptr++ = pr[j] ; *ptr++ = pg[j] ; *ptr++ = pb[j] ; *ptr++ = pa[j] ;
        }
    }

    delete [] q ; delete [] p ;

    q = new uint8_t [4*src_h] ; ptr = q ;
    qr = ptr ; ptr += src_h ;
    qg = ptr ; ptr += src_h ;
    qb = ptr ; ptr += src_h ;
    qa = ptr ;

    p = new uint8_t [dst_h * 4] ; ptr = p ;
    pr = ptr ; ptr += dst_h ;
    pg = ptr ; ptr += dst_h ;
    pb = ptr ; ptr += dst_h ;
    pa = ptr ; ptr += dst_h ;

    for( j=0 ; j<dst_w ; j++ )
    {
        for( i=0 ; i<src_h ; i++ ) {
            qr[i] = buffer1[i * dst_stride + j*4] ;
            qg[i] = buffer1[i * dst_stride + j*4+1] ;
            qb[i] = buffer1[i * dst_stride + j*4+2] ;
            qa[i] = buffer1[i * dst_stride + j*4+3] ;
        }

        scale_line_cubic(qr+3, src_offset_y, scale_y, pr, dst_h) ;
        scale_line_cubic(qg+3, src_offset_y, scale_y, pg, dst_h) ;
        scale_line_cubic(qb+3, src_offset_y, scale_y, pb, dst_h) ;
        scale_line_cubic(qa+3, src_offset_y, scale_y, pa, dst_h) ;

        for( i=0 ; i<dst_h ; i++ ) {
            dst_buffer[i * dst_stride + 4*j] = pr[i] ;
            dst_buffer[i * dst_stride + 4*j+1] = pg[i] ;
            dst_buffer[i * dst_stride + 4*j+2] = pb[i] ;
            dst_buffer[i * dst_stride + 4*j+3] = pa[i] ;
        }
    }

    delete [] q ; delete [] p ;
    delete [] buffer1 ;
}

bool RasterTileSource::read(int32_t ox, int32_t oy, uint32_t wx, uint32_t wy, uint8_t *buffer, uint32_t stride)
{
    Rect irect(0, 0, image_width_, image_height_), srect(ox, oy, wx, wy) ;

    for( uint32_t i=0 ; i<n_tiles_y_ ; i++ ) {
        for( uint32_t j=0 ; j<n_tiles_x_ ; j++ ) {

            int32_t tox = j * tile_width_, toy = i * tile_height_ ;

            Rect trect(tox, toy, tile_width_, tile_height_) ;

            // ensure that we need this tile
            if ( !trect.intersects(srect) ) continue ;

            // try to load it from the cache
            RasterTileData tile = cache_->fetch(i, j, this) ;

            if ( !tile.data_) return false ;

            // compute the effective crop region within the tile

            Rect itr = trect.intersection(srect).intersection(irect) ;
            Rect dr = itr.translated(-ox, -oy) ;
            Rect sr = itr.translated(-tox, -toy) ;

            // do the final pixel transfer

            copy_pixels((uint8_t *)tile.data_.get(), tile.stride_, sr.x_, sr.y_, sr.width_, sr.height_,
                        buffer, stride, dr.x_, dr.y_ ) ;

            LOG_INFO_STREAM(i << ' ' << j) ;

        }
    }

    return true ;

}

// we perform nearest neighbor interpolation when downsampling, otherwise we perform bicubic interpolation on the acquired
// image region

bool RasterTileSource::read(float src_ox, float src_oy, float src_wx, float src_wy,
                            uint32_t dst_wx, uint32_t dst_wy, uint8_t *buffer, uint32_t stride) {

    double src_x_inc = src_wx/(double)dst_wx ;
    double src_y_inc = src_wy/(double)dst_wy ;

    if ( std::max(src_x_inc, src_y_inc) > 1.5 ) {

        RasterTileData ctile ;
        int32_t cti = -1, ctj = -1;

        for( int32_t i = 0; i < dst_wy ; i++ )
        {
            int32_t buf_offset, src_offset ;

            double src_y  =  (i + 0.5) * src_y_inc + src_oy;
            double src_x = 0.5 * src_x_inc + src_ox ;

            int32_t isrc_y = (int32_t) src_y ;
            int32_t ti = isrc_y/tile_height_ ;

            if ( isrc_y < 0 ) continue ;
            if ( isrc_y > image_height_ - 1 ) continue ;

            buf_offset = (int32_t)i * stride ;

            src_offset = ( isrc_y - ti * tile_height_ )  ;

            for( int32_t j = 0 ; j< dst_wx ; j++, src_x += src_x_inc )
            {
                int32_t isrc_x = (int32_t) src_x ;
                int32_t tj = isrc_x/tile_width_ ;

                if ( isrc_x < 0 ) continue ;
                if ( isrc_x > image_width_ - 1 ) continue ;

                // load or reuse tile

                RasterTileData tile ;

                if ( cti == ti && ctj == tj ) tile = ctile ;
                else {
                    tile = ctile = cache_->fetch(ti, tj, this) ;
                    cti = ti ; ctj = tj ;
                    if ( !tile.data_ ) return false ;
                }

                uint8_t *src_ptr = (uint8_t *)tile.data_.get() + src_offset * tile.stride_ + 4*(isrc_x  - tj * tile_width_) ;
                uint8_t *dst_ptr = buffer + buf_offset + j*4 ;

                *dst_ptr++ = *src_ptr++ ;
                *dst_ptr++ = *src_ptr++ ;
                *dst_ptr++ = *src_ptr++ ;
                *dst_ptr++ = *src_ptr++ ;

            }
        }
    }
    else {

        // cubic bilinear interpolation.
        // first crop the area of the image padded and then scale it.

        uint32_t padding = 3 ;
        int32_t isrc_ox = floor(src_ox) - padding, isrc_oy = floor(src_oy) - padding ;

        uint32_t isrc_wx = floor(src_wx) + 2*padding ;
        uint32_t isrc_wy = floor(src_wy) + 2*padding ;

        std::unique_ptr<uint8_t []> src_buffer(new uint8_t [isrc_wx*isrc_wy* 4]) ;
        memset(src_buffer.get(), 0, isrc_wx*isrc_wy* 4) ;

        // get the request source raster region

        if ( !read(isrc_ox, isrc_oy, isrc_wx, isrc_wy, src_buffer.get(), isrc_wx* 4) ) return false ;

        scale_image(src_buffer.get(), src_ox - isrc_ox - padding, src_oy - isrc_oy - padding, src_x_inc, src_y_inc,
                    isrc_wx, isrc_wy, 4 *isrc_wx, buffer, dst_wx, dst_wy, stride) ;
    }

    return true ;
}

#include <cairo.h>
#include <cstdio>
#include <csetjmp>
#include <jpeglib.h>
#include <string.h>

#include <stdexcept>

using namespace std ;


struct my_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */

    jmp_buf setjmp_buffer;	/* for return to caller */

    char error_buf[80] ;
};

typedef struct my_error_mgr * my_error_ptr;

static void my_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Create the message */
    (*cinfo->err->format_message) (cinfo, myerr->error_buf);

    throw std::runtime_error(myerr->error_buf) ;
}


/*
 * Here's the routine that will replace the standard error_exit method:
 */

#if 0
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}
#endif

static void destroy_image_buffer(void *data)
{
    unsigned char *buffer = (unsigned char *)data ;

    delete [] buffer ;
}

cairo_surface_t *cairo_read_jpeg(const string &fileName)
{
    FILE *fp = fopen(fileName.c_str(), "rb") ;
    if ( fp == 0 ) return 0 ;

    struct jpeg_decompress_struct dinfo;
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;

    /* We set up the normal JPEG error routines, then override error_exit. */

    dinfo.err = jpeg_std_error(&jerr.pub) ;
    jerr.pub.error_exit = my_error_exit;

    try {
        /* Now we can initialize the JPEG decompression object. */
        jpeg_create_decompress(&dinfo);

        /* Step 2: specify data source (eg, a file) */

        jpeg_stdio_src(&dinfo, fp);

        /* Step 3: read file parameters with jpeg_read_header() */

        (void)jpeg_read_header(&dinfo, true);

        unsigned int width = dinfo.image_width ;
        unsigned int height = dinfo.image_height ;
        unsigned int stride ;

        // we only support RGB

        if ( dinfo.out_color_space == JCS_RGB )
           stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width) ;
        else return 0 ;

        unsigned long imgsize = stride * height ;

        unsigned char *data = new unsigned char [imgsize] ;

        JSAMPARRAY buffer;		/* Output row buffer */

        char *dptr ;

        (void) jpeg_start_decompress(&dinfo);

        /* We can ignore the return value since suspension is not possible
         * with the stdio data source.
         */

        /* JSAMPLEs per row in output buffer */

        int row_stride = dinfo.output_width * dinfo.output_components;

        /* Make a one-row-high sample array that will go away when done with image */

        buffer = (*dinfo.mem->alloc_sarray)
              ((j_common_ptr) &dinfo, JPOOL_IMAGE, row_stride, 1);

        /* Step 6: while (scan lines remain to be read) */
        /*           jpeg_read_scanlines(...); */

        /* Here we use the library's state variable cinfo.output_scanline as the
         * loop counter, so that we don't have to keep track ourselves.
         */

        dptr = (char *)data ;

        while (dinfo.output_scanline < dinfo.output_height) {

            /* jpeg_read_scanlines expects an array of pointers to scanlines.
            * Here the array is only one element long, but you could ask for
            * more than one scanline at a time if that's more convenient.
            */

            (void) jpeg_read_scanlines(&dinfo, buffer, 1);

            /* Assume put_scanline_someplace wants a pointer and sample count. */

            memcpy(dptr, buffer[0], row_stride) ;

            dptr += stride ;
        }

        /* Step 7: Finish decompression */

        (void) jpeg_finish_decompress(&dinfo);
        /* We can ignore the return value since suspension is not possible
         * with the stdio data source.
         */

        /* Step 8: Release JPEG decompression object */

        /* This is an important step since it will release a good deal of memory. */
        jpeg_destroy_decompress(&dinfo);

        cairo_surface_t *surface_ = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_RGB24, width, height, stride) ;

        cairo_surface_write_to_png(surface_, "/tmp/ooo.png") ;

        static cairo_user_data_key_t image_buffer_key ;

        cairo_surface_set_user_data(surface_, &image_buffer_key, data, destroy_image_buffer) ;

        fclose(fp) ;

        return surface_ ;


    }
    catch (std::runtime_error &)
    {
        jpeg_destroy_decompress(&dinfo);
        fclose(fp) ;
        return 0 ;
    }

}


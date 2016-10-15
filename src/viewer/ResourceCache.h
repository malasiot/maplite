#ifndef __RESOURCE_CACHE_H__
#define __RESOURCE_CACHE_H__

#include <boost/shared_ptr.hpp>
#include <cairo.h>

#include "Style.h"
#include "svg/Rendering.h"

class ResourceCache {

public:

    struct Data {

        Data(): image_surf_(0),  glyph_(0), svg_doc_(0), font_face_(0) {}

        cairo_surface_t *image_surf_ ; // png image
        unsigned int glyph_ ;           // resolved glyph of the specific face
        svg::DocumentInstance *svg_doc_ ;
        std::vector<sld::FeatureTypeStylePtr> style_ ;
        cairo_font_face_t * font_face_ ;
    };

    bool find(const string &name, Data &res) {
        map<string, Data>::const_iterator it = cache.find(name) ;

        if ( it == cache.end() ) return false ;
        else {
            res = it->second ;
            return true ;
        }
    }

    bool save(const string &name, const Data &data) {
        cache[name] = data ;
    }

    ~ResourceCache() {
         map<string, Data>::const_iterator it = cache.begin() ;

         for( ; it != cache.end() ; ++it )
         {
             const Data &data = it->second ;

             if ( data.image_surf_)
                 cairo_surface_destroy(data.image_surf_) ;
             if ( data.svg_doc_ )
                 delete data.svg_doc_ ;
             if ( data.font_face_ )
                 cairo_font_face_destroy(data.font_face_) ;
         }


    }

private:

    map<string, Data> cache ;
};




#endif

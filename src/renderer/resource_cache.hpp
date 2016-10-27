#ifndef __RESOURCE_CACHE_H__
#define __RESOURCE_CACHE_H__

#include <boost/variant.hpp>
#include <cairo.h>
#include <memory>
#include <string>
#include <map>

#include "svg/rendering.hpp"

inline void delete_cairo_surface(cairo_surface_t *obj) {
    cairo_surface_destroy(obj) ;
}


inline void delete_cairo_font_face(cairo_font_face_t *obj) {
    cairo_font_face_destroy(obj) ;
}

typedef std::shared_ptr<cairo_surface_t> CairoSurfacePtr ;
typedef std::shared_ptr<cairo_font_face_t> CairoFontFacePtr ;

class Resource {
public:
    virtual ~Resource() {}
};

class CairoSurface: public Resource {
public:
    CairoSurface(cairo_surface_t *surf): surface_(surf) {
     }
    ~CairoSurface() {
           cairo_surface_destroy(surface_) ;
    }
    cairo_surface_t *surface_ ;
};


class CairoFontFace: public Resource {
public:

    CairoFontFace(cairo_font_face_t *face): face_(face) {}
    ~CairoFontFace() {
           cairo_font_face_destroy(face_) ;
    }
    cairo_font_face_t *face_ ;
};

class SVGDocumentResource: public Resource {

public:
    SVGDocumentResource(const std::shared_ptr<svg::DocumentInstance> &instance): instance_(instance) {}

    std::shared_ptr<svg::DocumentInstance> instance_ ;
};

typedef std::shared_ptr<Resource> ResourcePtr ;

class ResourceCache {

public:

    typedef ResourcePtr Value ;
    typedef std::string Key ;

    bool find(const Key &name, Value &res) {
        auto it = cache_.find(name) ;

        if ( it == cache_.end() ) return false ;
        else {
            res = it->second ;
            return true ;
        }
    }

    void save(const Key &name, const Value &data) {
        cache_[name] = data ;
    }

    ~ResourceCache() {}

private:

    std::map<Key, Value> cache_ ;
};




#endif

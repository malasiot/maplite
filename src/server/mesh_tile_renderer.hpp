#ifndef __MESH_TILE_RENDERER_H__
#define __MESH_TILE_RENDERER_H__

#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>

#include "geometry.hpp"
#include "dictionary.hpp"
#include "shader_config.hpp"

struct RenderingContext ;

struct DataBuffers {

    struct Channel {
        std::string name_ ;
        uint32_t dim_, offset_ ;
    };

    std::vector<Channel> channels_ ;
    std::vector<float> coords_, attr_ ;
    std::vector<GLuint> indices_ ;
};

class MeshTileRenderer {
public:

    MeshTileRenderer(const glsl::ProgramList &programs, uint32_t tile_size = 256) ;
    ~MeshTileRenderer() ;

    bool init() ;

    // will render a mesh tile encoded in protobuf string into an image and return the PNG encoded image bytes
    // options contains a list of uniform values passed to the loaded shader
    std::string render(uint32_t x, uint32_t y, uint32_t z, const std::string &tilebytes, const Dictionary &options) ;

private:

    void release() ;
    void init_buffers(const DataBuffers &data) ;
    void init_default_uniforms(const BBox &box) ;
    bool use_program(const Dictionary &params) ;
    void render();

    void clear();

private:

    uint32_t tile_size_ ;
    std::shared_ptr<RenderingContext> ctx_ ; // platform specific GL context initialization

    GLuint fbo_ = 0, texture_id_ = 0;
    GLuint vao_, coords_, attr_, indices_, pid_ ;
    GLuint elem_count_ ;
    glsl::ProgramList programs_ ;


} ;

#endif

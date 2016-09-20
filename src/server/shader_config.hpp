#ifndef __SHADER_CONFIG_HPP__
#define __SHADER_CONFIG_HPP__

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <GL/glew.h>

#include "dictionary.hpp"

namespace glsl {

/// glsl shader container
///
class Shader {
public:
    Shader(const std::string &type, const std::string &src);

    bool setup() ; // compile shader

protected:

    friend class ProgramList ;
    friend class Program ;

    std::string type_ ;  // shader type
    std::string source_ ; // source code
private:
    GLuint obj_id_ ;
};

/// glsl uniform variable

class Uniform {
public:
    Uniform(const std::string name, const std::string &param, const std::string type, const std::string &default_val) ;

    bool setup(GLuint prog_id) ;

    bool set_value(const std::string &val) ;

protected:

    friend class ProgramList ;
    friend class Program ;

    std::string name_ ;  // name of the uniform to be passed as input to the shader program
    std::string param_ ; // parameter name to be used in the request url
    std::string type_ ;  // type of the variable (glsl type string: e.g. vec3, float). Values of composite types should be encoded
                         // as a comma separated list of POD values
    std::string default_ ; // default value to be used when no url parameter present
    GLint loc_ ;
};

// glsl vertex attribute associated with the mesh vertices
// the attribute name should match the channel name in the renderer tile

class Attribute {
public:
    Attribute(const std::string name, const std::string type) ;

    bool setup() ;
private:

    friend class ProgramList ;

    std::string name_ ;
    std::string type_ ;
};

// glsl program container

class Program {
public:
    Program() = default ;

    bool build() ;
    GLuint id() const { return id_ ; }
    void set_uniforms(const Dictionary &params) ;

protected:

    friend class ProgramList ;

    std::vector<Shader> shaders_ ;
    std::vector<Uniform> uniforms_ ;
    std::vector<Attribute> attributes_ ;
    std::string name_ ;
    GLuint id_ ;
};

// Specification of all programs that the renderer supports

class ProgramList {
public:
    /// load from xml file
    bool load(const std::string &cfg_file) ;

    /// compile and link programs with GL
    bool install() ;

    /// destroy programs
    void uninstall() ;

    // list of programs indexed by name
    std::map<std::string, Program> programs_ ;
 };

}

#endif

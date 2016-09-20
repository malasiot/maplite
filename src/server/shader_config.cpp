#include "shader_config.hpp"
#include "pugixml.hpp"

#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>

using namespace std ;
namespace glsl {


Uniform::Uniform(const string name, const string &param, const string type, const string &default_val):
    name_(name), param_(param), type_(type), default_(default_val) {
}

Attribute::Attribute(const string name, const string type):
    name_(name), type_(type) {
}

Shader::Shader(const string &type, const string &src):
    type_(type), source_(src) {
}

bool ProgramList::load(const std::string &cfg_file) {

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cfg_file.c_str());

    if ( !result ) {
        cerr << "XML [" << cfg_file << "] parsed with errors\n";
        cerr << "error parsing configuration file (" <<
                cfg_file << "): " << result.description() << "\n";
        return false ;
    }

    for( pugi::xml_node p: doc.children("program") ) {
        string prog_name = p.attribute("name").as_string() ;

        Program prog ;
        prog.name_ = prog_name ;

        for( pugi::xml_node a: p.children("attribute") ) {
            string attr_name = a.attribute("name").as_string() ;
            string attr_type = a.attribute("type").as_string() ;

            if ( attr_name.empty() || attr_type.empty() ) continue ;

            Attribute attr(attr_name, attr_type) ;
            prog.attributes_.push_back(std::move(attr)) ;
        }

        for( pugi::xml_node u: p.children("uniform") ) {
            string uni_name = u.attribute("name").as_string() ;
            string uni_type = u.attribute("type").as_string() ;
            string uni_default = u.attribute("default").as_string() ;
            string uni_param = u.attribute("param").as_string() ;

            if ( uni_name.empty() || uni_type.empty() ) continue ;
            if ( uni_param.empty() ) uni_param = uni_name ;

            Uniform uni(uni_name, uni_param, uni_type, uni_default) ;
            prog.uniforms_.push_back(std::move(uni)) ;
        }

        for( pugi::xml_node s: p.children("shader") ) {
            string shader_type = s.attribute("type").as_string() ;

            string src = s.text().as_string() ;

            Shader shader(shader_type, src) ;

            prog.shaders_.push_back(std::move(shader)) ;

        }

        programs_.insert(std::make_pair(prog_name, std::move(prog))) ;

    }


    return true ;
}

bool Shader::setup() {

    if ( type_.empty() || source_.empty() ) return false ;

    GLenum shader_type ;
    if ( type_ == "fragment" ) shader_type = GL_FRAGMENT_SHADER ;
    else if ( type_ == "vertex" ) shader_type = GL_VERTEX_SHADER ;
    else if ( type_ == "geometry" ) shader_type = GL_GEOMETRY_SHADER ;
    else if ( type_ == "compute") shader_type = GL_COMPUTE_SHADER ;
    else if ( type_ == "tess_control" ) shader_type = GL_TESS_CONTROL_SHADER ;
    else if ( type_ == "tess_evaluation" ) shader_type = GL_TESS_EVALUATION_SHADER ;
    else return false ;

    obj_id_ = glCreateShader(shader_type);

    if ( obj_id_ == 0) return false ;

    const GLchar* p[1];
    p[0] = (GLchar *)source_.c_str() ;
    GLint lengths[1] = { (GLint)source_.length() };

    glShaderSource(obj_id_, 1, p, lengths);

    glCompileShader(obj_id_);

    GLint success;
    glGetShaderiv(obj_id_, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(obj_id_, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader: '%s'\n", InfoLog);
        return false ;
    }

    return true ;
}

bool Uniform::setup(GLuint pid) {
    loc_ = glGetUniformLocation(pid, name_.c_str()) ;
}

static bool decode_fv(const string &val, GLint loc, size_t dim, size_t count) {
    boost::char_separator<char> sep(",");
    using tokenizer_t = boost::tokenizer< boost::char_separator<char>> ;

    tokenizer_t tok(val, sep);

    GLfloat vf[count * dim] ;
    size_t c = 0 ;

    try {
    for( tokenizer_t::iterator beg = tok.begin();
         beg != tok.end() && c < count * dim ; ++beg, ++c ) {
        vf[c] = stof(*beg) ;
    }
    }
    catch ( std::invalid_argument & ) {
        return false ;
    }

    switch (dim) {
    case 1:
        glUniform1fv(loc, count, vf) ; break ;
    case 2:
        glUniform2fv(loc, count, vf) ; break ;
    case 3:
        glUniform3fv(loc, count, vf) ; break ;
    case 4:
        glUniform4fv(loc, count, vf) ; break ;
    defualt:
        return false ;
    }


    return true ;
}


static bool decode_iv(const string &val, GLint loc, size_t dim, size_t count) {
    boost::char_separator<char> sep(",");
    using tokenizer_t = boost::tokenizer< boost::char_separator<char>> ;

    tokenizer_t tok(val, sep);

    GLint vf[count * dim] ;
    size_t c = 0 ;

    try {
    for( tokenizer_t::iterator beg = tok.begin();
         beg != tok.end() && c < count * dim ; ++beg, ++c ) {
        if ( *beg == "true" ) vf[c] = 1 ;
        else if ( *beg == "false" ) vf[c] = 0 ;
        else vf[c] = stoi(*beg) ;
    }
    }
    catch ( std::invalid_argument & ) {
        return false ;
    }

    switch (dim) {
    case 1:
        glUniform1iv(loc, count, vf) ; break ;
    case 2:
        glUniform2iv(loc, count, vf) ; break ;
    case 3:
        glUniform3iv(loc, count, vf) ; break ;
    case 4:
        glUniform4iv(loc, count, vf) ; break ;
    defualt:
        return false ;
    }


    return true ;
}

bool Uniform::set_value(const string &val)
{
    static boost::regex rx(R"(([a-z1-4]+)(?:\[([1-9]+)\])?)") ;

    boost::smatch sm ;
    if ( !boost::regex_match(type_, sm, rx) ) return false ;

    string atype = sm.str(1) ;
    size_t asize = 1;

    if ( !sm.str(2).empty() ) {
        try {
            asize = stoi(sm.str(2));
        }
        catch ( invalid_argument &) {
            return false ;
        }
    }

    if ( atype == "float" || atype == "double" )
        return decode_fv(val, loc_, 1, asize) ;
    else if ( atype == "bool" || atype == "int" || atype == "uint")
        return decode_iv(val, loc_, 1, asize) ;
    else if ( atype == "ivec2" || atype == "uvec2" || atype == "bvec2")
        return decode_iv(val, loc_, 2, asize) ;
    else if ( atype == "vec2" || atype == "dvec2" )
        return decode_fv(val, loc_, 2, asize) ;
    else if ( atype == "ivec3" || atype == "uvec3" || atype == "bvec3")
        return decode_iv(val, loc_, 3, asize) ;
    else if ( atype == "vec3" || atype == "dvec3" )
        return decode_fv(val, loc_, 3, asize) ;
    else if ( atype == "ivec4" || atype == "uvec4" || atype == "bvec4")
        return decode_iv(val, loc_, 4, asize) ;
    else if ( atype == "vec4" || atype == "dvec4" )
        return decode_fv(val, loc_, 4, asize) ;
    else
        return false ;
}

bool Program::build() {

    id_ = glCreateProgram();

    // load and compile shaders

    for( Shader &s: shaders_ ) {

        if ( !s.setup() ) return false ;

        glAttachShader(id_, s.obj_id_);
    }

    // link program

    GLchar error_log[1024] = { 0 };

    glLinkProgram(id_) ;

    GLint success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);

    if ( success == 0 ) {
        glGetProgramInfoLog(id_, sizeof(error_log), NULL, error_log);
        fprintf(stderr, "Error linking shader program: '%s'\n", error_log);
        return false ;
    }

    glValidateProgram(id_);
    glGetProgramiv(id_, GL_VALIDATE_STATUS, &success);

    if ( !success ) {
        glGetProgramInfoLog(id_, sizeof(error_log), NULL, error_log);
        fprintf(stderr, "Invalid shader program: '%s'\n", error_log);
        return false ;
    }

    // get uniforms location

    for( auto &u: uniforms_ )
        u.setup(id_) ;

    return true ;
}

void Program::set_uniforms(const Dictionary &params)
{
    for( Uniform &u: uniforms_ ) {
        string val = params.get(u.param_) ;
        if ( val.empty() )
            u.set_value(u.default_) ;
        else
            u.set_value(val) ;
    }

}

bool ProgramList::install() {

    for( auto &pit: programs_ ) {
        if ( !pit.second.build() )
            return false ;
    }

    return true ;

}


} // namespace glsl

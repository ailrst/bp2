#include "shaders.h"
#include <fstream>
#include <vector>


bool Shader::handle_error(std::string &filename) 
{
    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &fShaderCompiled );
    if( fShaderCompiled == GL_TRUE ) {
        return false;
    }

    int maxLength = 0;
    
    //Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    
    std::vector<GLchar> err_log(maxLength);
    //Get info log
    glGetShaderInfoLog( shader, maxLength, &maxLength, &err_log[0]);

    std::cerr<< (char *)(&err_log[0]) << " (" << filename << ")" << std::endl;

    return true;
}

void Shader::add_shader(GLuint type, std::string filename)
{

    shader = glCreateShader(type);

    // read source
    std::ifstream source_file(filename, std::fstream::in);
    if (source_file.is_open()) {
        source = std::string(std::istreambuf_iterator<char>(source_file), 
                             std::istreambuf_iterator<char>());
    }

    if (!source_file.is_open() || source.size() == 0) {
        throw std::exception();
    }


    const char *cstr = source.c_str();
    glShaderSource(shader, 1, &cstr, NULL);

    glCompileShader(shader);

    if (handle_error(filename)) {
        // compilation failed
        glDeleteShader(shader);
        throw std::exception();
    }

    glAttachShader(program, shader);

}

Shader::Shader()
{
    program = glCreateProgram();
}

Shader::Shader(std::string filename)
{
    program = glCreateProgram();
    add_shader(GL_VERTEX_SHADER, filename);
}

Shader::Shader(std::string filename, std::string filename2)
{
    program = glCreateProgram();
    add_shader(GL_VERTEX_SHADER, filename);
    add_shader(GL_FRAGMENT_SHADER, filename2);
    link();
}

void Shader::link() {
        glLinkProgram(program);

        //Check for errors
        GLint programSuccess = GL_TRUE;
        glGetProgramiv( program, GL_LINK_STATUS, &programSuccess );
        if( programSuccess != GL_TRUE )
        {
            printf( "Error linking program %d!\n", program );
            throw std::exception();
        }

}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::set(const std::string &name, int value) {
    setInt(name, value);
}

void Shader::set(const std::string &name, bool value) {
    setBool(name, value);
}

void Shader::set(const std::string &name, float value) {
    setFloat(name, value);
}

void Shader::setVec3(const std::string &name, glm::vec3 vec) {
    glUniform3f(glGetUniformLocation(program, name.c_str()), vec.x, vec.y, vec.z);
}


GLint Shader::get_attrib(std::string attrib) 
{
    return glGetAttribLocation(program, attrib.c_str());
}

GLint Shader::get_uniform(std::string unif) 
{
    return glGetUniformLocation(program, unif.c_str());
}


Shader::~Shader() {
    glDeleteProgram(program);
}


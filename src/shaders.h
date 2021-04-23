
#ifndef G_SHADERSH
#define G_SHADERSH

#include <SDL2/SDL.h>
//Using SDL, SDL OpenGL, standard IO, and, strings

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>

#include <glm/vec3.hpp>

class Shader {

    std::string source;
    std::string filename;
    bool handle_error(std::string &filename);

    public:
    GLuint program = 0;
    GLuint shader;
    GLuint type;

    Shader();
    Shader(std::string filename);
    Shader(std::string filename, std::string filename2);
    ~Shader();

    void use();
    void link(); 
    void add_shader(GLuint type, std::string filename);

    void set(const std::string &name, float value);
    void set(const std::string &name, bool value);
    void set(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setVec3(const std::string &name, glm::vec3 vec); 
    GLint get_attrib(std::string attribute_name);
    GLint get_uniform(std::string uniform_name); 
};
#endif

#include <SDL2/SDL.h>
//Using SDL, SDL OpenGL, standard IO, and, strings

#include <SDL2/SDL_mouse.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>
#include <GL/glu.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <map>
#include <stack>
#include <set>

#include "shaders.h"
#include "glerror.h"

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cerr << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

// handles shaders, models, meshes for drawing a collection of similar objects
class drawing_object_handle {
    public:
    virtual void  draw(const glm::mat4 view, const glm::mat4 projection) = 0;
};


void printProgramLog( GLuint program )
{
	//Make sure name is shader
	if( glIsProgram( program ) )
	{
		//Program loglength
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		
		//Get info string length
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
		
		//Allocate string
		char* infoLog = new char[ maxLength ];
		
		//Get info log
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			printf( "%s\n", infoLog );
		}
		
		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a program\n", program );
	}
}


void set_camera(Shader *shader, glm::vec3 pos, glm::vec3 lookat, float zoom) {
    shader->setVec3("gcam.pos", pos);

    shader->setVec3("gcam.lookat", lookat);

    shader->setFloat("gcam.zoom", zoom);
};

class SDLGLGLWindow {

    public:

    Shader *shader;

    int screen_width;
    int screen_height;
    unsigned int framebuffer;
    unsigned int framebuffer_texture;
    unsigned int pixel_vertex_buffer;
    unsigned int pixel_vertex_array;

    unsigned int rbo;
    unsigned int intermediateFBO;
    unsigned int screenTexture;
    SDL_Window *gWindow;

    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 camera_direction = glm::vec3(0.f,0.f,0.f);

    float cam_pitch = 0;  
    float cam_yaw = -90;
    int num_pixels;
    
    SDLGLGLWindow(int width, int height) : screen_width(width), screen_height(height) 
    {
        num_pixels = screen_width * screen_height;

        if (!create_gl_window()) {
            exit(1);
        }

    }

    int create_gl_window() {
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, 
                SDL_GL_CONTEXT_PROFILE_CORE );


        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, 
                SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, 
                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        auto gContext = SDL_GL_CreateContext( gWindow );

        if( gContext == NULL ) {
            std::cerr <<  "OpenGL context could not be created! SDL Error: " 
                << SDL_GetError() << std::endl ;
            return false;
        }

        glewExperimental = GL_TRUE; 
        GLenum glewError = glewInit();
        if( glewError != GLEW_OK )
        {
            std::cerr << "Error initializing GLEW! " 
                << glewGetErrorString( glewError ) << std::endl;
        }

        glCheckError();
        shader = new Shader ("src/shaders/march.vert", "src/shaders/march.frag");
        shader->use();
        glCheckError();
        shader->setInt("width", screen_width);
        shader->setInt("height", screen_height);

        glCheckError();

        // FRAMEBUFFER STUFF 
        /*
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenTextures(1, &framebuffer_texture);
        glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
          
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenFramebuffers(1, &framebuffer);


        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_2D, framebuffer_texture, 0);
        // ------------------------------------------------------------- //
        glCheckError();
        */

        //Use Vsync
        if( SDL_GL_SetSwapInterval( 1 ) < 0 )
        {
            std::cerr << "Warning: Unable to set VSync! SDL Error: " 
                << SDL_GetError() << std::endl;
        }


        // PIXEL INFO --------------------------------------------------- //

            float vertices[] = {
        -1.0f, 1.0f, 0.0f  // left 
    }; 


        glGenVertexArrays(1, &pixel_vertex_array);
        glGenBuffers(1, &pixel_vertex_buffer);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(pixel_vertex_array);

        glBindBuffer(GL_ARRAY_BUFFER, pixel_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered pixel_vertex_buffer as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);



        return true;
    }

    void update_camera() {
        if (cam_pitch > 89.0f) 
            cam_pitch = 89.0f;
        if (cam_pitch < -89.0f) 
            cam_pitch = -89.0f;

        camera_direction.x = cos(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));
        camera_direction.y = sin(glm::radians(cam_pitch));
        camera_direction.z = sin(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));
        camera_front = glm::normalize(camera_direction);

        // update shader
        set_camera(shader, camera_pos, camera_front, 1.0);
    }

    void render()
    {
        glCheckError();
        shader->use();


        glClearColor(0.2f, 0.7f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glCheckError();


        glCheckError();
//        shader->setInt("width", screen_width);
 //       shader->setInt("height", screen_height);

        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glBindVertexArray(pixel_vertex_array);
//        glDrawArrays(GL_TRIANGLES, 0 , 6);
        glDrawArraysInstanced(GL_POINTS,0,1,num_pixels);
        //glDrawElementsInstanced(GL_POINTS, 0, GL_UNSIGNED_INT, 0, num_pixels);
        glCheckError();

        swap_window();
    }

    void 
    swap_window() 
    {
        SDL_GL_SwapWindow(gWindow);
    }

    ~SDLGLGLWindow() {
        //Deallocate program

        //Destroy window	
        SDL_DestroyWindow( gWindow );
        gWindow = NULL;

        delete shader;

        //Quit SDL subsystems
        SDL_Quit();
    }
};

int main(int argc, char **argv) {
    SDLGLGLWindow cont {800, 600};

    bool quit = false;

    //Event handler
    SDL_Event e;
    
    //Enable text input
   // SDL_StartTextInput();

    // SDL_SetRelativeMouseMode(SDL_TRUE);
    bool mouse_mode = true;

    std::cout << "Rendering now\n";
    glCheckError();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCheckError();
    

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                const float camera_speed = 0.11f;
                switch (e.key.keysym.scancode) {
                    case SDL_SCANCODE_SPACE:
                        mouse_mode = !mouse_mode;
                        SDL_SetRelativeMouseMode(mouse_mode ? SDL_TRUE : SDL_FALSE);
                        break;
                    case SDL_SCANCODE_Q:
                    case SDL_SCANCODE_ESCAPE:
                        quit = true;
                        break;
                    case SDL_SCANCODE_W:
                        cont.camera_pos += camera_speed * cont.camera_front;
                        break;
                    case SDL_SCANCODE_S:
                        cont.camera_pos -= camera_speed * cont.camera_front;
                        break;
                    case SDL_SCANCODE_A:
                        cont.camera_pos -= camera_speed * glm::cross(cont.camera_front, cont.camera_up);
                        break;
                    case SDL_SCANCODE_D:
                        cont.camera_pos += camera_speed * glm::cross(cont.camera_front, cont.camera_up);
                        break;
                }
            } if (e.type == SDL_MOUSEMOTION) {

                if (mouse_mode) {
                    cont.cam_pitch -= e.motion.yrel * 0.1f;
                    cont.cam_yaw += e.motion.xrel * 0.1f;
                    cont.update_camera();
                }

                static bool first = true;
                if (first) {
                    first = false;
                    continue;
                }

            }
        }

        cont.render();
    }

    return 0;
}



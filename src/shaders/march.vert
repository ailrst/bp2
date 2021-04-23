#version 330 core

layout (location = 0) in vec3 Position;
uniform int width;
uniform int height;

void main() 
{ 

    float x = 1 - (2 * (gl_InstanceID % width) / float(width));
    float y = 1 - (2 * (gl_InstanceID / width) / float(height));
    gl_Position = vec4(x,y,0,1);

}

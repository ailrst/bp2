#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float gamma = 2.2;
uniform float exposure = 0.8;

void main()
{ 
    vec3 hdrcol = texture(screenTexture, TexCoords).rgb;
    vec3 mapped = vec3(1.0) - exp((-hdrcol) * exposure);
    //vec3 mapped = hdrcol / (hdrcol + vec3(1.0));
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);

}

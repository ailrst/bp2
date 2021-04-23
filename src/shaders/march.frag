#version 330 core

uniform vec3 lightPos;


struct camera {
    vec3 pos;
    vec3 lookat;
    float zoom;
};

struct ray {
    vec3 pos;
    vec3 dir;
};

struct ray_out {
    vec3 pos;
    vec3 dir;
    vec3 norm;
    float hit;
};

uniform int width;
uniform int height;

uniform camera gcam;

out vec4 FragColor;
in ray_out frag;

void main()
{
//    FragColor = vec4(datout.hit * datout.norm, 1.0f);
    float diff  = 0.5;
    float amb = 0.2;
    float spec = 0.3;
    float alph = 5;
    vec3 speccol = vec3(1);
    vec3 diffcol = vec3(0.4, 1.0, 1.0);
    vec3 ambcol = vec3(0.4, 1.0,1.0);

    vec3 lm = normalize(frag.pos - lightPos);

    vec3 rm = 2 * dot(lm, frag.norm) * frag.norm - lm;

    vec3 v = normalize(frag.pos - gcam.pos);

    vec3 lum = ambcol * amb + diff * dot(lm, frag.norm) * diffcol + spec * pow(dot(rm, v), alph) * speccol;

    FragColor = vec4(lum, 1);
}


#version 330 core

layout (location = 0) in vec3 Position;
uniform int width;
uniform int height;


const int MAX_ITERATIONS = 200;
const int MAX_DISTANCE = 10;
const float COLLISION_EPSILON = 0.1;


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


uniform camera gcam;
out ray_out frag;

float sphere_dist(vec3 from, float r) {
    return length(from) - r;
}

ray create_camera_ray(float ux, float uy, camera cam){
    vec3 f = normalize(cam.lookat - cam.pos);
    vec3 r = cross(vec3(0.0,1.0,0.0),f);
    vec3 u = cross(f,r);    
    vec3 c=cam.pos +f* cam.zoom;
    vec3 i=c + ux*r + uy*u;
    vec3 dir=i-cam.pos;
    return ray(cam.pos,dir);
}


float bound_dist_func(vec3 pos) {
    return sphere_dist(pos, 0.09);
}

vec3 estimate_normal(vec3 p) {
    float EPSILON = 0.01;
    return normalize(vec3(
        bound_dist_func(vec3(p.x + EPSILON, p.y, p.z)) - bound_dist_func(vec3(p.x - EPSILON, p.y, p.z)),
        bound_dist_func(vec3(p.x, p.y + EPSILON, p.z)) - bound_dist_func(vec3(p.x, p.y - EPSILON, p.z)),
        bound_dist_func(vec3(p.x, p.y, p.z  + EPSILON)) - bound_dist_func(vec3(p.x, p.y, p.z - EPSILON))
    ));
}


//uniform vec3 camera_pos = vec3(2.0,1.0,0.5);
//uniform vec3 camera_lookat = vec3(0.0);
//uniform float camera_zoom = 1.0;

void main() 
{ 

    float px = 1 - (2 * (gl_InstanceID % width) / float(width));
    float py = 1 - (2 * (gl_InstanceID / width) / float(height));
    gl_Position = vec4(px,py,0,1);

    ray cr = create_camera_ray(px,py, gcam);

    float dist = bound_dist_func(cr.pos);
    float total_dist = 0;

    for (int i = 0; i < MAX_ITERATIONS && dist > COLLISION_EPSILON && total_dist
    < MAX_DISTANCE ; i++) {
        
        total_dist += dist;

        cr.pos += cr.dir * dist;

        dist = bound_dist_func(cr.pos);
    }

    if (dist <= COLLISION_EPSILON) {
        frag.hit = 1;
    } else {
        frag.hit = 0; 
    }

    frag.pos = cr.pos;
    frag.norm = estimate_normal(cr.pos);
}

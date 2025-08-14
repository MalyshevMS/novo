#version 460
layout(location = 0) in vec3 vertex_positon;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

uniform mat4 model;
uniform mat4 view_projection;

void main() {
    gl_Position =  view_projection * model * vec4(vertex_positon * 0.1f, 1.0);
};
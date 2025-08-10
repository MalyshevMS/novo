#version 460
layout(location = 0) in vec3 vertex_positon;
layout(location = 1) in vec2 texture_coord;

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view_projection;
void main() {
    tex_coord = texture_coord;
    gl_Position =  view_projection * model * vec4(vertex_positon, 1.0);
};
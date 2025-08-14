#version 460
layout(location = 0) in vec3 vertex_positon;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

out vec2 tex_coord;
out vec3 frag_normal;
out vec3 frag_position;

uniform mat4 model;
uniform mat4 view_projection;

void main() {
    vec4 v_pos_world = model * vec4(vertex_positon, 1.0);
    tex_coord = texture_coord;
    frag_normal = mat3(transpose(inverse(model))) * vertex_normal;
    frag_position = v_pos_world.xyz;
    gl_Position =  view_projection * v_pos_world;
};
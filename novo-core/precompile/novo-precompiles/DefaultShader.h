#pragma once

extern "C" {
    const char* vertex_shader = R"(
        #version 460
        layout(location = 0) in vec3 vertex_positon;
        layout(location = 1) in vec3 vertex_color;
        layout(location = 2) in vec2 texture_coord;

        out vec3 color;
        out vec2 tex_coord;
        
        uniform mat4 model;
        uniform mat4 view_projection;
        void main() {
            color = vertex_color;
            tex_coord = texture_coord;
            gl_Position =  view_projection * model * vec4(vertex_positon, 1.0);
        };
    )";

    const char* fragment_shader = R"(
        #version 460
        
        in vec3 color;
        in vec2 tex_coord;

        layout(binding = 0) uniform sampler2D InTexture;

        out vec4 frag_color;

        void main() {
            frag_color = texture(InTexture, tex_coord);
        }
    )";
}
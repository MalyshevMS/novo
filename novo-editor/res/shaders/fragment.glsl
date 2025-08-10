#version 460

in vec2 tex_coord;

layout(binding = 0) uniform sampler2D InTexture;

out vec4 frag_color;

void main() {
    frag_color = texture(InTexture, tex_coord);
}
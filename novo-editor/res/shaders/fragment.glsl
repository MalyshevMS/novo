#version 460

in vec2 tex_coord;
in vec3 frag_normal;
in vec3 frag_position;

layout(binding = 0) uniform sampler2D InTexture;

out vec4 frag_color;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;

uniform float ambient_factor;
uniform float diffuse_factor;
uniform float specular_factor;
uniform float shininess;

void main() {
    // Ambient
    vec3 ambient = ambient_factor * light_color;

    // Diffuse
    vec3 normal = normalize(frag_normal);
    vec3 light_dir = normalize(light_position - frag_position);
    vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_dir), 0.f);

    // Specular
    vec3 view_dir = normalize(camera_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.f), shininess);
    vec3 specular = specular_factor * specular_value * light_color;

    frag_color = texture(InTexture, tex_coord) * vec4(ambient + diffuse + specular, 1.f);
}
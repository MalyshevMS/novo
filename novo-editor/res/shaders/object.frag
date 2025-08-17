#version 460

in vec2 tex_coord;
in vec3 frag_normal;
in vec3 frag_position;

layout(binding = 0) uniform sampler2D InTexture;

out vec4 frag_color;

#define MAX_LIGHTS 256

uniform vec3 light_colors[MAX_LIGHTS];
uniform vec3 light_positions[MAX_LIGHTS];

uniform vec3 camera_position;

uniform float ambient_factor;
uniform float diffuse_factor;
uniform float specular_factor;
uniform float shininess;

void main() {
    vec3 total_ambient = vec3(0);
    vec3 total_diffuse = vec3(0);
    vec3 total_specular = vec3(0);

    vec3 normal = normalize(frag_normal);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (length(light_colors[i]) == 0.0) continue;

        vec3 light_dir = normalize(light_positions[i] - frag_position);
        float distance = length(light_positions[i] - frag_position);
        float distance_factor = 1.0 / (distance * distance);

        // Ambient
        total_ambient += ambient_factor * light_colors[i];

        // Diffuse
        total_diffuse += distance_factor * diffuse_factor * light_colors[i] * max(dot(normal, light_dir), 0.0);

        // Specular
        vec3 view_dir = normalize(camera_position - frag_position);
        vec3 reflect_dir = reflect(-light_dir, normal);
        float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
        total_specular += specular_factor * specular_value * light_colors[i];
    }

    frag_color = texture(InTexture, tex_coord) * vec4(total_ambient + total_diffuse + total_specular, 1.f);
}
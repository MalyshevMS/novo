extern "C" {
    const char* vertex_shader = 
        "#version 460\n"
        "layout(location = 0) in vec3 vertex_positon;"
        "layout(location = 1) in vec3 vertex_color;"
        "out vec3 color;"
        "void main() {"
        "    color = vertex_color;"
        "    gl_Position = vec4(vertex_positon, 1.0);"
        "}";

    const char* fragment_shader = 
        "#version 460\n"
        "out vec4 frag_color;"
        "in vec3 color;"
        "void main() {"
        "    frag_color = vec4(color, 1.0);"
        "}";
}
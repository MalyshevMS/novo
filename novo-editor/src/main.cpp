#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>

int main(int argc, char const *argv[]) {
    Novo::Window window = Novo::Window("Hello World", {800, 600});

    GLfloat points[] = {
        0.0, 0.5, 0.0,
        0.5, -0.5, 0.0,
        -0.5, -0.5, 0.0
    };

    GLfloat color[] = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    };

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

    GLuint vao;

    Novo::Shader shader;
    shader.addShader(vertex_shader, GL_VERTEX_SHADER);
    shader.addShader(fragment_shader, GL_FRAGMENT_SHADER);
    shader.link();

    GLuint points_vbo = 0;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint color_vbo = 0;
    glGenBuffers(1, &color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glViewport(0, 0, 800, 600);

    while (!window.shouldClose()) {
        window.update();
        glClear(GL_COLOR_BUFFER_BIT);
        shader.load();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        shader.unload();
    }
    return 0;
}

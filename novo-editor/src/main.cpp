#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>

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

        
    Novo::Shader shader;
    shader.addShader(vertex_shader, GL_VERTEX_SHADER);
    shader.addShader(fragment_shader, GL_FRAGMENT_SHADER);
    shader.link();
    
    Novo::VBO points_vbo = Novo::VBO(points, sizeof(points), Novo::VBO::Mode::STATIC);
    Novo::VBO color_vbo  = Novo::VBO(color,   sizeof(color), Novo::VBO::Mode::STATIC);
        
    Novo::VAO vao;
    
    vao.addVBO(points_vbo);
    vao.addVBO(color_vbo);

    glViewport(0, 0, 800, 600);

    while (!window.shouldClose()) {
        window.update();
        glClear(GL_COLOR_BUFFER_BIT);

        shader.load();
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        shader.unload();
    }
    return 0;
}

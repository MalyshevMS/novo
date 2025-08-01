#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <chrono>
#include <thread>

int main(int argc, char const *argv[]) {
    Novo::Window window = Novo::Window("Hello World", {800, 600});

    bool one_buffer = true;

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

    GLfloat pos_col[] = {
         0.0,  0.5, 0.0,        1.0, 1.0, 0.0,
         0.5, -0.5, 0.0,        0.0, 1.0, 1.0,
        -0.5, -0.5, 0.0,        1.0, 0.0, 1.0
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

    Novo::BufferLayout layout2
    {
        Novo::ShaderDataType::Float3,
        Novo::ShaderDataType::Float3
    };

    Novo::BufferLayout layout1 {
        Novo::ShaderDataType::Float3
    };
    
    Novo::VBO pos_col_vbo = Novo::VBO(pos_col, sizeof(pos_col), layout2);
    Novo::VBO pos_vbo = Novo::VBO(points, sizeof(points), layout1);
    Novo::VBO col_vbo = Novo::VBO(color, sizeof(color), layout1);
        
    Novo::VAO vao1;
    Novo::VAO vao2;
    
    vao2.addVBO(pos_col_vbo);

    vao1.addVBO(pos_vbo);
    vao1.addVBO(col_vbo);

    glViewport(0, 0, 800, 600);

    while (!window.shouldClose()) {
        window.update();
        glClear(GL_COLOR_BUFFER_BIT);

        if (window.isKeyPressed(GLFW_KEY_SPACE)) {
            one_buffer = !one_buffer;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        shader.load();
        if (one_buffer) vao2.bind();
        else vao1.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        shader.unload();
    }
    return 0;
}

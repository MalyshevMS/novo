#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <novo-precompiles/DefaultShader.h>
#include <chrono>
#include <thread>

int main(int argc, char const *argv[]) {
    Novo::Window window = Novo::Window("Hello World", {800, 600});
    window.setFullscreen(true);

    GLfloat pos_col[] = {
         0.0,  0.5, 0.0,        1.0, 1.0, 0.0,
         0.5, -0.5, 0.0,        0.0, 1.0, 1.0,
        -0.5, -0.5, 0.0,        1.0, 0.0, 1.0
    };
        
    Novo::Shader shader;
    shader.addShader(vertex_shader, GL_VERTEX_SHADER);
    shader.addShader(fragment_shader, GL_FRAGMENT_SHADER);
    shader.link();

    Novo::BufferLayout layout_2vec3f
    {
        Novo::ShaderDataType::Float3,
        Novo::ShaderDataType::Float3
    };
    
    Novo::VBO pos_col_vbo = Novo::VBO(pos_col, sizeof(pos_col), layout_2vec3f);
        
    Novo::VAO vao;
    
    vao.addVBO(pos_col_vbo);

    while (!window.shouldClose()) {
        window.update();
        glClear(GL_COLOR_BUFFER_BIT);

        if (window.isKeyPressed(GLFW_KEY_F11)) {
            window.setFullscreen(!window.isFullscreen());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
        }

        shader.load(); // Render zone

        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        shader.unload();
    }
    return 0;
}

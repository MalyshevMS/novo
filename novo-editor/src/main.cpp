#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <novo-core/IBO.hpp>

#include <novo-precompiles/DefaultShader.h>

#include "Debugger.hpp"

#include <chrono>
#include <thread>

int main(int argc, char const *argv[]) {
    Novo::Window window = Novo::Window("Hello World", {800, 600});
    window.setFullscreen(true);

    Debugger dbg = Debugger(window);

    GLfloat pos_col[] = {
        -0.5, -0.5, 0.0,        1.0, 1.0, 0.0,
         0.5, -0.5, 0.0,        0.0, 1.0, 1.0,
        -0.5,  0.5, 0.0,        1.0, 0.0, 1.0,
         0.5,  0.5, 0.0,        1.0, 0.0, 0.0,
    };

    GLuint indices[] = {
        0, 1, 2, 3, 2, 1
    };

    glm::vec4 bg_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        
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

    Novo::IBO ibo = Novo::IBO(indices, sizeof(indices) / sizeof(GLuint));
        
    Novo::VAO vao;
    
    vao.addVBO(pos_col_vbo);
    vao.setIBO(ibo);

    
    while (!window.shouldClose()) {
        window.update();
        glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        dbg.frame_start();

        ImGui::Begin("Background Color");
        ImGui::ColorEdit3("Color", glm::value_ptr(bg_color));
        ImGui::End();

        dbg.frame_end();

        if (window.isKeyPressed(GLFW_KEY_F11)) {
            window.setFullscreen(!window.isFullscreen());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
        }

        shader.load(); // Render zone

        vao.bind();
        glDrawElements(GL_TRIANGLES, vao.getIndCount(), GL_UNSIGNED_INT, nullptr);

        shader.unload();
    }
    return 0;
}

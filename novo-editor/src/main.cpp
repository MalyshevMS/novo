#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <novo-core/IBO.hpp>

#include <novo-core/Camera.hpp>

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

    glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
    float rotation = 0.f;
    glm::vec3 translate = glm::vec3(0.f, 0.f, 0.f);

    glm::vec3 camera_pos = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 camera_rot = glm::vec3(0.f, 0.f, 0.f);
    bool perspective = true;

    glm::vec4 bg_color = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);
    glm::mat4 model = glm::mat4(1.f);

    Novo::Camera camera;
        
    Novo::Shader shader;
    shader.addShader(vertex_shader, GL_VERTEX_SHADER);
    shader.addShader(fragment_shader, GL_FRAGMENT_SHADER);
    shader.link();

    Novo::BufferLayout layout_2vec3f {
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

        if (window.isKeyPressed(GLFW_KEY_F11)) {
            window.setFullscreen(!window.isFullscreen());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
            return 0;
        }

        shader.load(); // Render zone

        glm::mat4 scale_mat = glm::scale(glm::mat4(1.f), scale);
        float rotation_rad = glm::radians(rotation);
        glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.f), rotation_rad, glm::vec3(0.f, 0.f, 1.f));
        glm::mat4 translate_mat = glm::translate(glm::mat4(1.f), translate);

        camera.set_position_rotation(camera_pos, camera_rot);
        camera.set_projection_mode(perspective ? Novo::Camera::CameraType::Perspective : Novo::Camera::CameraType::Orthographic);

        model = translate_mat * rotate_mat * scale_mat;

        shader.setUniform("model", model);
        shader.setUniform("view_projection", camera.get_view_proj_matrix());

        vao.bind();
        glDrawElements(GL_TRIANGLES, vao.getIndCount(), GL_UNSIGNED_INT, nullptr);

        shader.unload();

        dbg.frame_start();

        ImGui::Begin("Debugger");
        ImGui::ColorEdit3("Background Color", glm::value_ptr(bg_color));

        ImGui::SliderFloat3("Translation", glm::value_ptr(translate), -1.f, 1.f);
        ImGui::SliderFloat("Rotation", &rotation, 0.f, 360.f);
        ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 0.f, 2.f);
        if (ImGui::Button("Exit")) {
            window.close();
            return 0;
        }
        ImGui::End();

        ImGui::Begin("Camera");
        ImGui::SliderFloat3("Position", glm::value_ptr(camera_pos), -10.f, 10.f);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(camera_rot), -180.f, 180.f);
        ImGui::Checkbox("Perspective", &perspective);
        ImGui::End();

        dbg.frame_end();
    }
    return 0;
}

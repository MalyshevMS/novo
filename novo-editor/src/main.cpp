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

#define CUBE_COLOR1 1.f, 0.5f, 0.5f
#define CUBE_COLOR2 0.5f, 1.f, 0.5f

int main(int argc, char const *argv[]) {
    Novo::Window window = Novo::Window("Hello World", {800, 600});
    window.setFullscreen(true);

    Debugger dbg = Debugger(window);

    GLfloat pos_col[] = {
        -1.f, -1.f, 0.f,        CUBE_COLOR1,
         1.f, -1.f, 0.f,        CUBE_COLOR2,
         -1.f,  1.f, 0.f,       CUBE_COLOR1,
         1.f,  1.f, 0.f,        CUBE_COLOR2,

         -1.f, -1.f, 2.f,       CUBE_COLOR2,
         1.f, -1.f, 2.f,        CUBE_COLOR1,
        -1.f,  1.f, 2.f,        CUBE_COLOR2,
         1.f,  1.f, 2.f,        CUBE_COLOR1,
    };

    GLuint indices[] = {
        0, 1, 2, 3, 2, 1,
        4, 5, 6, 7, 6, 5,
        0, 2, 4, 6, 4, 2,
        1, 3, 5, 7, 5, 3
    };

    glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
    float rotation = 0.f;
    glm::vec3 translate = glm::vec3(0.f, 0.f, 0.f);

    glm::vec3 camera_pos = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 camera_rot = glm::vec3(0.f, 0.f, 0.f);
    float camera_fov = 90.f;
    bool perspective = true;

    bool debug = false;

    glm::vec4 bg_color = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);
    glm::mat4 model = glm::mat4(1.f);

    Novo::Camera camera = Novo::Camera(camera_pos, camera_rot, Novo::Camera::CameraType::Perspective, 90, window.getSize().x / window.getSize().y);
        
    Novo::Shader shader;
    shader.addShader(vertex_shader, GL_VERTEX_SHADER);
    shader.addShader(fragment_shader, GL_FRAGMENT_SHADER);
    shader.link();

    Novo::BufferLayout layout_2vec3f {
        Novo::ShaderDataType::Float3,
        Novo::ShaderDataType::Float3,
    };
    
    Novo::VBO pos_col_vbo = Novo::VBO(pos_col, sizeof(pos_col), layout_2vec3f);
    Novo::IBO ibo = Novo::IBO(indices, sizeof(indices) / sizeof(GLuint));
    Novo::VAO vao;
    
    vao.addVBO(pos_col_vbo);
    vao.setIBO(ibo);

    double lastMouseX = 0.0, lastMouseY = 0.0;
    bool mouseLocked = false;
    
    while (!window.shouldClose()) {
        window.update();
        glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        glClear(GL_COLOR_BUFFER_BIT);

        // Camera movement (along -Z axis)
        float radian_yaw   = glm::radians(camera_rot.y);
        float radian_pitch = glm::radians(camera_rot.x);
        glm::vec3 dir = glm::vec3(
        -sin(radian_yaw) * cos(radian_pitch),
        sin(radian_pitch),
        -cos(radian_yaw) * cos(radian_pitch)
        );

        if (window.isKeyPressed(GLFW_KEY_F11)) {
            window.setFullscreen(!window.isFullscreen());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
            window.close();
            return 0;
        }

        if (window.isKeyPressed(GLFW_KEY_W)) {
            camera_pos += dir * 0.1f;
        }

        if (window.isKeyPressed(GLFW_KEY_S)) {
            camera_pos -= dir * 0.1f;
        }

        if (window.isKeyPressed(GLFW_KEY_A)) {
            camera_pos -= glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f))) * 0.1f;
        }

        if (window.isKeyPressed(GLFW_KEY_D)) {
            camera_pos += glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f))) * 0.1f;
        }

        if (window.isKeyPressed(GLFW_KEY_LEFT)) {
            camera_rot.y += 1.f;
        }

        if (window.isKeyPressed(GLFW_KEY_RIGHT)) {
            camera_rot.y -= 1.f;
        }

        if (window.isKeyPressed(GLFW_KEY_UP)) {
            camera_pos.y += 0.1f;
        }

        if (window.isKeyPressed(GLFW_KEY_DOWN)) {
            camera_pos.y -= 0.1f;
        }

        if (window.isKeyPressed(GLFW_KEY_X)) {
            double mouseX, mouseY;
            glfwGetCursorPos(window.getHandle(), &mouseX, &mouseY);

            if (!mouseLocked) {
                lastMouseX = mouseX;
                lastMouseY = mouseY;
                mouseLocked = true;
            }

            float sensitivity = 0.1f;
            float deltaX = (mouseX - lastMouseX) * sensitivity;
            float deltaY = (mouseY - lastMouseY) * sensitivity;

            camera_rot.y += deltaX;
            camera_rot.x -= deltaY;

            camera_rot.x = glm::clamp(camera_rot.x, -89.0f, 89.0f);

            lastMouseX = mouseX;
            lastMouseY = mouseY;

            glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseLocked = false;
        }

        shader.load(); // Render zone

        glm::mat4 scale_mat = glm::scale(glm::mat4(1.f), scale);
        float rotation_rad = glm::radians(rotation);
        glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.f), rotation_rad, glm::vec3(0.f, 0.f, 1.f));
        glm::mat4 translate_mat = glm::translate(glm::mat4(1.f), translate);

        camera.set_position_rotation(camera_pos, camera_rot);
        camera.set_projection_mode(perspective ? Novo::Camera::CameraType::Perspective : Novo::Camera::CameraType::Orthographic);
        camera.set_fov(camera_fov);

        model = translate_mat * rotate_mat * scale_mat;

        shader.setUniform("model", model);
        shader.setUniform("view_projection", camera.get_view_proj_matrix());

        vao.bind();
        glDrawElements(debug ? GL_LINES : GL_TRIANGLES, vao.getIndCount(), GL_UNSIGNED_INT, nullptr);

        shader.unload(); // Render zone end

        dbg.frame_start();

        ImGui::Begin("Debugger");
        ImGui::ColorEdit3("Background Color", glm::value_ptr(bg_color));

        ImGui::SliderFloat3("Translation", glm::value_ptr(translate), -1.f, 1.f);
        ImGui::SliderFloat("Rotation", &rotation, 0.f, 360.f);
        ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 0.f, 2.f);
        ImGui::Checkbox("Debug", &debug);
        if (ImGui::Button("Exit")) {
            window.close();
            return 0;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Begin("Camera");
        ImGui::SliderFloat3("Position", glm::value_ptr(camera_pos), -10.f, 10.f);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(camera_rot), -180.f, 180.f);
        ImGui::SliderFloat("FOV", &camera_fov, 1.f, 179.f);
        ImGui::Checkbox("Perspective", &perspective);
        if (ImGui::Button("Reset")) {
            camera_pos = glm::vec3(0.f, 0.f, 1.f);
            camera_rot = glm::vec3(0.f, 0.f, 0.f);
        }
        if (ImGui::Button("Reset Position")) {
            camera_pos = glm::vec3(0.f, 0.f, 1.f);
        }
        if (ImGui::Button("Reset Rotation")) {
            camera_rot = glm::vec3(0.f, 0.f, 0.f);
        }
        ImGui::End();

        dbg.frame_end();
    }
    return 0;
}

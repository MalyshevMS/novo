#include <novo-core/Application.hpp>
#include <novo-precompiles/Layouts.h>

#include "Debugger.hpp"

#include <memory>
#include <chrono>
#include <thread>

#define CUBE_COLOR1 1.f, 0.f, 0.f
#define CUBE_COLOR2 0.f, 1.f, 0.f

class Application : public Novo::Application {
private:
    std::unique_ptr<Novo::Window> p_window = nullptr;
    std::unique_ptr<Novo::Camera> p_camera = nullptr;

    std::unique_ptr<Novo::VAO> p_vao = nullptr;
    std::unique_ptr<Novo::VBO> p_vbo = nullptr;
    std::unique_ptr<Novo::IBO> p_ibo = nullptr;
    std::unique_ptr<Novo::Shader> p_shader = nullptr;

    std::unique_ptr<Debugger> p_debugger = nullptr;

    glm::mat4 g_model = glm::mat4(1.f);
    glm::vec4 g_bgColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);

    glm::vec3 c_pos = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 c_rot = glm::vec3(0.f, 0.f, 0.f);
    float c_fov = 90.f;
    bool c_perspective = true;
    float c_speed = 10.f;

    GLfloat pos_col[48] = {
        -1.f, -1.f, 0.f,        CUBE_COLOR1,
         1.f, -1.f, 0.f,        CUBE_COLOR2,
         -1.f,  1.f, 0.f,       CUBE_COLOR1,
         1.f,  1.f, 0.f,        CUBE_COLOR2,

         -1.f, -1.f, 2.f,       CUBE_COLOR2,
         1.f, -1.f, 2.f,        CUBE_COLOR1,
        -1.f,  1.f, 2.f,        CUBE_COLOR2,
         1.f,  1.f, 2.f,        CUBE_COLOR1,
    };

    GLuint indices[24] = {
        0, 1, 2, 3, 2, 1,
        4, 5, 6, 7, 6, 5,
        0, 2, 4, 6, 4, 2,
        1, 3, 5, 7, 5, 3
    };

    bool b_debug = false;
public:
    virtual void init() override {
        p_window = std::make_unique<Novo::Window>("Novo", glm::vec2(1280, 720));
        p_window->setFullscreen(true);

        p_camera = std::make_unique<Novo::Camera>(c_pos, c_rot, Novo::Camera::CameraType::Perspective, c_fov, p_window->getAspectRatio());
        p_shader = std::make_unique<Novo::Shader>();
        p_shader->addShader(vertex_shader, GL_VERTEX_SHADER);
        p_shader->addShader(fragment_shader, GL_FRAGMENT_SHADER);
        p_shader->link();

        p_vbo = std::make_unique<Novo::VBO>(pos_col, sizeof(pos_col), Novo::Layout::l_2vec3f);
        p_ibo = std::make_unique<Novo::IBO>(indices, sizeof(indices) / sizeof(GLuint));
        p_vao = std::make_unique<Novo::VAO>();

        p_vao->addVBO(*p_vbo);
        p_vao->setIBO(*p_ibo);

        p_debugger = std::make_unique<Debugger>(*p_window);
    }

    virtual void on_update() override {
        while (p_window->isOpen()) {
            p_window->update();
            glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);
            glClear(GL_COLOR_BUFFER_BIT);

            p_shader->load();

            // Camera matrix
            p_camera->set_position_rotation(c_pos, c_rot);
            p_camera->set_fov(c_fov);
            p_camera->set_projection_mode(c_perspective ? Novo::Camera::CameraType::Perspective : Novo::Camera::CameraType::Orthographic);

            p_shader->setUniform("model", g_model);
            p_shader->setUniform("view_projection", p_camera->get_view_proj_matrix());

            p_vao->bind();
            glDrawElements(GL_TRIANGLES, p_vao->getIndCount(), GL_UNSIGNED_INT, nullptr);

            p_shader->unload();

            key_pressed();
            debug();
        }
        glfwTerminate();
    }

    virtual void key_pressed() override {
        float radian_yaw   = glm::radians(c_rot.y);
        float radian_pitch = 0.f;
        glm::vec3 dir = glm::vec3(
            -sin(radian_yaw) * cos(radian_pitch),
            sin(radian_pitch),
            -cos(radian_yaw) * cos(radian_pitch)
        );

        if (p_window->isKeyPressed(GLFW_KEY_ESCAPE)) {
            p_window->close();
        }

        if (p_window->isKeyPressed(GLFW_KEY_W)) {
            c_pos += dir * c_speed * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_S)) {
            c_pos -= dir * c_speed * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_A)) {
            c_pos -= glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f))) * c_speed * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_D)) {
            c_pos += glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f))) * c_speed * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_LEFT)) {
            c_rot.y += 100.f * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_RIGHT)) {
            c_rot.y -= 100.f * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_UP)) {
            c_pos.y += c_speed * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_DOWN)) {
            c_pos.y -= c_speed * p_window->getDeltaTime();
        }

        if (p_window->isKeyPressed(GLFW_KEY_G)) {
            b_debug = !b_debug;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void debug() {
        if (b_debug) {
            p_debugger->frame_start();

            ImGui::Begin("Background");
            ImGui::ColorEdit3("Background color", glm::value_ptr(g_bgColor));
            ImGui::End();

            ImGui::Begin("Camera");
            ImGui::DragFloat3("Position", glm::value_ptr(c_pos), 0.1f);
            ImGui::DragFloat("Rotation Y", &c_rot.y);
            ImGui::DragFloat("FOV", &c_fov);
            ImGui::End();

            p_debugger->frame_end();
        }
    }
};
#include <novo-core/Application.hpp>
#include <novo-precompiles/Layouts.h>

#include "Debugger.hpp"

#include <memory>
#include <chrono>
#include <thread>

#define COLOR1 1.f, 0.f, 0.f
#define COLOR2 0.f, 1.f, 0.f
#define COLOR3 0.f, 0.f, 1.f
#define COLOR4 1.f, 1.f, 1.f

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

    glm::vec3 c_pos = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 c_rot = glm::vec3(0.f, 0.f, 0.f);
    float c_fov = 90.f;
    bool c_perspective = true;
    float c_speed = 10.f;
    float c_sensitivity = 3.f;

    GLfloat pos_col[24] = {
        0, -1,-1,     COLOR1,
        0, 1, -1,     COLOR2,
        0, -1, 1,     COLOR3,
        0, 1,  1,     COLOR4,
    };

    GLuint indices[6] = {
        0, 1, 2,
        3, 1, 2
    };

    bool b_debug = false;
public:
    virtual void init() override {
        p_window = std::make_unique<Novo::Window>("Novo", glm::vec2(1920, 1080));

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
            p_camera->set_fov(c_fov);
            p_camera->set_projection_mode(c_perspective ? Novo::Camera::CameraType::Perspective : Novo::Camera::CameraType::Orthographic);

            g_model = glm::mat4(1.f);
            p_shader->setUniform("model", g_model);
            p_shader->setUniform("view_projection", p_camera->get_view_proj_matrix());

            p_vao->draw();

            p_shader->unload();

            key_pressed();
            debug();
        }
        glfwTerminate();
    }

    virtual void key_pressed() override {
        glm::vec3 movement_delta = glm::vec3();
        glm::vec3 rotation_delta = glm::vec3();
        float deltaTime = p_window->getDeltaTime();

        if (p_window->isKeyPressed(GLFW_KEY_ESCAPE)) {
            p_window->close();
        }

        if (p_window->isKeyPressed(GLFW_KEY_W)) {
            movement_delta.x += c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_S)) {
            movement_delta.x -= c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_A)) {
            movement_delta.y += c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_D)) {
            movement_delta.y -= c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_E)) {
            movement_delta.z += c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_Q)) {
            movement_delta.z -= c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_LEFT)) {
            rotation_delta.z -= c_sensitivity * 75.f * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_RIGHT)) {
            rotation_delta.z += c_sensitivity * 75.f * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_UP)) {
            rotation_delta.y += c_sensitivity * 75.f * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_DOWN)) {
            rotation_delta.y -= c_sensitivity * 75.f * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_P)) {
            rotation_delta.x -= c_sensitivity * 75.f * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_O)) {
            rotation_delta.x += c_sensitivity * 75.f * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_G)) {
            b_debug = !b_debug;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        p_camera->move_rotate(movement_delta, rotation_delta);
    }

    void debug() {
        if (b_debug) {
            c_pos = p_camera->get_position();
            c_rot = p_camera->get_rotation();

            p_debugger->frame_start();

            ImGui::Begin("Background");
            ImGui::SetWindowFontScale(1.5f);
            ImGui::ColorEdit3("Background color", glm::value_ptr(g_bgColor));
            ImGui::End();

            ImGui::Begin("Camera");
            ImGui::SetWindowFontScale(1.5f);
            ImGui::DragFloat("FOV", &c_fov);
            if (ImGui::DragFloat3("Position", glm::value_ptr(c_pos))) {
                p_camera->set_position(c_pos);
            }

            if (ImGui::DragFloat3("Rotation", glm::value_ptr(c_rot))) {
                p_camera->set_rotation(c_rot);
            }

            if (ImGui::Button("Reset Position")) {
                p_camera->set_position(glm::vec3(0.f, 0.f, 0.f));
            }

            if (ImGui::Button("Reset Rotation")) {
                p_camera->set_rotation(glm::vec3(0.f, 0.f, 0.f));
            }
            ImGui::End();

            p_debugger->frame_end();
        }
    }
};
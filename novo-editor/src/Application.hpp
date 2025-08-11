#include <novo-core/Application.hpp>
#include <novo-precompiles/Layouts.h>

#include "Debugger.hpp"

#include <memory>
#include <chrono>
#include <thread>

class NovoEditor : public Novo::Application {
private:
    std::unique_ptr<Novo::Window> p_window = nullptr;
    std::unique_ptr<Novo::Camera> p_camera = nullptr;

    std::unique_ptr<Novo::VAO> p_vao = nullptr;
    std::unique_ptr<Novo::VBO> p_vbo = nullptr;
    std::unique_ptr<Novo::IBO> p_ibo = nullptr;

    std::unique_ptr<Novo::Shader> p_shader = nullptr;
    
    std::unique_ptr<Novo::Resources> p_resources = nullptr;

    std::unique_ptr<Debugger> p_debugger = nullptr;

    glm::mat4 g_model = glm::mat4(1.f);
    glm::vec4 g_bgColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);

    glm::vec3 c_pos = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 c_rot = glm::vec3(0.f, 0.f, 0.f);
    float c_fov = 90.f;
    float c_speed = 10.f;
    float c_sensitivity = 5.f;

    GLfloat pos_col[20 * 2] = {
        -1, -1, -1,     1.f,   0.f,
        1, -1, -1,      0.f,   0.f,
        -1, 1, -1,      1.f,   1.f,
        1,  1, -1,      0.f,   1.f,

        -1, -1, 1,      1.f,   0.f,
        1, -1,  1,      0.f,   0.f,
        -1, 1,  1,      1.f,   1.f,
        1,  1,  1,      0.f,   1.f,
    };

    GLuint indices[6 * 2] = {
        0, 1, 2, 3, 1, 2,
        4, 5, 6, 7, 5, 6,
    };

    bool b_debug = false;
public:
    virtual void init() override {
        p_window = std::make_unique<Novo::Window>("Novo", glm::vec2(1920, 1080));
        p_window->setMaximized(true);

        p_resources = std::make_unique<Novo::Resources>(__argv[0]);

        p_camera = std::make_unique<Novo::Camera>(c_pos, c_rot, Novo::Camera::CameraType::Perspective, c_fov, p_window->getAspectRatio());
        p_shader = std::make_unique<Novo::Shader>();
        p_shader->addShader(p_resources->getFileStr("res/shaders/vertex.glsl"), GL_VERTEX_SHADER);
        p_shader->addShader(p_resources->getFileStr("res/shaders/fragment.glsl"), GL_FRAGMENT_SHADER);
        p_shader->link();

        p_vbo = std::make_unique<Novo::VBO>(pos_col, sizeof(pos_col), Novo::Layout::l_texture);
        p_ibo = std::make_unique<Novo::IBO>(indices, sizeof(indices) / sizeof(GLuint));
        p_vao = std::make_unique<Novo::VAO>();

        p_vao->addVBO(*p_vbo);
        p_vao->setIBO(*p_ibo);

        p_debugger = std::make_unique<Debugger>(*p_window);

        p_resources->loadTexture("Texture", "res/textures/texture.png")->setMagFilter(GL_NEAREST);
    }

    virtual void on_update() override {
        while (p_window->isOpen()) {
            p_window->update();
            glEnable(GL_DEPTH_TEST);
            glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            p_shader->load();

            p_resources->getTexture("Texture")->bind();

            // Camera matrix
            p_camera->set_fov(c_fov);
            p_camera->set_projection_mode(Novo::Camera::CameraType::Perspective);

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
        glm::vec2 mouseDelta = p_window->getMouseDelta();

        if (p_window->isKeyPressed(GLFW_KEY_ESCAPE)) {
            p_window->close();
        }

        if (p_window->isKeyPressed(GLFW_KEY_W)) {
            movement_delta.z += c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_S)) {
            movement_delta.z -= c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_A)) {
            movement_delta.x += c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_D)) {
            movement_delta.x -= c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_E)) {
            movement_delta.y += c_speed * deltaTime;
        }

        if (p_window->isKeyPressed(GLFW_KEY_Q)) {
            movement_delta.y -= c_speed * deltaTime;
        }
        
        if (p_window->isMousePressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            p_window->hideCursor();

            if (p_window->isMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
                movement_delta.x -= mouseDelta.x * (c_speed / 2) * deltaTime;
                movement_delta.y -= mouseDelta.y * (c_speed / 2) * deltaTime;
            } else {
                rotation_delta.x += mouseDelta.y * c_sensitivity * deltaTime;
                rotation_delta.y -= mouseDelta.x * c_sensitivity * deltaTime;
            }
        } else {
            p_window->showCursor();
        }

        if (p_window->isKeyPressed(GLFW_KEY_TAB)) {
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

            ImGui::DragFloat("Sensivity", &c_sensitivity, 0.01f, 0.f, 10.f, "%.2f");
            ImGui::DragFloat("Speed", &c_speed, 0.01f, 0.f, 10.f, "%.2f");

            if (ImGui::Button("Reset Position")) {
                p_camera->set_position(glm::vec3(0.f, 0.f, -1.f));
            }

            if (ImGui::Button("Reset Rotation")) {
                p_camera->set_rotation(glm::vec3(0.f, 0.f, 0.f));
            }

            ImGui::End();

            p_debugger->frame_end();
        }
    }
};
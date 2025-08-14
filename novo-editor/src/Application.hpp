#include <novo-core/Application.hpp>
#include <novo-precompiles/Layouts.h>

#include "Debugger.hpp"

#include <memory>
#include <chrono>
#include <thread>

class NovoEditor : public Novo::Application {
private:
    std::unique_ptr<Novo::Window> p_window = nullptr;
    std::unique_ptr<Novo::Resources> p_resources = nullptr;
    std::unique_ptr<Debugger> p_debugger = nullptr;

    std::shared_ptr<Novo::Camera> p_camera = nullptr;
    std::shared_ptr<Novo::Shader> p_shader = nullptr;

    std::unique_ptr<Novo::Mesh::Box> p_box1 = nullptr;
    std::unique_ptr<Novo::Mesh::Box> p_box2 = nullptr;
    std::unique_ptr<Novo::Mesh::Plane> p_plane1 = nullptr;
    std::unique_ptr<Novo::Mesh::Plane> p_plane2 = nullptr;
    std::unique_ptr<Novo::Mesh::Triangle> p_triangle = nullptr;

    glm::mat4 g_model = glm::mat4(1.f);
    glm::vec4 g_bgColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);

    glm::vec3 c_pos = glm::vec3(0.f, 2.f, -3.f);
    glm::vec3 c_rot = glm::vec3(0.f, 0.f, 0.f);
    float c_fov = 90.f;
    float c_speed = 10.f;
    float c_sensitivity = 5.f;

    bool b_debug = false;
public:
    virtual void init(const char** argv) override {
        p_window = std::make_unique<Novo::Window>("Novo", glm::vec2(1920, 1080));
        p_window->setMaximized(true);

        p_resources = std::make_unique<Novo::Resources>(argv[0]);

        p_camera = std::make_shared<Novo::Camera>(c_pos, c_rot, Novo::Camera::CameraType::Perspective, c_fov, p_window->getAspectRatio());
        p_shader = std::make_shared<Novo::Shader>();
        p_shader->addShader(p_resources->getFileStr("res/shaders/vertex.glsl"), GL_VERTEX_SHADER);
        p_shader->addShader(p_resources->getFileStr("res/shaders/fragment.glsl"), GL_FRAGMENT_SHADER);
        p_shader->link();

        p_debugger = std::make_unique<Debugger>(*p_window);

        auto test_texture = p_resources->loadTexture("Texture", "res/textures/texture.png");
        auto box_texture = p_resources->loadTexture("Box", "res/textures/box_texture.png");
        auto grass_texture = p_resources->loadTexture("Grass", "res/textures/grass.png");

        test_texture->setMagFilter(GL_NEAREST);

        p_plane1 = std::make_unique<Novo::Mesh::Plane>(grass_texture, p_shader, glm::vec3(2.f, -1.f, 0.f));
        p_plane2 = std::make_unique<Novo::Mesh::Plane>(box_texture, p_shader, glm::vec3(3.f, -1.f, 5.f));
        
        p_box1 = std::make_unique<Novo::Mesh::Box>(test_texture, p_shader);
        p_box2 = std::make_unique<Novo::Mesh::Box>(box_texture, p_shader, glm::vec3(5.f, 0.f, 0.f));

        p_triangle = std::make_unique<Novo::Mesh::Triangle>(box_texture, p_shader, glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 3.f, 0.f));
        
        p_box1->reload();
        p_box2->reload();
        p_plane1->reload();
        p_plane2->reload();
        p_triangle->reload();
    }

    virtual void on_update() override {
        while (p_window->isOpen()) {
            p_window->update();
            glEnable(GL_DEPTH_TEST);
            glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            p_box1->draw(p_camera);
            p_box2->draw(p_camera);
            p_triangle->draw(p_camera);
            p_plane1->draw(p_camera);
            p_plane2->draw(p_camera);

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

        if (p_window->isKeyPressed(GLFW_KEY_R)) {
            p_box1->set_uv(glm::vec2(1.f, 1.f));
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
            if (ImGui::DragFloat("FOV", &c_fov)) {
                p_camera->set_fov(c_fov);
            }

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

            p_box1->draw_ui("Box 1");
            p_box2->draw_ui("Box 2");
            p_plane1->draw_ui("Plane 1");
            p_plane2->draw_ui("Plane 2");
            p_triangle->draw_ui("Triangle");

            p_debugger->frame_end();
        }
    }
};
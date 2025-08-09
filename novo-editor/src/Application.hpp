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

class NovoEditor : public Novo::Application {
private:
    std::unique_ptr<Novo::Window> p_window = nullptr;
    std::unique_ptr<Novo::Camera> p_camera = nullptr;

    std::unique_ptr<Novo::VAO> p_vao = nullptr;
    std::unique_ptr<Novo::VBO> p_vbo = nullptr;
    std::unique_ptr<Novo::IBO> p_ibo = nullptr;

    std::unique_ptr<Novo::Shader> p_shader = nullptr;

    std::unique_ptr<Novo::Texture2D> p_texture_smile = nullptr;
    std::unique_ptr<Novo::Texture2D> p_texture_quads = nullptr;

    std::unique_ptr<Debugger> p_debugger = nullptr;

    glm::mat4 g_model = glm::mat4(1.f);
    glm::vec4 g_bgColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);

    glm::vec3 c_pos = glm::vec3(-1.f, 0.f, 0.f);
    glm::vec3 c_rot = glm::vec3(0.f, 0.f, 0.f);
    float c_fov = 90.f;
    bool c_perspective = true;
    float c_speed = 10.f;
    float c_sensitivity = 5.f;

    GLfloat pos_col[32] = {
        0, -1,-1,     COLOR1,   -2.f, 2.f,
        0, 1, -1,     COLOR2,   2.f, 2.f,
        0, -1, 1,     COLOR3,   -2.f, -2.f,
        0, 1,  1,     COLOR4,   2.f, -2.f
    };

    GLuint indices[6] = {
        0, 1, 2,
        3, 1, 2
    };

    bool b_debug = false;
    bool b_texture = false; // false = smile, true = quad

    GLuint texture_smile;
    GLuint texture_quads;

    void generate_circle(unsigned char* data,
                         const unsigned int width,
                         const unsigned int height,
                         const unsigned int center_x,
                         const unsigned int center_y,
                         const unsigned int radius,
                         const unsigned char color_r,
                         const unsigned char color_g,
                         const unsigned char color_b) {
        for (unsigned int x = 0; x < width; x++) {
            for (unsigned int y = 0; y < height; y++) {
                if ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) < radius * radius) {
                    data[(x + y * width) * 3 + 0] = color_r;
                    data[(x + y * width) * 3 + 1] = color_g;
                    data[(x + y * width) * 3 + 2] = color_b;
                }
            }
        }
    }

    void smile_texture(unsigned char* data, unsigned int width, unsigned int height) {
        // Background
        for (unsigned int x = 0; x < width; x++) {
            for (unsigned int y = 0; y < height; y++) {
                data[(x + y * width) * 3 + 0] = 200;
                data[(x + y * width) * 3 + 1] = 191;
                data[(x + y * width) * 3 + 2] = 231;
            }
        }

        // Face
        generate_circle(data, width, height, width * 0.5, height * 0.5, width * 0.4, 255, 255, 0);

        // Smile
        generate_circle(data, width, height, width * 0.5, height * 0.4, width * 0.2, 0, 0, 0);
        generate_circle(data, width, height, width * 0.5, height * 0.45, width * 0.2, 255, 255, 0);

        // Eyes
        generate_circle(data, width, height, width * 0.35, height * 0.6, width * 0.07, 255, 0, 255);
        generate_circle(data, width, height, width * 0.65, height * 0.6, width * 0.07, 0, 0, 255);
    }

    void quad_texture(unsigned char* data, unsigned int width, unsigned int height) {
        for (unsigned int x = 0; x < width; x++) {
            for (unsigned int y = 0; y < height; y++) {
                if ((x < width * 0.5 && y < height * 0.5) || (x > width * 0.5 && y > height * 0.5)) {
                    data[(x + y * width) * 3 + 0] = 255;
                    data[(x + y * width) * 3 + 1] = 255;
                    data[(x + y * width) * 3 + 2] = 255;
                } else {
                    data[(x + y * width) * 3 + 0] = 0;
                    data[(x + y * width) * 3 + 1] = 0;
                    data[(x + y * width) * 3 + 2] = 0;
                }
            }
        }
    }
public:
    virtual void init() override {
        p_window = std::make_unique<Novo::Window>("Novo", glm::vec2(1920, 1080));
        p_window->setMaximized(true);
        
        const unsigned int width = 1000;
        const unsigned int height = 1000;
        const unsigned int channels = 3;
        auto* data = new unsigned char[width * height * channels];
        
        smile_texture(data, width, height);
        p_texture_smile = std::make_unique<Novo::Texture2D>(data, glm::vec2(width, height), channels);
        
        quad_texture(data, width, height);
        p_texture_quads = std::make_unique<Novo::Texture2D>(data, glm::vec2(width, height), channels);

        delete[] data;

        p_camera = std::make_unique<Novo::Camera>(c_pos, c_rot, Novo::Camera::CameraType::Perspective, c_fov, p_window->getAspectRatio());
        p_shader = std::make_unique<Novo::Shader>();
        p_shader->addShader(vertex_shader, GL_VERTEX_SHADER);
        p_shader->addShader(fragment_shader, GL_FRAGMENT_SHADER);
        p_shader->link();

        p_vbo = std::make_unique<Novo::VBO>(pos_col, sizeof(pos_col), Novo::Layout::l_texture);
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

            if (b_texture) {
                p_texture_quads->bind();
            } else {
                p_texture_smile->bind();
            }

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
        glm::vec2 mouseDelta = p_window->getMouseDelta();

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
        
        if (p_window->isMousePressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            p_window->hideCursor();

            if (p_window->isMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
                movement_delta.y -= mouseDelta.x * (c_speed / 2) * deltaTime;
                movement_delta.z -= mouseDelta.y * (c_speed / 2) * deltaTime;
            } else {
                rotation_delta.y -= mouseDelta.y * c_sensitivity * deltaTime;
                rotation_delta.z += mouseDelta.x * c_sensitivity * deltaTime;
            }
        } else {
            p_window->showCursor();
        }

        if (p_window->isKeyPressed(GLFW_KEY_TAB)) {
            b_debug = !b_debug;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (p_window->isKeyPressed(GLFW_KEY_G)) {
            b_texture = !b_texture;
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
                p_camera->set_position(glm::vec3(-1.f, 0.f, 0.f));
            }

            if (ImGui::Button("Reset Rotation")) {
                p_camera->set_rotation(glm::vec3(0.f, 0.f, 0.f));
            }

            ImGui::End();

            p_debugger->frame_end();
        }
    }
};
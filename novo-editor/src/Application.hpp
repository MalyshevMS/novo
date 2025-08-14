#include <novo-core/Application.hpp>
#include <novo-precompiles/Layouts.h>

#include "Debugger.hpp"

#include <memory>
#include <chrono>
#include <thread>

#define VERTIECES_NORMAL_UV { \
/*  X   Y   Z            NORMAL                  U   V*/    \
    /* Front */                                             \
    -1, 1,  -1,      0.0, 0.0,-1.0,         1.f,   1.f,     \
    -1, -1, -1,      0.0, 0.0,-1.0,         1.f,   0.f,     \
    1, -1,  -1,      0.0, 0.0,-1.0,         0.f,   0.f,     \
    1,  1,  -1,      0.0, 0.0,-1.0,         0.f,   1.f,     \
\
    /* Back */                       \
    -1, 1,  1,       0.0, 0.0, 1.0,         -1.f,  1.f,     \
    1,  1,  1,       0.0, 0.0, 1.0,         0.f,   1.f,     \
    1, -1,  1,       0.0, 0.0, 1.0,         0.f,   0.f,     \
    -1, -1, 1,       0.0, 0.0, 1.0,         -1.f,  0.f,     \
\
    /* Left */                       \
    -1,  1, -1,     -1.0, 0.0, 0.0,         -1.f,  1.f,     \
    -1,  1,  1,     -1.0, 0.0, 0.0,         0.f,   1.f,     \
    -1, -1,  1,     -1.0, 0.0, 0.0,         0.f,   0.f,     \
    -1, -1, -1,     -1.0, 0.0, 0.0,         -1.f,  0.f,     \
\
    /* Right */                      \
    1, -1, -1,      1.0, 0.0, 0.0,          1.f,   0.f,     \
    1, -1, 1,       1.0, 0.0, 0.0,          0.f,   0.f,     \
    1, 1,  1,       1.0, 0.0, 0.0,          0.f,   1.f,     \
    1, 1, -1,       1.0, 0.0, 0.0,          1.f,   1.f,     \
\
    /* Up */                         \
    -1, 1, -1,      0.0, 1.0, 0.0,          1.f,   0.f,     \
    1, 1, -1,       0.0, 1.0, 0.0,          0.f,   0.f,     \
    1,  1, 1,       0.0, 1.0, 0.0,          0.f,   1.f,     \
    -1, 1, 1,       0.0, 1.0, 0.0,          1.f,   1.f,     \
\
    /* Down */                       \
    -1, -1, 1,      0.0,-1.0, 0.0,          -1.f,  1.f,     \
    1, -1, 1,       0.0,-1.0, 0.0,          0.f,   1.f,     \
    1, -1, -1,      0.0,-1.0, 0.0,          0.f,   0.f,     \
    -1, -1, -1,     0.0,-1.0, 0.0,          -1.f,  0.f,     \
}

class NovoEditor : public Novo::Application {
private:
    std::unique_ptr<Novo::Window> p_window = nullptr;
    std::unique_ptr<Novo::Resources> p_resources = nullptr;
    std::unique_ptr<Debugger> p_debugger = nullptr;

    std::shared_ptr<Novo::Camera> p_camera = nullptr;
    std::shared_ptr<Novo::Shader> p_shader = nullptr;
    std::shared_ptr<Novo::Shader> p_light_shader = nullptr;

    std::unique_ptr<Novo::Scene> p_scene = nullptr;

    std::unique_ptr<Novo::VAO> p_vao = nullptr;
    std::unique_ptr<Novo::VBO> p_vbo = nullptr;
    std::unique_ptr<Novo::IBO> p_ibo = nullptr;

    glm::mat4 l_model = glm::mat4(1.f);
    glm::vec4 g_bgColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.f);

    glm::vec3 l_pos = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 l_col = glm::vec3(1.f, 1.f, 1.f);
    float l_ambient_factor = 0.1f;
    float l_diffuse_factor = 1.f;
    float l_specular_factor = 1.f;
    float l_shininess = 32.f;

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

        p_light_shader = std::make_shared<Novo::Shader>();
        p_light_shader->addShader(p_resources->getFileStr("res/shaders/light_source.vert"), GL_VERTEX_SHADER);
        p_light_shader->addShader(p_resources->getFileStr("res/shaders/light_source.frag"), GL_FRAGMENT_SHADER);
        p_light_shader->link();

        p_debugger = std::make_unique<Debugger>(*p_window);

        Novo::CurrentCamera::set_camera(p_camera);

        auto test_texture = p_resources->loadTexture("Texture", "res/textures/texture.png");
        auto box_texture = p_resources->loadTexture("Box", "res/textures/box_texture.png");
        auto grass_texture = p_resources->loadTexture("Grass", "res/textures/grass.png");

        test_texture->setMagFilter(GL_NEAREST);
        
        p_scene = std::make_unique<Novo::Scene>();

        p_scene->add_object(Novo::Mesh::Box(box_texture, p_shader, glm::vec3( 4.f,  0.f,  0.f)), "Box 1");
        p_scene->add_object(Novo::Mesh::Box(box_texture, p_shader, glm::vec3(-3.f, -1.f,  1.f)), "Box 2");
        p_scene->add_object(Novo::Mesh::Box(box_texture, p_shader, glm::vec3(-1.f,  5.f, -3.f)), "Box 3");
        p_scene->add_object(Novo::Mesh::Box(box_texture, p_shader, glm::vec3( 2.f, -4.f, -1.f)), "Box 4");
        p_scene->add_object(Novo::Mesh::Box(box_texture, p_shader, glm::vec3( 3.f, -4.f, -5.f)), "Box 5");
        
        p_scene->reload_all();

        // Light source
        GLfloat vertices_uv[] = VERTIECES_NORMAL_UV;

        GLuint indices[] = {
            0,  1,  2,  2,  3,  0,  // Front
            4,  5,  6,  4,  6,  7,  // Back
            8,  9,  10, 8,  10, 11, // Left
            12, 13, 14, 12, 14, 15, // Right
            16, 17, 18, 16, 18, 19, // Up
            20, 21, 22, 20, 22, 23, // Down
        };

        p_vbo = std::make_unique<Novo::VBO>(vertices_uv, sizeof(vertices_uv), Novo::Layout::l_texture);
        p_ibo = std::make_unique<Novo::IBO>(indices, sizeof(indices) / sizeof(GLuint));
        p_vao = std::make_unique<Novo::VAO>();

        p_vao->addVBO(*p_vbo);
        p_vao->setIBO(*p_ibo);
    }

    virtual void on_update() override {
        while (p_window->isOpen()) {
            p_window->update();
            glEnable(GL_DEPTH_TEST);
            glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            p_shader->load();
            p_shader->setUniform("light_color", l_col);
            p_shader->setUniform("light_position", l_pos);
            p_shader->setUniform("camera_position", Novo::CurrentCamera::get_position());

            p_shader->setUniform("ambient_factor", l_ambient_factor);
            p_shader->setUniform("diffuse_factor", l_diffuse_factor);
            p_shader->setUniform("specular_factor", l_specular_factor);
            p_shader->setUniform("shininess", l_shininess);
            p_scene->render();
            p_shader->unload();

            glDisable(GL_CULL_FACE);
            p_light_shader->load();
            l_model = glm::translate(glm::mat4(1.f), l_pos);
            p_light_shader->setUniform("light_color", l_col);
            p_light_shader->setUniform("model", l_model);
            p_light_shader->setUniform("view_projection", Novo::CurrentCamera::get_view_proj_matrix());
            p_vao->draw();
            p_light_shader->unload();

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

            ImGui::Begin("Light");
            ImGui::SetWindowFontScale(1.5f);
            if (ImGui::DragFloat3("Position", glm::value_ptr(l_pos), 0.1f)) {}
            if (ImGui::ColorEdit3("Color", glm::value_ptr(l_col))) {}
            if (ImGui::DragFloat("Ambient", &l_ambient_factor, 0.01f, 0.f, 1.f)) {};
            if (ImGui::DragFloat("Diffuse", &l_diffuse_factor, 0.01f, 0.f, 3.f)) {}
            if (ImGui::DragFloat("Specular", &l_specular_factor, 0.01f, 0.f, 10.f)) {}
            if (ImGui::DragFloat("Shininess", &l_shininess, 0.01f, 0.f, 100.f)) {}
            ImGui::End();

            p_scene->draw_ui();

            p_debugger->frame_end();
        }
    }
};
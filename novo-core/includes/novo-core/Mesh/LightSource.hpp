#pragma once

#include <novo-core/Mesh/MeshBase.hpp>

#define VERTIECES_NORMAL_UV { \
/*  X   Y   Z            NORMAL                  U   V*/    \
    /* Front */                                             \
    -1, 1,  -1,      0.0, 0.0,-1.0,         _uv.x,   _uv.y, \
    -1, -1, -1,      0.0, 0.0,-1.0,         _uv.x,   0.f,   \
    1, -1,  -1,      0.0, 0.0,-1.0,         0.f,     0.f,   \
    1,  1,  -1,      0.0, 0.0,-1.0,         0.f,     _uv.y, \
\
    /* Back */                       \
    -1, 1,  1,       0.0, 0.0, 1.0,         -_uv.x,  _uv.y, \
    1,  1,  1,       0.0, 0.0, 1.0,         0.f,     _uv.y, \
    1, -1,  1,       0.0, 0.0, 1.0,         0.f,     0.f,   \
    -1, -1, 1,       0.0, 0.0, 1.0,         -_uv.x,  0.f,   \
\
    /* Left */                       \
    -1,  1, -1,     -1.0, 0.0, 0.0,         -_uv.x,  _uv.y, \
    -1,  1,  1,     -1.0, 0.0, 0.0,         0.f,     _uv.y, \
    -1, -1,  1,     -1.0, 0.0, 0.0,         0.f,     0.f,   \
    -1, -1, -1,     -1.0, 0.0, 0.0,         -_uv.x,  0.f,   \
\
    /* Right */                      \
    1, -1, -1,      1.0, 0.0, 0.0,          _uv.x,   0.f,   \
    1, -1, 1,       1.0, 0.0, 0.0,          0.f,     0.f,   \
    1, 1,  1,       1.0, 0.0, 0.0,          0.f,     _uv.y, \
    1, 1, -1,       1.0, 0.0, 0.0,          _uv.x,   _uv.y, \
\
    /* Up */                         \
    -1, 1, -1,      0.0, 1.0, 0.0,          _uv.x,   0.f,   \
    1, 1, -1,       0.0, 1.0, 0.0,          0.f,     0.f,   \
    1,  1, 1,       0.0, 1.0, 0.0,          0.f,     _uv.y, \
    -1, 1, 1,       0.0, 1.0, 0.0,          _uv.x,   _uv.y, \
\
    /* Down */                       \
    -1, -1, 1,      0.0,-1.0, 0.0,          -_uv.x,  _uv.y, \
    1, -1, 1,       0.0,-1.0, 0.0,          0.f,     _uv.y, \
    1, -1, -1,      0.0,-1.0, 0.0,          0.f,     0.f,   \
    -1, -1, -1,     0.0,-1.0, 0.0,          -_uv.x,  0.f,   \
}

namespace Novo {
    namespace Mesh {
        class LightSource : public MeshBase {
        private:
            glm::vec3 _light_color;
        public:
            LightSource(const glm::vec3& light_color, std::shared_ptr<Novo::Shader> light_shader, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
               : MeshBase(std::make_shared<Texture2D>(Texture2D(nullptr, glm::vec2(0), 3)), std::move(light_shader), std::make_shared<Material>(), position, size, rotation), _light_color(light_color) {
                GLfloat vertices_uv[] = VERTIECES_NORMAL_UV;

                GLuint indices[] = {
                    0,  1,  2,  2,  3,  0,  // Front
                    4,  5,  6,  4,  6,  7,  // Back
                    8,  9,  10, 8,  10, 11, // Left
                    12, 13, 14, 12, 14, 15, // Right
                    16, 17, 18, 16, 18, 19, // Up
                    20, 21, 22, 20, 22, 23, // Down
                };

                _vbo = new Novo::VBO(vertices_uv, sizeof(vertices_uv), Novo::Layout::l_texture);
                _ibo = new Novo::IBO(indices, sizeof(indices) / sizeof(GLuint));
                _vao = new Novo::VAO();
                
                _vao->addVBO(*_vbo);
                _vao->setIBO(*_ibo);
            }

            virtual void draw() override {
                if (!_draw) return;
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                glFrontFace(GL_CCW);

                _shader->load();
                _texture->bind(0);
                glm::mat4 model = glm::mat4(1.f);
                glm::mat4 translate = glm::translate(model, _position);
                glm::mat4 rotate_x = glm::rotate(glm::mat4(1.f), glm::radians(_rotation.x), glm::vec3(1.f, 0.f, 0.f));
                glm::mat4 rotate_y = glm::rotate(glm::mat4(1.f), glm::radians(_rotation.y), glm::vec3(0.f, 1.f, 0.f));
                glm::mat4 rotate_z = glm::rotate(glm::mat4(1.f), glm::radians(_rotation.z), glm::vec3(0.f, 0.f, 1.f));
                glm::mat4 scale = glm::scale(model, _size);

                model = translate * rotate_x * rotate_y * rotate_z * scale;

                _shader->setUniform("model", model);
                _shader->setUniform("view_projection", CurrentCamera::get_view_proj_matrix());
                _shader->setUniform("light_color", _light_color);

                _vao->draw();
                _shader->unload();
            }

            void set_light_color(glm::vec3 light_color) {
                _light_color = light_color;
            }

            glm::vec3 get_light_color() { return _light_color; }

            virtual void draw_ui(const std::string& tab_name) override {
                if (!ImGui::TreeNode(tab_name.c_str())) return;
                if (ImGui::DragFloat3("Position", glm::value_ptr(_position), 0.1f)) {
                    set_position(_position);
                }
                if (ImGui::DragFloat3("Rotation", glm::value_ptr(_rotation), 0.1f)) {
                    set_rotation(_rotation);
                }
                if (ImGui::DragFloat3("Size", glm::value_ptr(_size), 0.1f)) {
                    set_size(_size);
                }
                if (ImGui::ColorEdit3("Light color", glm::value_ptr(_light_color))) {
                    set_light_color(_light_color);
                }
                ImGui::Checkbox("Show", &_draw);
                ImGui::TreePop();
            }

            virtual void set_uv(glm::vec2 uv) override {
                _uv = uv;
                GLfloat new_uv[] = VERTIECES_NORMAL_UV;

                delete _vbo;
                delete _vao;

                _vbo = new Novo::VBO(new_uv, sizeof(new_uv), Novo::Layout::l_texture);
                _vao = new Novo::VAO();
                
                _vao->addVBO(*_vbo);
                _vao->setIBO(*_ibo);
            }
        };
    }
}

#undef VERTIECES_NORMAL_UV
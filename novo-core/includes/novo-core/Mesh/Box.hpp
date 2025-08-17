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
        class Box : public MeshBase {
        private:
            bool _inverse = false;
        public:
            Box(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, std::shared_ptr<Material> material, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
               : MeshBase(std::move(texture), std::move(shader), std::move(material), position, size, rotation) {
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
                glEnable(GL_CULL_FACE);
                if (!_inverse) glCullFace(GL_FRONT);
                else glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
                MeshBase::draw();
            }

            void inverse() {
                _inverse = !_inverse;
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
                if (ImGui::DragFloat2("UV", glm::value_ptr(_uv), 0.1f)) {
                    set_uv(_uv);
                }
                if (ImGui::TreeNode("Material")) {
                    if (ImGui::DragFloat("Ambient factor", &_material->ambient_factor, 0.01f)) {
                        set_material(*_material);
                    }
                    if (ImGui::DragFloat("Diffuse factor", &_material->diffuse_factor, 0.01f)) {
                        set_material(*_material);
                    }
                    if (ImGui::DragFloat("Specular factor", &_material->specular_factor, 0.01f)) {
                        set_material(*_material);
                    }
                    if (ImGui::DragFloat("Shininess", &_material->shininess, 0.01f)) {
                        set_material(*_material);
                    }
                    ImGui::TreePop();
                }
                if (ImGui::Button("Inverse")) {
                    inverse();
                }
                ImGui::Checkbox("Show", &_draw);
                ImGui::TreePop();
            }
        };
    }
}

#undef VERTIECES_NORMAL_UV
#pragma once

#include <novo-core/Mesh/MeshBase.hpp>

namespace Novo {
    namespace Mesh {
        class Plane : public MeshBase {
        public:
            Plane(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
            : MeshBase(std::move(texture), std::move(shader), position, size, rotation) {
                GLfloat vertices_uv[] = {
                //  X   Y   Z       //  U    V
                    -1, 0, -1,      _uv.x,   0.f,
                    1, 0, -1,       0.f,     0.f,
                    -1, 0, 1,       _uv.x,   _uv.y,
                    1,  0, 1,       0.f,     _uv.y,
                };

                GLuint indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                _vbo = std::make_unique<Novo::VBO>(vertices_uv, sizeof(vertices_uv), Novo::Layout::l_texture);
                _ibo = std::make_unique<Novo::IBO>(indices, sizeof(indices) / sizeof(GLuint));
                _vao = std::make_unique<Novo::VAO>();
                
                _vao->addVBO(*_vbo);
                _vao->setIBO(*_ibo);
            }

            virtual void set_uv(glm::vec2 uv) override {
                _uv = uv;
                GLfloat new_uv[20] = {
                    -1, 0, -1,      _uv.x,   0.f,
                    1, 0, -1,       0.f,     0.f,
                    -1, 0, 1,       _uv.x,   _uv.y,
                    1,  0, 1,       0.f,     _uv.y,
                };

                _vbo = std::make_unique<Novo::VBO>(new_uv, sizeof(new_uv), Novo::Layout::l_texture);
                _vao = std::make_unique<Novo::VAO>();
                
                _vao->addVBO(*_vbo);
                _vao->setIBO(*_ibo);
            }
        };
    }
}
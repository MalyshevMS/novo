#pragma once

#include <novo-core/Mesh/MeshBase.hpp>

namespace Novo {
    namespace Mesh {
        class Triangle : public MeshBase {
        private:
            glm::vec3 a, b, c;
        public:
            Triangle(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
            : MeshBase(std::move(texture), std::move(shader), position, size, rotation), a(a), b(b), c(c) {
                GLfloat vertices_uv[] = {
                    a.x, a.y, a.z,     _uv.x,   0.f,
                    b.x, b.y, b.z,      0.f,     0.f,
                    c.x, c.y, c.z,      _uv.x,   _uv.y,
                };
                GLuint indices[] = {
                    0, 1, 2
                };

                _vbo = new Novo::VBO(vertices_uv, sizeof(vertices_uv), Novo::Layout::l_texture);
                _ibo = new Novo::IBO(indices, sizeof(indices) / sizeof(GLuint));
                _vao = new Novo::VAO();
                
                _vao->addVBO(*_vbo);
                _vao->setIBO(*_ibo);
            }

            virtual void set_uv(glm::vec2 uv) override {
                _uv = uv;
                GLfloat new_uv[] = {
                    a.x, a.y, a.z,    _uv.x,   0.f,
                    b.x, b.y, b.z,     0.f,     0.f,
                    c.x, c.y, c.z,     _uv.x,   _uv.y
                };

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
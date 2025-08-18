#pragma once

#include <novo-core/Mesh/MeshBase.hpp>

#define VERTIECES_NORMAL_UV { \
/*  X   Y   Z     NORMAL              U    V*/      \
    -1, 0, -1, 0.0, 1.0, 0.0,      _uv.x,   0.f,   \
    1, 0, -1,  0.0, 1.0, 0.0,      0.f,     0.f,   \
    1,  0, 1,  0.0, 1.0, 0.0,      0.f,     _uv.y, \
    -1, 0, 1,  0.0, 1.0, 0.0,      _uv.x,   _uv.y, \
\
    -1, 0, 1,  0.0,-1.0, 0.0,      _uv.x,   _uv.y, \
    1,  0, 1,  0.0,-1.0, 0.0,      0.f,     _uv.y, \
    1, 0, -1,  0.0,-1.0, 0.0,      0.f,     0.f,   \
    -1, 0, -1, 0.0,-1.0, 0.0,      _uv.x,   0.f,   \
}

namespace Novo {
    namespace Mesh {
        class Plane : public MeshBase {
        public:
            Plane(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, std::shared_ptr<Material> material, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
            : MeshBase(std::move(texture), std::move(shader), std::move(material), position, size, rotation) {
                GLfloat vertices_uv[] = VERTIECES_NORMAL_UV;

                GLuint indices[] = {
                    0, 1, 2,
                    0, 2, 3,

                    4, 5, 6,
                    4, 6, 7,
                };

                _vbo = new Novo::VBO(vertices_uv, sizeof(vertices_uv), Novo::Layout::l_texture);
                _ibo = new Novo::IBO(indices, sizeof(indices) / sizeof(GLuint));
                _vao = new Novo::VAO();
                
                _vao->addVBO(*_vbo);
                _vao->setIBO(*_ibo);
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
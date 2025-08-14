#pragma once

#include <novo-core/Mesh/MeshBase.hpp>

#define VERTECES_UV { \
/*  X   Y   Z             U   V*/    \
    /* Front */                      \
    -1, 1,  -1,      _uv.x,   _uv.y, \
    -1, -1, -1,      _uv.x,   0.f,   \
    1, -1,  -1,      0.f,     0.f,   \
    1,  1,  -1,      0.f,     _uv.y, \
\
    /* Back */                       \
    -1, 1,  1,      -_uv.x,  _uv.y,  \
    1,  1,  1,      0.f,     _uv.y,  \
    1, -1,  1,      0.f,     0.f,    \
    -1, -1, 1,      -_uv.x,  0.f,    \
\
    /* Left */                       \
    -1, 1, -1,      -_uv.x,  _uv.y,  \
    -1, 1,  1,      0.f,     _uv.y,  \
    -1, -1, 1,      0.f,     0.f,    \
    -1, -1, -1,     -_uv.x,  0.f,    \
\
    /* Right */                      \
    1, -1, -1,      _uv.x,   0.f,    \
    1, -1, 1,      0.f,      0.f,    \
    1, 1,  1,      0.f,      _uv.y,  \
    1, 1, -1,      _uv.x,    _uv.y,  \
\
    /* Up */                         \
    -1, 1, -1,      _uv.x,   0.f,    \
    1, 1, -1,       0.f,     0.f,    \
    1,  1, 1,       0.f,     _uv.y,  \
    -1, 1, 1,       _uv.x,   _uv.y,  \
\
    /* Down */                       \
    -1, -1, 1,      -_uv.x,  _uv.y,  \
    1, -1, 1,       0.f,     _uv.y,  \
    1, -1, -1,      0.f,     0.f,    \
    -1, -1, -1,     -_uv.x,  0.f,    \
}

namespace Novo {
    namespace Mesh {
        class Box : public MeshBase {
        public:
            Box(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
               : MeshBase(std::move(texture), std::move(shader), position, size, rotation) {
                GLfloat vertices_uv[] = VERTECES_UV;

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
                glCullFace(GL_FRONT);
                glFrontFace(GL_CCW);
                MeshBase::draw();
            }

            virtual void set_uv(glm::vec2 uv) override {
                _uv = uv;
                GLfloat new_uv[] = VERTECES_UV;

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

#undef VERTECES_UV
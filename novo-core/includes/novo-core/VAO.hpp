#pragma once

#include <glad/glad.h>
#include <novo-core/VBO.hpp>

namespace Novo {
    class VAO {
    private:
        GLuint _id;
        GLuint _elCount = 0;
    public:
        VAO() {
            glGenVertexArrays(1, &_id);
        }

        ~VAO() {
            glDeleteVertexArrays(1, &_id);
        }

        void bind() {
            glBindVertexArray(_id);
        }

        static void unbind() {
            glBindVertexArray(0);
        }

        void addVBO(VBO& vbo) {
            bind();
            vbo.bind();

            for (auto& element : vbo.get_layout().get_elements()) {
                glVertexAttribPointer(
                    _elCount,
                    element.components_count,
                    element.component_type,
                    GL_FALSE,
                    vbo.get_layout().get_stride(),
                    reinterpret_cast<void*>(element.offset)
                );
                glEnableVertexAttribArray(_elCount);
                ++_elCount;
            }
        }
    };
}
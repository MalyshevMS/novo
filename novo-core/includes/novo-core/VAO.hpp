#pragma once

#include <glad/glad.h>
#include <novo-core/VBO.hpp>
#include <novo-core/IBO.hpp>

namespace Novo {
    class VAO {
    private:
        GLuint _id;
        GLuint _elCount = 0;
        GLuint _indCount = 0;
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

        void setIBO(IBO& ibo) {
            bind();
            ibo.bind();
            _indCount = ibo.get_count();

            
        }
        
        size_t getIndCount() const {
            return _indCount;
        }

        void draw(GLenum method = GL_TRIANGLES) {
            bind();
            if (_indCount > 0) {
                glDrawElements(method, _indCount, GL_UNSIGNED_INT, nullptr);
            } else {
                glDrawArrays(method, 0, _elCount);
            }
        }
    };
}
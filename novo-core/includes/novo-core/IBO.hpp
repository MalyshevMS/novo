#pragma once

#include <novo-core/VBO.hpp>

namespace Novo {
    class IBO {
    private:
        GLuint _id;
        size_t _count;
    public:
        IBO(const void* data, const size_t count, const VBO::Mode mode = VBO::Mode::STATIC)
            : _count(count) {
            glGenBuffers(1, &_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, VBO::modeToGL(mode));
        }

        ~IBO() {
            glDeleteBuffers(1, &_id);
        }

        void bind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
        }

        static void unbind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        size_t get_count() const {
            return _count;
        }
    };
};
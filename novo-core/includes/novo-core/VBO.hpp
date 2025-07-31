#include <glad/glad.h>
#include <iostream>

namespace Novo {
    class VBO {
    private:
        GLuint _id;
    public:
        enum class Mode {
            STATIC,
            DYNAMIC,
            STREAM
        };

        private: static GLenum modeToGL(Mode mode) {
            switch (mode) {
                case Mode::STATIC:
                    return GL_STATIC_DRAW;
                case Mode::DYNAMIC:
                    return GL_DYNAMIC_DRAW;
                case Mode::STREAM:
                    return GL_STREAM_DRAW;
                default:
                    return GL_STATIC_DRAW;
            }
        }

        public: VBO(void* data, const size_t size, Mode mode = Mode::STATIC) {
            glGenBuffers(1, &_id);
            glBindBuffer(GL_ARRAY_BUFFER, _id);
            glBufferData(GL_ARRAY_BUFFER, size, data, modeToGL(mode));
        }

        ~VBO() {
            glDeleteBuffers(1, &_id);
        }

        void bind() {
            glBindBuffer(GL_ARRAY_BUFFER, _id);
        }

        static void unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    };
}
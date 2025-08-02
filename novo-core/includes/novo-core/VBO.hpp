#pragma once

#include <glad/glad.h>
#include <iostream>

namespace Novo {
    enum class ShaderDataType {
        Float,      Int,
        Float2,     Int2,
        Float3,     Int3,
        Float4,     Int4,
    };

    constexpr size_t get_count(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Int:
            case ShaderDataType::Float:
                return 1;
            case ShaderDataType::Int2:
            case ShaderDataType::Float2:
                return 2;
            case ShaderDataType::Int3:
            case ShaderDataType::Float3:
                return 3;
            case ShaderDataType::Int4:
            case ShaderDataType::Float4:
                return 4;
        }
        return 0;
    }

    constexpr size_t get_size(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                return sizeof(GLint) * get_count(type);
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
                return sizeof(GLfloat) * get_count(type);
        }
        return 0;
    }

    constexpr GLenum get_component_type(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                return GL_INT;
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
                return GL_FLOAT;
        }
        return 0;
    }

    struct BufferElement {
        ShaderDataType type;
        GLenum component_type;
        size_t components_count;
        size_t size;
        size_t offset;

        BufferElement(ShaderDataType type)
         : type(type),
          component_type(get_component_type(type)),
          components_count(get_count(type)),
          size(get_size(type)),
          offset(0) {}
    };

    class BufferLayout {
    private:
        std::vector<BufferElement> _elements;
        size_t _stride;
    public:
        BufferLayout(std::initializer_list<BufferElement> elements)
         : _elements(std::move(elements)) {
            size_t offset = 0;
            _stride = 0;
            for (auto& element : _elements) {
                element.offset = offset;
                offset += element.size;
                _stride += element.size;
            }
        }

        const std::vector<BufferElement>& get_elements() const {
            return _elements;
        }

        size_t get_stride() const {
            return _stride;
        }
    };

    class VBO {
    private:
        GLuint _id;
        BufferLayout _layout;
    public:
        enum class Mode {
            STATIC,
            DYNAMIC,
            STREAM
        };

        static GLenum modeToGL(Mode mode) {
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

        public: VBO(void* data, const size_t size, BufferLayout layout, Mode mode = Mode::STATIC) : _layout(layout) {
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

        BufferLayout get_layout() const {
            return _layout;
        }
    };
}
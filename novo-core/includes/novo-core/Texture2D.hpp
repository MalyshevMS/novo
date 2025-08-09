#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Novo {
    class Texture2D {
    private:
        GLuint _id;
    public:
        Texture2D(const unsigned char* texture, const glm::vec2& size, const unsigned int channels, const GLenum wrap = GL_REPEAT, const GLenum filter = GL_LINEAR) {
            GLenum internalFormat;
            GLenum format;
            switch (channels) {
                case 3:
                    internalFormat = GL_RGB8;
                    format = GL_RGB;
                    break;
                case 4:
                    internalFormat = GL_RGBA8;
                    format = GL_RGBA;
                    break;
                default:
                    internalFormat = GL_RGB8;
                    format = GL_RGB;
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &_id);
            glTextureStorage2D(_id, 1, internalFormat, size.x, size.y);
            glTextureSubImage2D(_id, 0, 0, 0, size.x, size.y, format, GL_UNSIGNED_BYTE, texture);

            glTextureParameteri(_id, GL_TEXTURE_WRAP_S, wrap);
            glTextureParameteri(_id, GL_TEXTURE_WRAP_T, wrap);
            glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, filter);
            glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, filter);
        }

        ~Texture2D() {
            glDeleteTextures(1, &_id);
        }

        void bind(int unit = 0) const {
            glBindTextureUnit(unit, _id);
        }

        static void unbind(int unit = 0) {
            glBindTextureUnit(unit, 0);
        }
    };
}
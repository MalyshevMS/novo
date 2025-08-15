#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Novo {
    class Shader {
    private:
        static bool compileShader(const std::string& source, const GLenum shaderType, GLuint& shaderID) {
            shaderID = glCreateShader(shaderType);
            const char* code = source.c_str();
            glShaderSource(shaderID, 1, &code, nullptr);
            glCompileShader(shaderID);

            GLint success;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

            if (!success) {
                GLchar infolog[1024];

                glGetShaderInfoLog(shaderID, 1024, nullptr, infolog);

                std::cerr << "Shader error:\n" << infolog << std::endl;
                return false;
            }
            return true;
        };

        bool _isLinked = false;
        bool _attachedFS = false;
        bool _attachedVS = false;
        GLuint _shaderID = 0;

    public:
        Shader() {
            init();
        }

        Shader(const std::string& vertexSource, const std::string& fragmentSource) {
            GLuint vertexShaderID = addShader(vertexSource, GL_VERTEX_SHADER);
            GLuint fragmentShaderID = addShader(fragmentSource, GL_FRAGMENT_SHADER);
            init();
            link();
            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);
        }

        void init() {
            _shaderID = glCreateProgram();
        }

        GLuint addShader(const std::string& source, const GLenum shaderType) {
            GLuint ID;
            if (!compileShader(source, shaderType, ID)) {
                glDeleteShader(ID);
                return 0;
            }

            glAttachShader(_shaderID, ID);

            if (shaderType == GL_VERTEX_SHADER) {
                _attachedVS = true;
            } else if (shaderType == GL_FRAGMENT_SHADER) {
                _attachedFS = true;
            }

            return ID;
        }

        void link() {
            if (!_attachedFS || !_attachedVS) {
                std::cerr << "Shader link error: not all shaders are attached" << std::endl;
                return;
            }

            glLinkProgram(_shaderID);
            GLint success;
            glGetProgramiv(_shaderID, GL_LINK_STATUS, &success);

            if (!success) {
                GLchar infolog[1024];

                glGetShaderInfoLog(_shaderID, 1024, nullptr, infolog);

                std::cerr << "Shader link error:\n" << infolog << std::endl;
            } else {
                _isLinked = true;
            }
        }

        bool isLinked() const {
            return _isLinked;
        }

        void load() const {
            glUseProgram(_shaderID);
        }

        static void unload() {
            glUseProgram(0);
        }

        void setUniform(const std::string& name, const GLint value) {
            glUniform1i(glGetUniformLocation(_shaderID, name.c_str()), value);
        }

        void setUniform(const std::string& name, const GLfloat value) {
            glUniform1f(glGetUniformLocation(_shaderID, name.c_str()), value);
        }

        void setUniform(const std::string& name, const glm::vec2& vector) {
            glUniform2fv(glGetUniformLocation(_shaderID, name.c_str()), 1, glm::value_ptr(vector));
        }

        void setUniform(const std::string& name, const glm::vec3& vector) {
            glUniform3fv(glGetUniformLocation(_shaderID, name.c_str()), 1, glm::value_ptr(vector));
        }

        void setUniform(const std::string& name, const glm::mat4& matrix) {
            glUniformMatrix4fv(glGetUniformLocation(_shaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
        }
    };
}
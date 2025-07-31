#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

namespace Novo {
    class Window {
    private:
        GLFWwindow* _win = nullptr;
        glm::vec2 _size;
        std::string _title;
    public:
        Window(const std::string& title, glm::vec2 size) {
            if (!glfwInit()) { // Check for GLFW
                std::cerr << "glfwInit failed!" << std::endl;
                return;
            }

            // GLFW window settings
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            glfwWindowHint(GLFW_MAXIMIZED, 0);

            _win = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
            _size = size;
            _title = title;

            if (!_win) { // Check for GLFW window
                std::cerr << "Window creation failed!" << std::endl;
                glfwTerminate();
                return;
            }

            glfwMakeContextCurrent(_win);

            if (!gladLoadGL()) { // Check for GLAD
                std::cerr << "Can't load GLAD!" << std::endl;
                return;
            }
        }

        bool shouldClose() {
            if (_win) {
                return glfwWindowShouldClose(_win);
            } else {
                return true;
            }
        }

        void update() {
            if (_win) {
                glfwSwapBuffers(_win);
                glfwPollEvents();
            }
        }

        void close() {
            if (_win) {
                glfwDestroyWindow(_win);
                glfwTerminate();
            }
        }
    };
}
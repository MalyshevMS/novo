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
        bool _fullscreen = false;

        double _dt = 0.0f;
        double _lastTime = 0.0f;
        double _currentTime = 0.0f;

        std::function<void(Window&, glm::vec2)> _size_callback = [](Window&, glm::vec2){};
        std::function<void(Window&, int)>       _key_callback  = [](Window&, int){};

        static void s_size_callback(GLFWwindow* win, int width, int height) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));
            window->_size_callback(*window, glm::vec2(width, height));
        }

        static void s_key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));
                window->_key_callback(*window, key);
            }
        }
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
            glfwSetWindowUserPointer(_win, this);

            _size_callback = [](Window& win, glm::vec2 size) {
                win.setSize(size);
            }; // Default size callback

            glfwSetWindowSizeCallback(_win, s_size_callback);
            glfwSetKeyCallback(_win, s_key_callback);

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

                _currentTime = glfwGetTime();
                _dt = _currentTime - _lastTime;
                _lastTime = _currentTime;
            }
        }

        void close() {
            if (_win) {
                glfwDestroyWindow(_win);
                glfwTerminate();
            }
        }

        bool isKeyPressed(int key) {
            if (_win) {
                return glfwGetKey(_win, key) == GLFW_PRESS;
            } else {
                return false;
            }
        }

        glm::vec2 getSize() const {
            return _size;
        }

        void setSize(glm::vec2 size) {
            _size = size;
            glfwSetWindowSize(_win, _size.x, _size.y);
            glViewport(0, 0, _size.x, _size.y);
        }

        void setTitle(const std::string& title) {
            _title = title;
            glfwSetWindowTitle(_win, _title.c_str());
        }

        void setFullscreen(bool fullscreen) {
            if (fullscreen) {
                auto vm = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(_win, glfwGetPrimaryMonitor(), 0, 0, vm->width, vm->height, vm->refreshRate);
                this->setSize({vm->width, vm->height});
            } else {
                glfwSetWindowMonitor(_win, nullptr, 0, 0, _size.x, _size.y, 0);
            }
            _fullscreen = fullscreen;
        }

        bool isFullscreen() const {
            return _fullscreen;
        }

        GLFWwindow* getHandle() {
            return _win;
        }

        float getAspectRatio() const {
            return _size.x / _size.y;
        }

        float getDeltaTime() const {
            return _dt;
        }
    };
}
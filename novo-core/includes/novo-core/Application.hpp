#pragma once

#include <novo-core/Window.hpp>
#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <novo-core/IBO.hpp>
#include <novo-core/Texture2D.hpp>
#include <novo-core/Camera.hpp>
#include <novo-core/Resources.hpp>

#include <novo-core/Mesh/Box.hpp>
#include <novo-core/Mesh/Plane.hpp>
#include <novo-core/Mesh/Triangle.hpp>

namespace Novo {
    class Application {
    private:
    public:
        virtual void init() {}
        virtual void on_update() {}
        virtual void key_pressed() {}
    };
};
#pragma once

#include <novo-core/Camera.hpp>
#include <memory>

namespace Novo {
    namespace CurrentCamera {
        static std::shared_ptr<Camera> p_current_camera;
        static void set_camera(std::shared_ptr<Camera> camera) {
            CurrentCamera::p_current_camera = camera;
        }

        static glm::mat4 get_view_proj_matrix() {
            return CurrentCamera::p_current_camera->get_view_proj_matrix();
        }

        static glm::vec3 get_position() {
            return CurrentCamera::p_current_camera->get_position();
        }
    };
}
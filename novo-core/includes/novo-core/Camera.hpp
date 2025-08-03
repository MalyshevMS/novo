#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Novo {
    class Camera {
    public: enum class CameraType;
    private:
        CameraType _type;
        glm::vec3 _position;
        glm::vec3 _rotation;
        glm::mat4 _view_matrix;
        glm::mat4 _proj_matrix;

        void update_view_matrix() {
            glm::mat4 translate_mat = glm::translate(glm::mat4(1.f), -_position);

            float rotate_rad_z = glm::radians(-_rotation.z);
            glm::mat4 rotate_mat_z = glm::rotate(glm::mat4(1.f), rotate_rad_z, glm::vec3(0.f, 0.f, 1.f));

            float rotate_rad_x = glm::radians(-_rotation.x);
            glm::mat4 rotate_mat_x = glm::rotate(glm::mat4(1.f), rotate_rad_x, glm::vec3(1.f, 0.f, 0.f));

            float rotate_rad_y = glm::radians(-_rotation.y);
            glm::mat4 rotate_mat_y = glm::rotate(glm::mat4(1.f), rotate_rad_y, glm::vec3(0.f, 1.f, 0.f));

            _view_matrix = rotate_mat_y * rotate_mat_x * translate_mat;
        }

        void update_proj_matrix(const CameraType proj_mode) {
            if (proj_mode == CameraType::Perspective) {
                float r = 0.1f;
                float t = 0.1f;
                float f = 10.f;
                float n = 0.1f;
                _proj_matrix = glm::mat4(
                    n / r, 0.f, 0.f, 0.f,
                    0.f, n / t, 0.f, 0.f,
                    0.f, 0.f, -(f + n) / (f - n), -1.f,
                    0.f, 0.f, -(2.f * f * n) / (f - n), 0.f
                );
            } else {
                float r = 2;
                float t = 2;
                float f = 100;
                float n = 0.1f;
                _proj_matrix = glm::mat4(
                    1 / r, 0.f, 0.f, 0.f,
                    0.f, 1 / t, 0.f, 0.f,
                    0.f, 0.f, -2 / (f - n), 0.f,
                    0.f, 0.f, -(f + n) / (f - n), 1.f
                );
            }
        }
    public:
        enum class CameraType {
            Perspective,
            Orthographic
        };

        Camera(const glm::vec3& position = {0, 0, 0},
               const glm::vec3& rotation = {0, 0, 0},
               const CameraType proj_mode = CameraType::Perspective)
               : _position(position),
                 _rotation(rotation),
                 _type(proj_mode) {
            update_view_matrix();
            update_proj_matrix(proj_mode);
        }

        void set_position(const glm::vec3& position) {
            _position = position;
            update_view_matrix();
        }

        void set_rotation(const glm::vec3& rotation) {
            _rotation = rotation;
            update_view_matrix();
        }

        void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation) {
            _position = position;
            _rotation = rotation;
            update_view_matrix();
        }

        void set_projection_mode(const CameraType proj_mode) {
            _type = proj_mode;
            update_proj_matrix(_type);
        }

        glm::mat4 get_view_matrix() const {
            return _view_matrix;
        }

        glm::mat4 get_proj_matrix() const {
            return _proj_matrix;
        }

        glm::mat4 get_view_proj_matrix() const {
            return _proj_matrix * _view_matrix;
        }
    };
}
#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Novo {
    class Camera {
    public: enum class CameraType;
    private:
        CameraType _type;
        glm::vec3 _position;
        glm::vec3 _rotation; // X = pitch, Y = yaw, Z = roll

        glm::vec3 _direction;
        glm::vec3 _right;
        glm::vec3 _up;

        static constexpr glm::vec3 s_up =      {0, 1, 0};
        static constexpr glm::vec3 s_right =   {1, 0, 0};
        static constexpr glm::vec3 s_forward = {0, 0, 1};

        glm::mat4 _view_matrix;
        glm::mat4 _proj_matrix;
        float _fov;
        float _aspect_ratio;
        float _near = 0.001f;
        float _far = 100.f;

        void update_view_matrix() {
            float pitch_rad = glm::radians(_rotation.x);
            float yaw_rad = glm::radians(_rotation.y);    
            float roll_rad = glm::radians(_rotation.z);

            glm::mat4 rotation = glm::eulerAngleYXZ(yaw_rad, pitch_rad, roll_rad);

            _direction = glm::normalize(glm::vec3(rotation * glm::vec4(s_forward, 0)));
            _right = glm::normalize(glm::vec3(rotation * glm::vec4(s_right, 0)));
            _up = glm::normalize(glm::vec3(rotation * glm::vec4(s_up, 0)));

            _view_matrix = glm::lookAt(_position, _position + _direction, _up);
        }

        void update_proj_matrix(const CameraType proj_mode) {
            if (proj_mode == CameraType::Perspective) {
                _proj_matrix = glm::perspective(glm::radians(_fov), _aspect_ratio, _near, _far);
            } else {
                _proj_matrix = glm::ortho(-_aspect_ratio * _near, _aspect_ratio * _near,
                                            -_near, _near, _near, _far);
            }
        }
    public:
        enum class CameraType {
            Perspective,
            Orthographic
        };

        Camera(const glm::vec3& position = {0, 0, 0},
               const glm::vec3& rotation = {0, 0, 0},
               const CameraType proj_mode = CameraType::Perspective,
               const float& fov = 45.f,
               const float& aspect_ratio = 16.f / 9.f)
               : _position(position),
                 _rotation(rotation),
                 _type(proj_mode),
                 _aspect_ratio(aspect_ratio),
                 _fov(fov) {
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

        void set_fov(const float fov) {
            _fov = fov;
            update_proj_matrix(_type);
        }

        void move_forward(const float delta) {
            _position += _direction * delta;
            update_view_matrix();
        }

        void move_right(const float delta) {
            _position += _right * delta;
            update_view_matrix();
        }

        void move_up(const float delta) {
            _position += _up * delta;
            update_view_matrix();
        }

        void move_rotate(const glm::vec3& delta_move, const glm::vec3 delta_rot) {
            _position += _right     * delta_move.x;
            _position += _up        * delta_move.y;
            _position += _direction * delta_move.z;
            _rotation += delta_rot;
            update_view_matrix();
        }

        const glm::vec3& get_position() const {
            return _position;
        }

        const glm::vec3& get_rotation() const {
            return _rotation;
        }
    };
}
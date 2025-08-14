#pragma once

#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <novo-core/IBO.hpp>
#include <novo-core/Texture2D.hpp>
#include <novo-core/Camera.hpp>

#include <novo-precompiles/Layouts.h>

#include <memory>
#include <imgui/imgui.h>


namespace Novo {
    namespace Mesh {
        class MeshBase {
        protected:
            std::unique_ptr<Novo::VAO> _vao = nullptr;
            std::unique_ptr<Novo::VBO> _vbo = nullptr;
            std::unique_ptr<Novo::IBO> _ibo = nullptr;

            std::shared_ptr<Novo::Shader> _shader = nullptr;
            std::shared_ptr<Novo::Texture2D> _texture = nullptr;

            glm::vec3 _position;
            glm::vec3 _size;
            glm::vec3 _rotation;

            glm::vec2 _uv = glm::vec2(1.f, 1.f);
        public:
            /// @warning Don't forget to initialize _vao, _vbo and _ibo
            MeshBase(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0)) {
                _texture = texture;
                _shader = shader;

                _position = position;
                _size = size;
                _rotation = rotation;
            };

            virtual void draw(std::shared_ptr<Novo::Camera> camera) {
                _shader->load();
                _texture->bind(0);
                glm::mat4 model = glm::mat4(1.f);
                glm::mat4 translate = glm::translate(model, _position);
                glm::mat4 rotate_x = glm::rotate(glm::mat4(1.f), glm::radians(_rotation.x), glm::vec3(1.f, 0.f, 0.f));
                glm::mat4 rotate_y = glm::rotate(glm::mat4(1.f), glm::radians(_rotation.y), glm::vec3(0.f, 1.f, 0.f));
                glm::mat4 rotate_z = glm::rotate(glm::mat4(1.f), glm::radians(_rotation.z), glm::vec3(0.f, 0.f, 1.f));
                glm::mat4 scale = glm::scale(model, _size);

                model = translate * rotate_x * rotate_y * rotate_z * scale;

                _shader->setUniform("model", model);
                _shader->setUniform("view_projection", camera->get_view_proj_matrix());

                _vao->draw();
                _shader->unload();
            }

            virtual void set_uv(glm::vec2 uv) {}

            virtual void set_position(glm::vec3 position) {
                _position = position;
            }

            virtual void set_size(glm::vec3 size) {
                _size = size;
            }

            virtual void set_rotation(glm::vec3 rotation) {
                _rotation = rotation;
            }

            virtual void draw_ui(const std::string& tab_name) {
                ImGui::Begin(tab_name.c_str());
                ImGui::SetWindowFontScale(1.5f);
                if (ImGui::DragFloat3("Position", glm::value_ptr(_position), 0.1f)) {
                    set_position(_position);
                }
                if (ImGui::DragFloat3("Rotation", glm::value_ptr(_rotation), 0.1f)) {
                    set_rotation(_rotation);
                }
                if (ImGui::DragFloat3("Size", glm::value_ptr(_size), 0.1f)) {
                    set_size(_size);
                }
                if (ImGui::DragFloat2("UV", glm::value_ptr(_uv), 0.1f)) {
                    set_uv(_uv);
                }
                ImGui::End();
            }

            virtual void reload() {
                set_uv(_uv);
            }
        };
    }
}
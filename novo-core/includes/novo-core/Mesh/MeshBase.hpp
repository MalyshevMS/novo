#pragma once

#include <novo-core/Shader.hpp>
#include <novo-core/VBO.hpp>
#include <novo-core/VAO.hpp>
#include <novo-core/IBO.hpp>
#include <novo-core/Texture2D.hpp>
#include <novo-core/CurrentCamera.hpp>
#include <novo-core/Material.hpp>
#include <novo-core/Mesh/MeshID.hpp>

#include <novo-precompiles/Layouts.h>

#include <memory>
#include <imgui/imgui.h>


namespace Novo {
    namespace Mesh {
        class MeshBase {
        protected:
            Novo::VAO* _vao = nullptr;
            Novo::VBO* _vbo = nullptr;
            Novo::IBO* _ibo = nullptr;

            std::shared_ptr<Novo::Shader> _shader = nullptr;
            std::shared_ptr<Novo::Texture2D> _texture = nullptr;
            std::shared_ptr<Material> _material = nullptr;

            glm::vec3 _position;
            glm::vec3 _size;
            glm::vec3 _rotation;

            glm::vec2 _uv = glm::vec2(1.f, 1.f);


            bool _draw = true;
        public:
            /// @warning Don't forget to initialize _vao, _vbo and _ibo
            MeshBase(std::shared_ptr<Novo::Texture2D> texture, std::shared_ptr<Novo::Shader> shader, std::shared_ptr<Material> material, glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0)) {
                _texture = texture;
                _shader = shader;

                _position = position;
                _size = size;
                _rotation = rotation;

                _material = material;
            };

            virtual void draw() {
                if (!_draw) return;
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
                _shader->setUniform("view_projection", CurrentCamera::get_view_proj_matrix());

                _shader->setUniform("camera_position", Novo::CurrentCamera::get_position());

                _shader->setUniform("ambient_factor", _material->ambient_factor);
                _shader->setUniform("diffuse_factor", _material->diffuse_factor);
                _shader->setUniform("specular_factor", _material->specular_factor);
                _shader->setUniform("shininess", _material->shininess);

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

            virtual glm::vec3 get_position() { return _position; }
            virtual glm::vec3 get_size() { return _size; }
            virtual glm::vec3 get_rotation() { return _rotation; }
            virtual glm::vec2 get_uv() { return _uv; }
            virtual const Novo::MeshID get_id() const { return MeshID::MeshBase; }

            virtual std::shared_ptr<Shader> get_shader() { return _shader; }
            virtual std::shared_ptr<Texture2D> get_texture() { return _texture; }
            virtual std::shared_ptr<Material> get_material() { return _material; }

            virtual void set_material(const Material& material) {
                *_material = material;
            }

            virtual void show(bool draw = true) {
                _draw = draw;
            }
            
            virtual void hide(bool draw = false) {
                _draw = draw;
            }

            virtual bool is_visible() {
                return _draw;
            }

            virtual void draw_ui(const std::string& tab_name) {
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
                if (ImGui::TreeNode("Material")) {
                    if (ImGui::DragFloat("Ambient factor", &_material->ambient_factor, 0.01f)) {
                        set_material(*_material);
                    }
                    if (ImGui::DragFloat("Diffuse factor", &_material->diffuse_factor, 0.01f)) {
                        set_material(*_material);
                    }
                    if (ImGui::DragFloat("Specular factor", &_material->specular_factor, 0.01f)) {
                        set_material(*_material);
                    }
                    if (ImGui::DragFloat("Shininess", &_material->shininess, 0.01f)) {
                        set_material(*_material);
                    }
                    ImGui::TreePop();
                }
                ImGui::Checkbox("Show", &_draw);
            }

            virtual void reload() {
                set_uv(_uv);
            }
        };
    }
}
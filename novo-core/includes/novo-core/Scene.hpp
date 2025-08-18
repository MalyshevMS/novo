#pragma once

#include <novo-core/Mesh/MeshBase.hpp>
#include <novo-core/Mesh/Box.hpp>
#include <novo-core/Mesh/Plane.hpp>
#include <novo-core/Mesh/LightSource.hpp>
#include <vector>

namespace Novo {
    class Scene {
    private:
        using ObjPair = std::pair<std::shared_ptr<Novo::Mesh::MeshBase>, std::string>;
        using LightPair = std::pair<std::shared_ptr<Novo::Mesh::LightSource>, std::string>;
        std::vector<ObjPair> _objects = {};
        std::vector<LightPair> _lights = {};
        std::shared_ptr<Novo::Resources> _resources;
    public:
        Scene(std::shared_ptr<Novo::Resources> resources) {
            _resources = resources;
        }

        void add_object(const Novo::Mesh::MeshBase& obj) {
            _objects.push_back(ObjPair(std::make_shared<Novo::Mesh::MeshBase>(obj), "Scene object #" + std::to_string(_objects.size())));
        }

        void add_object(const std::shared_ptr<Novo::Mesh::MeshBase>& obj) {
            _objects.push_back(ObjPair(obj, "Scene object #" + std::to_string(_objects.size())));
        }

        void add_object(const Novo::Mesh::MeshBase& obj, const std::string& name) {
            _objects.push_back(ObjPair(std::make_shared<Novo::Mesh::MeshBase>(obj), name));
        }

        void add_object(const std::shared_ptr<Novo::Mesh::MeshBase>& obj, const std::string& name) {
            _objects.push_back(ObjPair(obj, name));
        }

        void add_light(const Novo::Mesh::LightSource& light) {
            _lights.push_back(LightPair(std::make_shared<Novo::Mesh::LightSource>(light), "Light #" + std::to_string(_lights.size())));
        }

        void add_light(const std::shared_ptr<Novo::Mesh::LightSource>& light) {
            _lights.push_back(LightPair(light, "Light #" + std::to_string(_lights.size())));
        }

        void add_light(const Novo::Mesh::LightSource& light, const std::string& name) {
            _lights.push_back(LightPair(std::make_shared<Novo::Mesh::LightSource>(light), name));
        }

        void add_light(const std::shared_ptr<Novo::Mesh::LightSource>& light, const std::string& name) {
            _lights.push_back(LightPair(light, name));
        }

        void reload_all() {
            for (auto& obj : _objects) {
                obj.first->reload();
            }
        }

        void draw_ui() {
            ImGui::Begin("Scene");
            ImGui::Text("Objects");
            ImGui::Separator();
            ImGui::SetWindowFontScale(1.5f);
            for (auto& obj : _objects) {
                obj.first->draw_ui(obj.second);
            }
            ImGui::Separator();
            ImGui::Text("Lights");
            ImGui::Separator();
            for (auto& light : _lights) {
                light.first->draw_ui(light.second);
            }
            ImGui::Separator();

            static bool isAddingObject = false;
            static bool isAddingLight = false;
            static bool isAddingMaterial = false;
            static bool isAddingShader = false;

            if (ImGui::Button("Add object...")) {
                isAddingObject = true;
            }
            if (ImGui::Button("Add light...")) {
                isAddingLight = true;
            }
            if (ImGui::Button("Add material...")) {
                isAddingMaterial = true;
            }
            if (ImGui::Button("Add shader...")) {
                isAddingShader = true;
            }

            if (isAddingObject) {
                static std::string type = "Box";
                static std::string texture = "None";
                static std::string shader = "None";
                static std::string material = "None";
                static std::string name = "";
                static std::vector<char> buffer(256);

                if (name.size() >= buffer.size()) {
                    buffer.resize(name.size() + 1);
                }
                memcpy(buffer.data(), name.c_str(), name.size() + 1);

                ImGui::Begin("Add object", &isAddingObject);
                ImGui::SetWindowFontScale(1.5f);

                if (ImGui::InputText("Name", buffer.data(), buffer.size())) {
                    name.assign(buffer.data());
                }
                if (ImGui::BeginCombo("Type", type.c_str())){
                    if (ImGui::Selectable("Box")) {
                        type = "Box";
                    }
                    if (ImGui::Selectable("Plane")) {
                        type = "Plane";
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::BeginCombo("Texture", texture.c_str())) {
                    for (auto& tex : _resources->getTexturesMap()) {
                        if (ImGui::Selectable(tex.first.c_str())) {
                            texture = tex.first;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::BeginCombo("Shader", shader.c_str())) {
                    for (auto& shad : _resources->getShadersMap()) {
                        if (ImGui::Selectable(shad.first.c_str())) {
                            shader = shad.first;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::BeginCombo("Material", material.c_str())) {
                    for (auto& mat : _resources->getMaterialsMap()) {
                        if (ImGui::Selectable(mat.first.c_str())) {
                            material = mat.first;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Add")) {
                    if (type == "Box") {
                        add_object(std::make_shared<Novo::Mesh::Box>(_resources->getTexture(texture), _resources->getShader(shader), _resources->getMaterial(material)), name);
                    } else if (type == "Plane") {
                        add_object(std::make_shared<Novo::Mesh::Plane>(_resources->getTexture(texture), _resources->getShader(shader), _resources->getMaterial(material)), name);
                    }
                    reload_all();
                    isAddingObject = false;
                }
                if (ImGui::Button("Cancel")) {
                    isAddingObject = false;
                }
                ImGui::End();
            }
            if (isAddingLight) {
                static std::string name = "";
                static std::vector<char> buffer(256);
                static glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
                static std::string shader = "None";

                if (name.size() >= buffer.size()) {
                    buffer.resize(name.size() + 1);
                }
                memcpy(buffer.data(), name.c_str(), name.size() + 1);

                ImGui::Begin("Add Light", &isAddingLight);
                ImGui::SetWindowFontScale(1.5f);
                if (ImGui::InputText("Name", buffer.data(), buffer.size())) {
                    name.assign(buffer.data());
                }
                ImGui::ColorEdit3("Color", &color.x);
                if (ImGui::BeginCombo("Shader", shader.c_str())) {
                    for (auto& shad : _resources->getShadersMap()) {
                        if (ImGui::Selectable(shad.first.c_str())) {
                            shader = shad.first;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Add")) {
                    add_light(std::make_shared<Novo::Mesh::LightSource>(color, _resources->getShader(shader)), name);
                    isAddingLight = false;
                }
                if (ImGui::Button("Cancel")) {
                    isAddingLight = false;
                }
                ImGui::End();
            }
            if (isAddingMaterial) {
                static std::shared_ptr<Novo::Material> material = std::make_shared<Novo::Material>();
                static std::string name = "";
                static std::vector<char> buffer(256);

                if (name.size() >= buffer.size()) {
                    buffer.resize(name.size() + 1);
                }
                memcpy(buffer.data(), name.c_str(), name.size() + 1);

                ImGui::Begin("Add material", &isAddingMaterial);
                ImGui::SetWindowFontScale(1.5f);
                if (ImGui::InputText("Name", buffer.data(), buffer.size())) {
                    name.assign(buffer.data());
                }
                ImGui::DragFloat("Ambient factor", &material->ambient_factor, 0.01f);
                ImGui::DragFloat("Diffuse factor", &material->diffuse_factor, 0.01f);
                ImGui::DragFloat("Specular factor", &material->specular_factor, 0.01f);
                ImGui::DragFloat("Shininess", &material->shininess, 0.01f);
                if (ImGui::Button("Add")) {
                    _resources->addMaterial(name, material);
                    isAddingMaterial = false;
                }
                if (ImGui::Button("Cancel")) {
                    isAddingMaterial = false;
                }

                ImGui::End();
            }
            if (isAddingShader) {
                static std::string name = "";
                static std::vector<char> buffer_name(256);
                static std::string vertex_path = "";
                static std::vector<char> buffer_vertex(256);
                static std::string fragment_path = "";
                static std::vector<char> buffer_fragment(256);

                if (name.size() >= buffer_name.size()) {
                    buffer_name.resize(name.size() + 1);
                }
                memcpy(buffer_name.data(), name.c_str(), name.size() + 1);

                if (vertex_path.size() >= buffer_vertex.size()) {
                    buffer_vertex.resize(vertex_path.size() + 1);
                }
                memcpy(buffer_vertex.data(), vertex_path.c_str(), vertex_path.size() + 1);

                if (fragment_path.size() >= buffer_fragment.size()) {
                    buffer_fragment.resize(fragment_path.size() + 1);
                }
                memcpy(buffer_fragment.data(), fragment_path.c_str(), fragment_path.size() + 1);

                ImGui::Begin("Add shader", &isAddingShader);
                ImGui::SetWindowFontScale(1.5f);
                if (ImGui::InputText("Name", buffer_name.data(), buffer_name.size())) {
                    name.assign(buffer_name.data());
                }
                if (ImGui::InputText("Vertex path", buffer_vertex.data(), buffer_vertex.size())) {
                    vertex_path.assign(buffer_vertex.data());
                }
                if (ImGui::InputText("Fragment path", buffer_fragment.data(), buffer_fragment.size())) {
                    fragment_path.assign(buffer_fragment.data());
                }
                if (ImGui::Button("Add")) {
                    _resources->loadShader(name, vertex_path, fragment_path);
                    isAddingShader = false;
                }
                if (ImGui::Button("Cancel")) {
                    isAddingShader = false;
                }
                ImGui::End();
            }

            ImGui::End();
        }

        void render() {
            for (auto& obj : _objects) {
                for (int i = 0; i < _lights.size(); ++i) {
                    if (!_lights[i].first->is_visible()) {
                        obj.first->get_shader()->load();
                        obj.first->get_shader()->insertUniformArray("light_colors", glm::vec3(0.f), i);
                        obj.first->get_shader()->unload();
                        continue;
                    }
                    _lights[i].first->draw();
                    obj.first->get_shader()->load();
                    obj.first->get_shader()->insertUniformArray("light_colors", _lights[i].first->get_light_color(), i);
                    obj.first->get_shader()->insertUniformArray("light_positions", _lights[i].first->get_position(), i);
                    obj.first->get_shader()->unload();
                }
                obj.first->draw();
            }
        }
    };
}
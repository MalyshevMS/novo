#pragma once

#include <novo-core/Mesh/MeshBase.hpp>
#include <novo-core/Mesh/Box.hpp>
#include <novo-core/Mesh/Plane.hpp>
#include <vector>

namespace Novo {
    class Scene {
    private:
        using ObjPair = std::pair<std::shared_ptr<Novo::Mesh::MeshBase>, std::string>;
        std::vector<ObjPair> _objects = {};
        std::shared_ptr<Novo::Resources> _resources;
        bool _isAddingObject = false;
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

        void reload_all() {
            for (auto& obj : _objects) {
                obj.first->reload();
            }
        }

        void draw_ui() {
            ImGui::Begin("Scene");
            ImGui::SetWindowFontScale(1.5f);
            for (auto& obj : _objects) {
                obj.first->draw_ui(obj.second);
            }

            if (ImGui::Button("Add object...")) {
                _isAddingObject = true;
            }

            if (_isAddingObject) {
                static std::string type = "Box";
                static std::string texture = "None";
                static std::string shader = "None";

                ImGui::Begin("Add object", &_isAddingObject);
                ImGui::SetWindowFontScale(1.5f);
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
                if (ImGui::Button("Add")) {
                    std::cout << "Adding object: " << type << " with texture: " << texture << " and shader: " << shader << std::endl;
                    if (type == "Box") {
                        add_object(std::make_shared<Novo::Mesh::Box>(_resources->getTexture(texture), _resources->getShader(shader)));
                    } else if (type == "Plane") {
                        add_object(std::make_shared<Novo::Mesh::Plane>(_resources->getTexture(texture), _resources->getShader(shader)));
                    }
                    reload_all();
                    _isAddingObject = false;
                }
                if (ImGui::Button("Cancel")) {
                    _isAddingObject = false;
                }
                ImGui::End();
            }

            ImGui::End();
        }

        void render() {
            for (auto& obj : _objects) {
                obj.first->draw();
            }
        }
    };
}
#pragma once

#include <novo-core/Mesh/MeshBase.hpp>
#include <vector>

namespace Novo {
    class Scene {
    private:
        using ObjPair = std::pair<std::shared_ptr<Novo::Mesh::MeshBase>, std::string>;
        std::vector<ObjPair> _objects = {};
    public:
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
            ImGui::End();
        }

        void render() {
            for (auto& obj : _objects) {
                obj.first->draw();
            }
        }
    };
}
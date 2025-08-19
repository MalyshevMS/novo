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
        std::string _name = "Scene";
    public:
        Scene(std::shared_ptr<Novo::Resources> resources) {
            _resources = resources;
        }

        Scene& load_from_json(const std::string& path) {
            Json json = Json::parse(_resources->getFileStr(path));

            _name = json["name"];
            
            std::vector<Json> shaders = json["shaders"];
            for (auto& shader : shaders) {
                _resources->loadShader(shader["name"], shader["vs"], shader["fs"]);
            }

            std::vector<Json> materials = json["materials"];
            for (auto& material : materials) {
                _resources->loadMaterial(material["name"], material["path"]);
            }

            std::vector<Json> textures = json["textures"];
            for (auto& texture : textures) {
                _resources->loadTexture(texture["name"], texture["path"]);
            }

            std::vector<Json> objects = json["objects"];
            for (auto& obj : objects) {
                auto shader = _resources->getShader(obj["shader"]);

                glm::vec3 position = glm::vec3(obj["transform"]["position"]["x"], obj["transform"]["position"]["y"], obj["transform"]["position"]["z"]);
                glm::vec3 rotation = glm::vec3(obj["transform"]["rotation"]["x"], obj["transform"]["rotation"]["y"], obj["transform"]["rotation"]["z"]);
                glm::vec3 scale    = glm::vec3(obj["transform"]["scale"]   ["x"], obj["transform"]["scale"]   ["y"], obj["transform"]["scale"]   ["z"]);
                glm::vec2 uv       = glm::vec2(obj["uv.x"], obj["uv.y"]);
                if (obj["type.id"] == Novo::MeshID::Box) {
                    auto material = _resources->getMaterial(obj["material"]);
                    auto texture = _resources->getTexture(obj["texture"]);
                    auto p_obj = std::make_shared<Novo::Mesh::Box>(texture, shader, material, position, scale, rotation);
                    p_obj->set_uv(uv);
                    add_object(p_obj, obj["name"]);
                } else if (obj["type.id"] == Novo::MeshID::Plane) {
                    auto material = _resources->getMaterial(obj["material"]);
                    auto texture = _resources->getTexture(obj["texture"]);
                    auto p_obj = std::make_shared<Novo::Mesh::Plane>(texture, shader, material, position, scale, rotation);
                    p_obj->set_uv(uv);
                    add_object(p_obj, obj["name"]);
                } else if (obj["type.id"] == Novo::MeshID::Light) {
                    Json properties = obj["other"]["Light"];
                    glm::vec3 color = glm::vec3(properties["color"]["r"], properties["color"]["g"], properties["color"]["b"]);

                    add_light(std::make_shared<Novo::Mesh::LightSource>(color, shader, position, scale, rotation), obj["name"]);
                }
            }

            return *this;
        }

        Json save_to_json(const std::string& path) {
            Json json;
            json["name"] = _name;
            json["shaders"] = Json::array();
            json["materials"] = Json::array();
            json["textures"] = Json::array();
            json["objects"] = Json::array();

            for (auto& shader : _resources->getShadersMap()) {
                Json shaderJson;
                shaderJson["name"] = shader.first;
                shaderJson["vs"] = _resources->getShaderPaths()[shader.first].first;
                shaderJson["fs"] = _resources->getShaderPaths()[shader.first].second;
                json["shaders"].push_back(shaderJson);
            }

            for (auto& material : _resources->getMaterialsMap()) {
                Json materialJson;
                materialJson["name"] = material.first;
                materialJson["path"] = _resources->getMaterialsPaths()[material.first];
                json["materials"].push_back(materialJson);
            }

            for (auto& texture : _resources->getTexturesMap()) {
                Json textureJson;
                textureJson["name"] = texture.first;
                textureJson["path"] = _resources->getTexturesPaths()[texture.first];
                json["textures"].push_back(textureJson);
            }

            for (auto& obj : _objects) {
                Json objJson;
                objJson["name"] = obj.second;
                objJson["type.id"] = obj.first->get_id();
                objJson["shader"] = _resources->getShaderName(obj.first->get_shader());
                objJson["material"] = _resources->getMaterialName(obj.first->get_material());
                objJson["texture"] = _resources->getTextureName(obj.first->get_texture());
                objJson["uv.x"] = obj.first->get_uv().x;
                objJson["uv.y"] = obj.first->get_uv().y;
                objJson["transform"] = {
                    {"position", {
                        {"x", obj.first->get_position().x},
                        {"y", obj.first->get_position().y},
                        {"z", obj.first->get_position().z}
                    }},
                    {"rotation", {
                        {"x", obj.first->get_rotation().x},
                        {"y", obj.first->get_rotation().y},
                        {"z", obj.first->get_rotation().z}
                    }},
                    {"scale", {
                        {"x", obj.first->get_size().x},
                        {"y", obj.first->get_size().y},
                        {"z", obj.first->get_size().z}
                    }}
                };
                objJson["other"] = {};

                json["objects"].push_back(objJson);
            }

            for (auto& light : _lights) {
                Json lightJson;
                lightJson["name"] = light.second;
                lightJson["type.id"] = light.first->get_id();
                lightJson["shader"] = _resources->getShaderName(light.first->get_shader());
                lightJson["material"] = _resources->getMaterialName(light.first->get_material());
                lightJson["texture"] = _resources->getTextureName(light.first->get_texture());
                lightJson["uv.x"] = light.first->get_uv().x,
                lightJson["uv.y"] = light.first->get_uv().y,
                lightJson["transform"] = {
                    {"position", {
                        {"x", light.first->get_position().x},
                        {"y", light.first->get_position().y},
                        {"z", light.first->get_position().z}
                    }},
                    {"rotation", {
                        {"x", light.first->get_rotation().x},
                        {"y", light.first->get_rotation().y},
                        {"z", light.first->get_rotation().z}
                    }},
                    {"scale", {
                        {"x", light.first->get_size().x},
                        {"y", light.first->get_size().y},
                        {"z", light.first->get_size().z}
                    }}
                };
                lightJson["other"]["Light"] = {
                    {"color", {
                        {"r", light.first->get_light_color().r},
                        {"g", light.first->get_light_color().g},
                        {"b", light.first->get_light_color().b}
                    }}
                };

                json["objects"].push_back(lightJson);
            }

            std::fstream file;
            file.open(path, std::ios::out);
            if (file.is_open()) {
                file << json << std::endl;
            } else {
                std::cerr << "Failed to open file " << path << std::endl;
            }
            return json;
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

        void clear() {
            _objects.clear();
            _lights.clear();
        }

        void draw_ui() {
            ImGui::Begin(_name.c_str());
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

            static bool isSaving = false;
            static bool isOpening = false;

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
            ImGui::Separator();
            if (ImGui::Button("Save")) {
                isSaving = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Open")) {
                isOpening = true;
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

            if (isSaving) {
                ImGui::Begin("Save", &isSaving);
                ImGui::SetWindowFontScale(1.5f);
                static std::string path = "";
                static std::vector<char> buffer(256);

                if (path.size() >= buffer.size()) {
                    buffer.resize(path.size() + 1);
                }
                memcpy(buffer.data(), path.c_str(), path.size() + 1);

                if (ImGui::InputText("File Path", buffer.data(), buffer.size())) {
                    path.assign(buffer.data());
                }
                ImGui::Text("Scene file will be saved as:\n%s", (_resources->getExePath() + path).c_str());
                if (ImGui::Button("Save")) {
                    save_to_json(_resources->getExePath() + path);
                    isSaving = false;
                }
                if (ImGui::Button("Cancel")) {
                    isSaving = false;
                }
                ImGui::End();
            }
            if (isOpening) {
                if (ImGui::Begin("Open", &isOpening)) {
                    ImGui::SetWindowFontScale(1.5f);
                    static std::string path = "";
                    static std::vector<char> buffer(256);

                    if (path.size() >= buffer.size()) {
                        buffer.resize(path.size() + 1);
                    }
                    memcpy(buffer.data(), path.c_str(), path.size() + 1);

                    if (ImGui::InputText("File Path", buffer.data(), buffer.size())) {
                        path.assign(buffer.data());
                    }
                    if (ImGui::Button("Open")) {
                        clear();
                        load_from_json(path);
                        reload_all();
                        isOpening = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        isOpening = false;
                    }
                    ImGui::End();
                }
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
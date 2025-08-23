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
        std::map<int, ObjPair> _objects  = {}; // first - id, second - ObjPair
        std::map<int, LightPair> _lights = {}; // first - id, second - LightPair
        std::shared_ptr<Novo::Resources> _resources;
        std::string _name = "Scene";
        int _lastID = 0;
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
                objJson["name"] = obj.second.second;
                objJson["type.id"] = obj.second.first->get_id();
                objJson["shader"] = _resources->getShaderName(obj.second.first->get_shader());
                objJson["material"] = _resources->getMaterialName(obj.second.first->get_material());
                objJson["texture"] = _resources->getTextureName(obj.second.first->get_texture());
                objJson["uv.x"] = obj.second.first->get_uv().x;
                objJson["uv.y"] = obj.second.first->get_uv().y;
                objJson["transform"] = {
                    {"position", {
                        {"x", obj.second.first->get_position().x},
                        {"y", obj.second.first->get_position().y},
                        {"z", obj.second.first->get_position().z}
                    }},
                    {"rotation", {
                        {"x", obj.second.first->get_rotation().x},
                        {"y", obj.second.first->get_rotation().y},
                        {"z", obj.second.first->get_rotation().z}
                    }},
                    {"scale", {
                        {"x", obj.second.first->get_size().x},
                        {"y", obj.second.first->get_size().y},
                        {"z", obj.second.first->get_size().z}
                    }}
                };
                objJson["other"] = {};

                json["objects"].push_back(objJson);
            }

            for (auto& light : _lights) {
                Json lightJson;
                lightJson["name"] = light.second.second;
                lightJson["type.id"] = light.second.first->get_id();
                lightJson["shader"] = _resources->getShaderName(light.second.first->get_shader());
                lightJson["material"] = _resources->getMaterialName(light.second.first->get_material());
                lightJson["texture"] = _resources->getTextureName(light.second.first->get_texture());
                lightJson["uv.x"] = light.second.first->get_uv().x,
                lightJson["uv.y"] = light.second.first->get_uv().y,
                lightJson["transform"] = {
                    {"position", {
                        {"x", light.second.first->get_position().x},
                        {"y", light.second.first->get_position().y},
                        {"z", light.second.first->get_position().z}
                    }},
                    {"rotation", {
                        {"x", light.second.first->get_rotation().x},
                        {"y", light.second.first->get_rotation().y},
                        {"z", light.second.first->get_rotation().z}
                    }},
                    {"scale", {
                        {"x", light.second.first->get_size().x},
                        {"y", light.second.first->get_size().y},
                        {"z", light.second.first->get_size().z}
                    }}
                };
                lightJson["other"]["Light"] = {
                    {"color", {
                        {"r", light.second.first->get_light_color().r},
                        {"g", light.second.first->get_light_color().g},
                        {"b", light.second.first->get_light_color().b}
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
            _objects[_lastID] = ObjPair(std::make_shared<Novo::Mesh::MeshBase>(obj), "Scene object #" + std::to_string(_lastID));
            ++_lastID;
        }

        void add_object(const std::shared_ptr<Novo::Mesh::MeshBase>& obj) {
            _objects[_lastID] = ObjPair(obj, "Scene object #" + std::to_string(_lastID));
            ++_lastID;
        }

        void add_object(const Novo::Mesh::MeshBase& obj, const std::string& name) {
            _objects[_lastID] = ObjPair(std::make_shared<Novo::Mesh::MeshBase>(obj), name);
            ++_lastID;
        }

        void add_object(const std::shared_ptr<Novo::Mesh::MeshBase>& obj, const std::string& name) {
            _objects[_lastID] = ObjPair(obj, name);
            ++_lastID;
        }

        void add_light(const Novo::Mesh::LightSource& light) {
            _lights[_lastID] = LightPair(std::make_shared<Novo::Mesh::LightSource>(light), "Light #" + std::to_string(_lastID));
            ++_lastID;
        }

        void add_light(const std::shared_ptr<Novo::Mesh::LightSource>& light) {
            _lights[_lastID] = LightPair(light, "Light #" + std::to_string(_lastID));
            ++_lastID;
        }

        void add_light(const Novo::Mesh::LightSource& light, const std::string& name) {
            _lights[_lastID] = LightPair(std::make_shared<Novo::Mesh::LightSource>(light), name);
            ++_lastID;
        }

        void add_light(const std::shared_ptr<Novo::Mesh::LightSource>& light, const std::string& name) {
            _lights[_lastID] = LightPair(light, name);
            ++_lastID;
        }

        void reload_all() {
            for (auto& obj : _objects) {
                obj.second.first->reload();
            }
        }

        void clear() {
            _objects.clear();
            _lights.clear();
        }

        void draw_ui() {
            ImGui::Begin(_name.c_str());
            ImGui::SetWindowFontScale(1.5f);
            static int selected_object = INT_MAX;
            static std::string selected_object_str = "None [id: " + std::to_string(selected_object) + "]";
            if (ImGui::BeginCombo("Selected object", selected_object_str.c_str())) {
                if (ImGui::Selectable(("None [id: " + std::to_string(INT_MAX) + "]").c_str())) {
                    selected_object = INT_MAX;
                    selected_object_str = "None [id: " + std::to_string(selected_object) + "]";
                }
                for (auto& obj : _objects) {
                    if (ImGui::Selectable((obj.second.second + " [id: " + std::to_string(obj.first) + "]").c_str())) {
                        selected_object = obj.first;
                        selected_object_str = obj.second.second + " [id: " + std::to_string(selected_object) + "]";
                    }
                }
                for (auto& light : _lights) {
                    if (ImGui::Selectable((light.second.second + " [id: " + std::to_string(light.first) + "]").c_str())) {
                        selected_object = light.first;
                        selected_object_str = light.second.second + " [id: " + std::to_string(selected_object) + "]";
                    }
                }
                ImGui::EndCombo();
            }
            for (auto& obj : _objects) if (obj.first == selected_object) {
                static std::vector<char> buffer(256);
                if (obj.second.second.size() >= buffer.size()) {
                    buffer.resize(obj.second.second.size() + 1);
                }
                memcpy(buffer.data(), obj.second.second.c_str(), obj.second.second.size() + 1);

                if (ImGui::InputText("Name", buffer.data(), buffer.size())) {
                    obj.second.second.assign(buffer.data());
                    selected_object_str = obj.second.second + " [id: " + std::to_string(selected_object) + "]";
                }
                obj.second.first->draw_ui(obj.second.second);
            }
            for (auto& light : _lights) if (light.first == selected_object) {
                static std::vector<char> buffer(256);
                if (light.second.second.size() >= buffer.size()) {
                    buffer.resize(light.second.second.size() + 1);
                }
                memcpy(buffer.data(), light.second.second.c_str(), light.second.second.size() + 1);

                if (ImGui::InputText("Name", buffer.data(), buffer.size())) {
                    light.second.second.assign(buffer.data());
                    selected_object_str = light.second.second + " [id: " + std::to_string(selected_object) + "]";
                }
                light.second.first->draw_ui(light.second.second);
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
                for (auto& light : _lights) {
                    if (!light.second.first->is_active()) {
                        obj.second.first->get_shader()->load();
                        obj.second.first->get_shader()->insertUniformArray("light_colors", glm::vec3(0.f), light.first);
                        obj.second.first->get_shader()->unload();
                        continue;
                    }
                    light.second.first->draw();
                    obj.second.first->get_shader()->load();
                    obj.second.first->get_shader()->insertUniformArray("light_colors", light.second.first->get_light_color(), light.first);
                    obj.second.first->get_shader()->insertUniformArray("light_positions", light.second.first->get_position(), light.first);
                    obj.second.first->get_shader()->unload();
                }
                obj.second.first->draw();
            }
        }
    };
}
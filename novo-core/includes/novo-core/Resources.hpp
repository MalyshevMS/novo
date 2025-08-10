#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "stb_image.h"

#include <novo-core/Texture2D.hpp>
#include <novo-core/Shader.hpp>

namespace Novo {
    using Byte = char;
    using Image = unsigned Byte*;

    class Resources {
    private:
        using TexturesMap = std::map<std::string, std::shared_ptr<Texture2D>>;

        std::string _exePath;
        TexturesMap _texturesMap;
    public:
        Resources(const std::string& exePath) {
            size_t found = exePath.find_last_of("/\\");
            _exePath = exePath.substr(0, found + 1);
        }

        std::string getFileStr(const std::string& path) {
            std::ifstream f;
            f.open(_exePath + path, std::ios::in | std::ios::binary);
            if (!f.is_open()) {
                std::cerr << "Failed to open file " << _exePath + path << std::endl;
                return std::string();
            }

            std::stringstream buffer;
            buffer << f.rdbuf();

            return buffer.str();
        };

        std::shared_ptr<Texture2D> loadTexture(const std::string& name, const std::string& path) {
            int width, height, channels;
            stbi_set_flip_vertically_on_load(true);
            Image image = stbi_load((_exePath + "/" + path).c_str(), &width, &height, &channels, 0);

            if (!image) {
                std::cerr << "Failed to load texture " << _exePath + path << std::endl;
            }

            _texturesMap[name] = std::make_shared<Texture2D>(image, glm::vec2(width, height), channels);
            stbi_image_free(image);

            return _texturesMap[name];
        }

        std::shared_ptr<Texture2D> getTexture(const std::string& name) {
            if (_texturesMap.find(name) != _texturesMap.end()) {
                return _texturesMap[name];
            }
            else {
                std::cerr << "Failed to find texture " << name << std::endl;
                return nullptr;
            }
        }
    };
}
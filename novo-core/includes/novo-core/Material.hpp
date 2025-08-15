#pragma once

namespace Novo {
    struct Material {
        float ambient_factor = 0.1f;
        float diffuse_factor = 10.f;
        float specular_factor = 1.f;
        float shininess = 32.f;

        Material(float ambient_factor = 0.1f, float diffuse_factor = 10.f, float specular_factor = 1.f, float shininess = 32.f) {
            this->ambient_factor = ambient_factor;
            this->diffuse_factor = diffuse_factor;
            this->specular_factor = specular_factor;
            this->shininess = shininess;
        }
    };
}
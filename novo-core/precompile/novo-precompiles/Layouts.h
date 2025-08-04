#pragma once

#include <novo-core/VBO.hpp>

namespace Novo {
    namespace Layout {
        const static BufferLayout vec1f = {
            ShaderDataType::Float
        };

        const static BufferLayout vec2f = {
            ShaderDataType::Float2
        };
        
        const static BufferLayout vec3f = {
            ShaderDataType::Float3
        };

        const static BufferLayout vec4f = {
            ShaderDataType::Float4
        };

        const static BufferLayout vec1i = {
            ShaderDataType::Int
        };
        
        const static BufferLayout vec2i = {
            ShaderDataType::Int2
        };

        const static BufferLayout vec3i = {
            ShaderDataType::Int3
        };

        const static BufferLayout vec4i = {
            ShaderDataType::Int4
        };

        const static BufferLayout l_2vec1f = {
            ShaderDataType::Float,
            ShaderDataType::Float
        };
        
        const static BufferLayout l_2vec2f = {
            ShaderDataType::Float2,
            ShaderDataType::Float2
        };
        
        const static BufferLayout l_2vec3f = {
            ShaderDataType::Float3,
            ShaderDataType::Float3
        };
        
        const static BufferLayout l_2vec4f = {
            ShaderDataType::Float4,
            ShaderDataType::Float4
        };
        
        const static BufferLayout l_2vec1i = {
            ShaderDataType::Int,
            ShaderDataType::Int
        };
        
        const static BufferLayout l_2vec2i = {
            ShaderDataType::Int2,
            ShaderDataType::Int2
        };
        
        const static BufferLayout l_2vec3i = {
            ShaderDataType::Int3,
            ShaderDataType::Int3
        };
        
        const static BufferLayout l_2vec4i = {
            ShaderDataType::Int4,
            ShaderDataType::Int4
        };

        static BufferLayout create(const std::initializer_list<BufferElement>& types) {
            return BufferLayout(std::move(types));
        }
    }
}
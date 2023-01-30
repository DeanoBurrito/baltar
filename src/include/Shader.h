#pragma once

#include <string_view>
#include <glm/common.hpp>

namespace Baltar
{
    class Shader
    {
    private:
        glm::uint programId = -1u;

    public:
        bool Load(std::string_view vertexSrc, std::string_view fragSrc);
        
        void Activate();
        glm::uint GetUniform(std::string_view attribName);
    };
}

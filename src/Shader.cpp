#include <Shader.h>
#include <Debug.h>
#include <3rdparty/Glad.h>
#include <cstring>

namespace Baltar
{
    bool Shader::Load(std::string_view vertexSrc, std::string_view fragSrc)
    {
        const char* vSrc = vertexSrc.data();
        glm::uint vertexId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexId, 1, &vSrc, nullptr);
        glCompileShader(vertexId);

        GLint success;
        glGetShaderiv(vertexId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char errorLog[512];
            glGetShaderInfoLog(vertexId, 512, nullptr, errorLog);
            Log("Vertex shader compilation failed: %s", LogLevel::Error, errorLog);
            return false;
        }

        const char* fSrc = fragSrc.data();
        glm::uint fragId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragId, 1, &fSrc, nullptr);
        glCompileShader(fragId);

        glGetShaderiv(fragId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char errorLog[512];
            glGetShaderInfoLog(fragId, 512, nullptr, errorLog);
            Log("Fragment shader compilation failed: %s", LogLevel::Error, errorLog);
            return false;
        }

        programId = glCreateProgram();
        glAttachShader(programId, vertexId);
        glAttachShader(programId, fragId);
        glLinkProgram(programId);

        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success)
        {
            char errorLog[512];
            glGetProgramInfoLog(programId, 512, nullptr, errorLog);
            Log("Shader program linking failed: %s", LogLevel::Error, errorLog);
            return false;
        }

        glValidateProgram(programId);
        glGetProgramiv(programId, GL_VALIDATE_STATUS, &success);
        if (!success)
        {
            char errorLog[512];
            glGetProgramInfoLog(programId, 512, nullptr, errorLog);
            Log("Shader program validation failed: %s", LogLevel::Error, errorLog);
            return false;
        }

        glDetachShader(programId, vertexId);
        glDetachShader(programId, fragId);
        glDeleteShader(vertexId);
        glDeleteShader(fragId);

        return true;
    }

    void Shader::Activate()
    {
        ASSERT(programId != -1u, StrUninitialized);
        glUseProgram(programId);
    }

    glm::uint Shader::GetUniform(std::string_view attribName)
    {
        ASSERT(programId != -1u, StrUninitialized);
        return glGetUniformLocation(programId, attribName.data());
    }
}

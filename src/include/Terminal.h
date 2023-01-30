#pragma once

#include <GlyphManager.h>
#include <Shader.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>

namespace Baltar
{
    class Terminal
    {
    private:
        GlyphManager glyphCache;
        Shader textShader;

        GLFWwindow* window = nullptr;
        glm::uint vboVertexData;
        glm::uint vao;

        char* buffer;
        size_t bufferLength;
        size_t buffBegin;
        size_t buffEnd;

        int ptyFd;
        pid_t shellPid;

        bool GfxInit();
        bool GfxShutdown();
        bool PtyInit();
        bool PtyShutdown();

        void Render();
        std::vector<size_t> GetLineLengths(size_t lineCount);

    public:
        Terminal();
        ~Terminal();

        Terminal(const Terminal&) = delete;
        Terminal& operator=(const Terminal&) = delete;
        Terminal(Terminal&&) = delete;
        Terminal& operator=(Terminal&&) = delete;

        void Run();
    };
}

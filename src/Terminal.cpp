#include <Terminal.h>
#include <Debug.h>
#include <ShaderSources.h>
#include <3rdparty/Glad.h>
#include <sys/mman.h>
#include <pty.h>
#include <poll.h>
#include <unistd.h>

//TODO: separate into line-based renderer
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Baltar
{
    void GlfwErrorCallback(int code, const char* message)
    {
        Log("GLFW error %u: %s", LogLevel::Fatal, code, message);
    }

    void GlfwResizeCallback(GLFWwindow* window, int width, int height)
    {
        Log("Window resized to %i, %i", LogLevel::Debug, width, height);
        glViewport(0, 0, width, height);
    }

    void GlfwCharCallback(GLFWwindow* window, uint32_t codepoint)
    {
        Log("Character received.", LogLevel::Debug);
    }

    void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffet)
    {
        Log("Scroll received %lf, %lf", LogLevel::Debug, xoffset, yoffet);
    }
    
    bool Terminal::GfxInit()
    {
        VERIFY(glfwInit(), false, "glfwInit()");

        //request openGL ES 3.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        //create window
        const size_t windowWidth = 800;
        const size_t windowHeight = 600;
        window = glfwCreateWindow(windowWidth, windowHeight, "Baltar Terminal", nullptr, nullptr);
        VERIFY(window != nullptr, false, "Window creation failed");
        glfwMakeContextCurrent(window);

        glfwSetFramebufferSizeCallback(window, GlfwResizeCallback);
        glfwSetCharCallback(window, GlfwCharCallback);
        glfwSetScrollCallback(window, GlfwScrollCallback);

        //acquire GL function pointers with GLAD
        VERIFY(gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress), false, "gladLoadGLES2Loader()");

        //setup default GL state
        glViewport(0, 0, windowWidth, windowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

        //setup glyph cache and text drawing shader
        glyphCache.Init();
        glyphCache.SetActiveFace("/usr/share/fonts/google-noto/NotoSansMono-Black.ttf", 18);
        VERIFY(textShader.Load(VertexShaderSrc, FragShaderSrc), false, "Shader::Load()");

        //populate glyph quad TODO: multi-glyph by default?
        const float quadVertices[]
        {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 
        
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

        glGenBuffers(1, &vboVertexData);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertexData);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        //setup VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }

    bool Terminal::GfxShutdown()
    {
        glfwTerminate();
        Log("Terminal exiting...", LogLevel::Info);
        return true;
    }

    bool Terminal::PtyInit()
    {
        const pid_t pid = forkpty(&ptyFd, nullptr, nullptr, nullptr);
        if (pid == 0)
        {
            execle("/bin/bash", "/bin/bash", nullptr, environ);
        }
        else
            shellPid = pid;
        Log("Child is alive", LogLevel::Info);

        return true;
    }

    bool Terminal::PtyShutdown()
    {
        return true;
    }

    void Terminal::Render()
    {
        //initial rendering setup
        textShader.Activate();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(textShader.GetUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(textShader.GetUniform("glyphColor"), 1.0f, 1.0f, 1.0f);
        glUniform1i(textShader.GetUniform("glyphTexture"), 0);

        const float scale = 1.0f;
        float x = 0.0f;
        float y = 0.0f;
        
        for (size_t i = buffBegin; i != buffEnd; i++)
        {
            if (buffer[i % bufferLength] == '\n')
            {
                y += glyphCache.MaxVerticalBearing();
                continue;
            }
            else if (buffer[i % bufferLength] == '\r')
            {
                x = 0.0f;
                continue;
            }
            
            const Glyph glyph = glyphCache.GetGlyph(buffer[i % bufferLength]);

            const float xPos = x + glyph.bearing.x * scale;
            const float yPos = y + (glyphCache.MaxVerticalBearing() - glyph.bearing.y) * scale;
            
            glm::mat4 model = glm::identity<glm::mat4>();
            model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
            model = glm::scale(model, glm::vec3(glyph.size.x * scale, glyph.size.y * scale, 1.0f));
            glUniformMatrix4fv(textShader.GetUniform("model"), 1, GL_FALSE, glm::value_ptr(model));

            glBindTexture(GL_TEXTURE_2D, glyph.texId);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            x += (glyph.advance >> 6) * scale;
        }

        //reset bound state to prevent future accidents
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    Terminal::Terminal()
    {
        ASSERT(PtyInit(), "PtyInit()");

        glfwSetErrorCallback(GlfwErrorCallback);
        ASSERT(GfxInit(), "GfxInit()");

        bufferLength = 0x4000;
        buffer = reinterpret_cast<char*>(mmap(NULL, bufferLength, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        ASSERT(buffer != (char*)-1, "scrollback buffer mmap() failed.");
        buffBegin = 0;
        buffEnd = 0;
    }

    Terminal::~Terminal()
    {
        GfxShutdown();
        PtyShutdown();
    }

    void Terminal::Run()
    {
        ASSERT(window != nullptr, StrUninitialized);
        
        constexpr float EventWaitTimeout = 0.1f;
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);
            Render();

            pollfd pollFd { .fd = ptyFd, .events = POLLIN, .revents = 0 };
            if (poll(&pollFd, 1, 0))
            {
                Log("Got data!", LogLevel::Debug);
                char readBuffer[256];
                const size_t readLen = read(ptyFd, readBuffer, 100);

                for (size_t i = 0; i < readLen; i++) //TODO: collapse this into directly reading into the incoming buffer
                    buffer[(buffEnd + i) % bufferLength] = readBuffer[i];
                buffEnd += readLen;
            }

            glfwSwapBuffers(window);
            glfwWaitEventsTimeout(EventWaitTimeout);
        }
    }
}

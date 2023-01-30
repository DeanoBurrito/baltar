namespace Baltar
{
    const char* VertexShaderSrc = " \
    #version 300 es \n\
    layout (location = 0) in vec4 vertex; \n\
    out vec2 texCoords; \n\
    \n\
    uniform mat4 projection; \n\
    uniform mat4 model; \n\
    \n\
    void main() \n\
    { \n\
        gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); \n\
        texCoords = vertex.zw; \n\
    } \n\
    ";
    
    const char* FragShaderSrc = " \n\
    #version 300 es \n\
    precision mediump float; \n\
    in vec2 texCoords; \n\
    out vec4 color; \n\
    \n\
    uniform sampler2D glyphTexture; \n\
    uniform vec3 glyphColor; \n\
    \n\
    void main() \n\
    { \n\
        color = vec4(glyphColor, texture2D(glyphTexture, texCoords).r); \n\
    } \n\
    ";
}

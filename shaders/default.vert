#version 420 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 VertexColor;

layout (binding = 1) uniform Object {
    mat4 vp;
    mat4 model;
    vec4 color;
    bool hasTexture;
    bool premultiplyOutput;
} object;

void main() {
    gl_Position = object.vp * object.model * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
    VertexColor = aColor;
}

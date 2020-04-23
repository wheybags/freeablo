#version 330

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_uv;

out vec2 uv;

layout(std140) uniform vertexUniforms
{
    vec2 scaleOrigin;
    float scale;

    float pad1;
};

void main()
{
    uv = vertex_uv;

    vec2 pos = vertex_position;
    pos = pos - scaleOrigin;
    pos *= scale;
    pos = pos + scaleOrigin;

    gl_Position = vec4(pos, 0.0, 1.0);
}

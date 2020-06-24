#version 330

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;

out vec4 color;

void main()
{
    color = vertex_color;

    // convert to NDC
    vec2 pos = vertex_position;
    pos = pos * 2 - 1;
    pos.y = -pos.y;

    gl_Position = vec4(pos, 0.0, 1.0);
}

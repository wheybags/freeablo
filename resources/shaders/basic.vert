#version 330
precision mediump float;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec2 v_imageSize;
layout (location = 3) in vec2 v_imageOffset;
layout (location = 4) in vec4 v_hoverColor;
layout (location = 5) in vec3 v_atlasOffset;

out vec2 uv;
flat out vec2 imageSize;
flat out vec2 imageOffset;
flat out vec4 hoverColor;
flat out vec3 atlasOffset;

uniform float screenWidth;
uniform float screenHeight;

void main() {
    uv = v_uv;
    imageSize = v_imageSize;
    imageOffset = v_imageOffset;
    hoverColor = v_hoverColor;
    atlasOffset = v_atlasOffset;
    
    gl_Position = vec4((vec2(vertex_position.x*v_imageSize.x + v_imageOffset.x, vertex_position.y*v_imageSize.y + v_imageOffset.y) / vec2(screenWidth, screenHeight)) * 2.0, 0.0, 1.0);
    gl_Position.x = gl_Position.x - 1.0;
    gl_Position.y = 1.0 - gl_Position.y;
}

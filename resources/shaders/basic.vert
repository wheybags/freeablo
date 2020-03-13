#version 330
precision mediump float;

in vec3 vertex_position;
in vec2 v_uv;
in vec2 v_imageSize;
in vec2 v_imageOffset;
in vec4 v_hoverColor;
in vec3 v_atlasOffset;

out vec2 uv;
flat out vec2 imageSize;
flat out vec4 hoverColor;
flat out vec3 atlasOffset;

uniform vec2 screenSize;

void main() {
    uv = v_uv;
    imageSize = v_imageSize;
    hoverColor = v_hoverColor;
    atlasOffset = v_atlasOffset;
    
    gl_Position = vec4((vertex_position.xy * v_imageSize + v_imageOffset) / screenSize * 2.0, 0.0, 1.0);
    gl_Position.x = gl_Position.x - 1.0;
    gl_Position.y = 1.0 - gl_Position.y;
}

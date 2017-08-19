#version 320 es
precision mediump float;

in vec3 vertex_position;
in vec2 v_uv;
out vec2 uv;
uniform float width;
uniform float height;
uniform float imgW;
uniform float imgH;
uniform float offsetX;
uniform float offsetY;
void main() {
    uv = v_uv;
    gl_Position = vec4((vec2(vertex_position.x*imgW + offsetX, vertex_position.y*imgH + offsetY) / vec2(width, height)) * 2.0, 0.0, 1.0);
    gl_Position.x = gl_Position.x - 1.0;
    gl_Position.y = 1.0 - gl_Position.y;
}

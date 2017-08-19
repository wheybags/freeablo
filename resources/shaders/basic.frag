#version 320 es
precision mediump float;

in vec2 uv;
out vec4 frag_colour;
uniform sampler2D tex;
void main() {
    vec4 c = texture(tex, uv);
	frag_colour = c;//vec4(c.r, c.g, c.b, 0.4 * c.a);
}

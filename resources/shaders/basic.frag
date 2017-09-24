#version 320 es
precision mediump float;

in vec2 uv;
out vec4 frag_colour;
uniform sampler2D tex;
uniform float h_color_r;
uniform float h_color_g;
uniform float h_color_b;
uniform float h_color_a;
uniform float imgW;
uniform float imgH;

void main() {
    vec4 c = texture(tex, uv);
    if (c.w == 0. && h_color_a > 0.)
    {
      for (float i= -1.; i <= 1.; i++)
        for (float j= -1.; j <= 1.; j++)
            {
              vec4 n = texture(tex, vec2 (uv.x + i/imgW, uv.y + j/imgH));
              if (n.w > 0. && (n.x > 0. || n.y > 0. || n.z > 0.))
                c = vec4 (h_color_r, h_color_g, h_color_b, h_color_a);
            }
    }
	frag_colour = c;//vec4(c.r, c.g, c.b, 0.4 * c.a);
}

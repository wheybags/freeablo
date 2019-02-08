#version 330
precision mediump float;

in vec2 uv;
flat in vec2 imageSize;
flat in vec2 imageOffset;
flat in vec4 hoverColor;
flat in vec3 atlasOffset;

out vec4 frag_colour;
uniform sampler2DArray tex;
uniform float atlasWidth;
uniform float atlasHeight;

void main() {
    vec4 c = texture(tex, vec3((atlasOffset.x + uv.x * imageSize.x) / atlasWidth, (atlasOffset.y + uv.y * imageSize.y) / atlasHeight, atlasOffset.z));

    if (c.w == 0. && hoverColor.a > 0.)
    {
      for (float i= -1.; i <= 1.; i++)
        for (float j= -1.; j <= 1.; j++)
            {
              vec4 n = texture(tex, vec3((atlasOffset.x + i + uv.x * imageSize.x) / atlasWidth, (atlasOffset.y + j + uv.y * imageSize.y) / atlasHeight, atlasOffset.z));
              if (n.w > 0. && (n.x > 0. || n.y > 0. || n.z > 0.))
                c = hoverColor;
            }
    }
	frag_colour = c;//vec4(c.r, c.g, c.b, 0.4 * c.a);
}

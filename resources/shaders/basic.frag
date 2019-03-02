#version 330
precision mediump float;

in vec2 uv;
flat in vec2 imageSize;
flat in vec4 hoverColor;
flat in vec3 atlasOffset;

out vec4 frag_colour;
uniform sampler2D tex[8]; // NOTE: The size of this array has a large impact on performance...
uniform vec2 atlasSize;

void main() {
    vec4 c = texture(tex[int(atlasOffset.z)], (atlasOffset.xy + uv * imageSize) / atlasSize);

    if (c.w == 0. && hoverColor.a > 0.)
    {
      for (float i= -1.; i <= 1.; i++)
        for (float j= -1.; j <= 1.; j++)
        {
          vec2 offset = vec2(i, j);
          vec4 n = texture(tex[int(atlasOffset.z)], (atlasOffset.xy + offset + uv * imageSize) / atlasSize);
          if (n.w > 0. && (n.x > 0. || n.y > 0. || n.z > 0.))
            c = hoverColor / 255;
        }
    }
    frag_colour = c; //vec4(c.rgb, 0.4 * c.a);
}

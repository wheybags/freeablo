#version 330

in vec2 uv;
flat in vec2 imageSize;
flat in vec4 hoverColor;
flat in vec2 atlasOffset;
flat in float f_zValue;

layout(std140) uniform fragmentUniforms
{
    vec2 atlasSize;

    vec2 pad1;
};

uniform sampler2D tex;

out vec4 frag_colour;

void main()
{
    vec4 c = texture(tex, (atlasOffset.xy + uv * imageSize) / atlasSize);

    if (c.w == 0. && hoverColor.a > 0.)
    {
        for (float i= -1.; i <= 1.; i++)
        {
            for (float j= -1.; j <= 1.; j++)
            {
                vec2 offset = vec2(i, j);
                vec4 n = texture(tex, (atlasOffset.xy + offset + uv * imageSize) / atlasSize);
                if (n.w > 0. && (n.x > 0. || n.y > 0. || n.z > 0.))
                    c = hoverColor;
            }
        }
    }

    frag_colour = c;
    gl_FragDepth = mix(1.0, f_zValue, frag_colour.a);
}

#version 330

in vec2 f_uvNorm;
flat in float f_zValue;
flat in vec2 f_spriteSizeInPixels;
flat in vec4 f_hoverColor;
flat in vec2 f_atlasOffsetInPixels;

layout(std140) uniform fragmentUniforms
{
    vec2 atlasSizeInPixels;

    vec2 pad1;
};

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, (f_atlasOffsetInPixels.xy + f_uvNorm * f_spriteSizeInPixels) / atlasSizeInPixels);

    if (color.w == 0 && f_hoverColor.a > 0)
    {
        for (float y = -1; y <= 1; y++)
        {
            for (float x = -1; x <= 1; x++)
            {
                vec2 offset = vec2(x, y);
                vec4 n = texture(tex, (f_atlasOffsetInPixels.xy + offset + f_uvNorm * f_spriteSizeInPixels) / atlasSizeInPixels);

                if (n.a > 0 && (n.r > 0 || n.g > 0 || n.b > 0))
                    color = f_hoverColor;
            }
        }
    }

    gl_FragColor = color;
    gl_FragDepth = mix(1.0, f_zValue, gl_FragColor.a);
}

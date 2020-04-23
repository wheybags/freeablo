#version 330

// From base vertices
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 v_uv;

// Per-instance
layout(location = 2) in float v_zValue;
layout(location = 3) in vec2 v_spriteSizeInPixels;
layout(location = 4) in vec2 v_destinationInPixels;
layout(location = 5) in vec4 v_hoverColor;
layout(location = 6) in vec2 v_atlasOffsetInPixels;

layout(std140) uniform vertexUniforms
{
    vec2 screenSizeInPixels;

    vec2 pad2;
};

out vec2 f_uvNorm;
flat out float f_zValue;
flat out vec2 f_spriteSizeInPixels;
flat out vec4 f_hoverColor;
flat out vec2 f_atlasOffsetInPixels;

void main()
{
    f_uvNorm = v_uv;
    f_zValue = v_zValue;
    f_spriteSizeInPixels = v_spriteSizeInPixels;
    f_hoverColor = v_hoverColor;
    f_atlasOffsetInPixels = v_atlasOffsetInPixels;

    vec2 destNorm = v_destinationInPixels / screenSizeInPixels;
    vec2 imageSizeNorm = v_spriteSizeInPixels / screenSizeInPixels;
    vec2 posNorm = destNorm + vertex_position * imageSizeNorm;

    vec2 posNDC = posNorm * 2 - 1;
    posNDC.y = -posNDC.y;

    gl_Position = vec4(posNDC, 0, 1);
}

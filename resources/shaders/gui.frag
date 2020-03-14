#version 330

uniform sampler2DArray Texture;
uniform vec4 hoverColor;
uniform vec2 imageSize;
uniform vec3 atlasOffset;
uniform vec2 atlasSize;
uniform int checkerboarded;

in vec2 Frag_UV;
in vec4 Frag_Color;

out vec4 Out_Color;

void main(){
    vec4 c = Frag_Color * texture(Texture, vec3((atlasOffset.xy + Frag_UV * imageSize) / atlasSize, int(atlasOffset.z)));
    if (c.w == 0. && hoverColor.a > 0.)
    {
        for (float i= -1.; i <= 1.; i++)
        for (float j= -1.; j <= 1.; j++)
        {
            vec2 offset = vec2(i, j);
            vec4 n = texture(Texture, vec3((atlasOffset.xy + offset + Frag_UV * imageSize) / atlasSize, int(atlasOffset.z)));
            if (n.w > 0. && (n.x > 0. || n.y > 0. || n.z > 0.))
            c = hoverColor;
        }
    }
    if (checkerboarded != 0)
    {
        float vx = floor(Frag_UV.st.x * imageSize.x);
        float vy = floor(Frag_UV.st.y * imageSize.y);
        if (mod(vx + vy, 2.) == 1.)
        c.w = 0.;
    }
    Out_Color = c;
}
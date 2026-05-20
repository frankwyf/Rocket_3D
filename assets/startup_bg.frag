#version 430

in vec2 vTexCoord;

layout(location = 0) uniform sampler2D uBackgroundTex;
layout(location = 0) out vec4 oColor;

void main()
{
    vec3 color = texture(uBackgroundTex, vTexCoord).rgb;
    oColor = vec4(color, 1.0);
}

#version 430

in vec2 vTexCoord;
in vec4 vColor;

layout(location = 1) uniform sampler2D uFontTex;
layout(location = 0) out vec4 oColor;

void main()
{
    float alpha = texture(uFontTex, vTexCoord).r;
    oColor = vec4(vColor.rgb, vColor.a * alpha);
}

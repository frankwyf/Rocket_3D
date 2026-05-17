#version 430

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec2 iTexCoord;
layout(location = 2) in vec4 iColor;

layout(location = 0) uniform vec2 uScreenSize;

out vec2 vTexCoord;
out vec4 vColor;

void main()
{
    vec2 ndc;
    ndc.x = (iPosition.x / uScreenSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (iPosition.y / uScreenSize.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    vTexCoord = iTexCoord;
    vColor = iColor;
}

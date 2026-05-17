#version 430

in vec3 v2fColor;

layout( location = 0 ) uniform vec3 uBaseColor;

layout( location = 0 ) out vec3 oColor;

void main()
{
    oColor = uBaseColor * v2fColor;
}
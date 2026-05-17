#version 430

layout( location = 0 ) in vec2 iPosition; 
layout( location = 1 ) in vec3 iColor; 

out vec3 v2fColor; // v2f = vertex to fragment
 
void main() 
{
    v2fColor = iColor;
    gl_Position = vec4( iPosition.xy, 0.0, 1.0 ); 
}

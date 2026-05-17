#version 430

layout( location = 0 ) in vec3 pos; 
layout( location = 0 ) uniform mat4 projCamera_world;
layout( location = 1 ) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;
layout( location = 1 ) uniform mat3 normalMat;
layout( location = 11 ) uniform mat4 trans;

out vec3 v2fColor;
out vec3 v2fNormal;
out vec3 v2fPosition;

void main()
{
	v2fColor = iColor;
	v2fPosition = (trans * vec4( pos, 1.0 )).xyz;
	gl_Position = projCamera_world * vec4( pos, 1.0 );
	v2fNormal = normalize(normalMat * iNormal); 
}
#version 430

// with texture
in vec2 v2fTexCoord;
in vec3 v2fColor;
in vec3 v2fNormal; 
in vec3 v2Position;
layout(binding = 0) uniform sampler2D uTexture;

layout(location = 0) out vec4 oColor; 
layout(location = 2) uniform vec3 uLightDir; 
layout(location = 3) uniform vec3 uLightDiffuse; 
layout(location = 4) uniform vec3 uSceneAmbient;


void main()
{
    vec3 normal = normalize(v2fNormal); 
    float nDotL = max(0.0, dot(normal, uLightDir)); 
    vec3 diffuse = nDotL * uLightDiffuse; 
    vec3 ambient = uSceneAmbient;

    //spectral
    //vec3 viewDir = normalize(-v2Position);
    //vec3 reflectDir = reflect(-uLightDir, normal);
    //float spec = pow(max(dot(viewDir,reflectDir), 0.0), 250.f);
    //vec3 spectral = vec3(0.5f,0.5f,0.5f) * vec3(1.f,1.f,1.f) * spec;

    vec4 texColor = texture(uTexture, v2fTexCoord); 

    vec3 color = (ambient + diffuse) * texColor.rgb;

    oColor = vec4(color * v2fColor, 1.0);

}


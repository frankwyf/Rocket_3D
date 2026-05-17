#version 430

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPosition;

layout( location = 2 ) uniform vec3 lightDot_1Pos;      
layout( location = 3 ) uniform vec3 ambientColor;  
layout( location = 4 ) uniform vec3 lightDot_1Col;    
layout( location = 5 ) uniform vec3 eyePos;       
layout( location = 6 ) uniform float shininess;
layout( location = 7 ) uniform vec3 lightDot_2Pos;
layout( location = 8 ) uniform vec3 lightDot_2Col;  
layout( location = 9 ) uniform vec3 lightDot_3Pos;
layout( location = 10 ) uniform vec3 lightDot_3Col;  

layout( location = 0 ) out vec4 oColor;

void main() {
    
    vec3 norm = normalize(v2fNormal);
    vec3 lightDir = normalize(lightDot_1Pos - v2fPosition);
    vec3 viewDir = normalize(eyePos - v2fPosition);

    vec3 ambient = ambientColor * lightDot_1Col;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightDot_1Col;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = spec * lightDot_1Col;

    float distance = length(lightDot_1Pos - v2fPosition);
    float distanceSquared = distance * distance;

    diffuse /= distanceSquared;
    specular /= distanceSquared;

    vec3 lightDir2 = normalize(lightDot_2Pos - v2fPosition);
    vec3 viewDir2 = normalize(eyePos - v2fPosition);

    vec3 ambient2 = ambientColor * lightDot_2Col;

    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * lightDot_2Col;

    vec3 halfwayDir2 = normalize(lightDir2 + viewDir);
    float spec2 = pow(max(dot(norm, halfwayDir2), 0.0), shininess);
    vec3 specular2 = spec2 * lightDot_2Col;

    float distance2 = length(lightDot_2Pos - v2fPosition);
    float distanceSquared2 = distance2 * distance2;

    diffuse2 /= distanceSquared2;
    specular2 /= distanceSquared2;


    vec3 lightDir3 = normalize(lightDot_3Pos - v2fPosition);
    vec3 viewDir3 = normalize(eyePos - v2fPosition);

    vec3 ambient3 = ambientColor * lightDot_3Col;

    float diff3 = max(dot(norm, lightDir3), 0.0);
    vec3 diffuse3 = diff3 * lightDot_3Col;

    vec3 halfwayDir3 = normalize(lightDir3 + viewDir);
    float spec3 = pow(max(dot(norm, halfwayDir3), 0.0), shininess);
    vec3 specular3 = spec3 * lightDot_3Col;

    float distance3 = length(lightDot_3Pos - v2fPosition);
    float distanceSquared3 = distance3 * distance3;

    diffuse3 /= distanceSquared3;
    specular3 /= distanceSquared3;

    // lighted model, used in and after Task 1.8 is impelmented
    vec3 tColor = (ambient + diffuse + specular + ambient2 + diffuse2 + specular2 + ambient3 + diffuse3 + specular3) * v2fColor;
   
   // No point lighting
   //vec3 tColor = (ambient + diffuse + specular) * v2fColor;
  
  oColor = vec4(tColor, 1.0);

}
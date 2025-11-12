#version 120

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

void main()
{
    vTexCoord = gl_MultiTexCoord0.xy;
    vNormal = normalMatrix * gl_Normal;
    vec4 worldPosition = modelMatrix * gl_Vertex;
    vPosition = worldPosition.xyz;
    gl_Position = projMatrix * viewMatrix * worldPosition;
}

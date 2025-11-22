#version 120

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

void main()
{
    vNormal = vec3(normalMatrix * vec4(gl_Normal, 0.0));
    vPosition = vec3(modelMatrix * gl_Vertex);
    vTexCoord = gl_MultiTexCoord0.xy;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * gl_Vertex;
}

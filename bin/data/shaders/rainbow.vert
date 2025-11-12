#version 120

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

varying vec2 vTexCoord;

void main()
{
    vTexCoord = gl_MultiTexCoord0.xy;
    gl_Position = projMatrix * viewMatrix * modelMatrix * gl_Vertex;
}
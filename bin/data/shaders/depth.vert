#version 120

uniform mat4 modelMatrix;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjMatrix;

void main()
{
    gl_Position = lightProjMatrix * lightViewMatrix * modelMatrix * gl_Vertex;
}

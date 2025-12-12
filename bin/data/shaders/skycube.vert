#version 120

attribute vec4 position;

varying vec3 v_texCoord;

uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;

void main()
{
    gl_Position = position;

    vec4 worldPos = inverseProjectionMatrix * position;
    worldPos = inverseViewMatrix * worldPos;

    v_texCoord = worldPos.xyz;
}

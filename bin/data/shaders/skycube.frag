#version 120

varying vec3 v_texCoord;

uniform samplerCube cubeMap;

void main()
{
    gl_FragColor = textureCube(cubeMap, v_texCoord);
}

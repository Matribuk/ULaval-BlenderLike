#version 120

uniform vec3 cameraPosition;
uniform samplerCube envMap;

varying vec3 vNormal;
varying vec3 vPosition;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 I = normalize(vPosition - cameraPosition);
    vec3 R = reflect(I, N);

    vec3 reflectionColor = textureCube(envMap, R).rgb;
    gl_FragColor = vec4(reflectionColor, 1.0);
}
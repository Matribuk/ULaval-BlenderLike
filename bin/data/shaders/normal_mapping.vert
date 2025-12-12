#version 120

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vTangent;
varying vec3 vBitangent;

void main()
{
    vTexCoord = gl_MultiTexCoord0.xy;
    vNormal = normalize(normalMatrix * gl_Normal);

    vec4 worldPosition = modelMatrix * gl_Vertex;
    vPosition = worldPosition.xyz;

    vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));

    vec3 tangent = (length(c1) > length(c2)) ? c1 : c2;

    vTangent = normalize(normalMatrix * tangent);
    vBitangent = normalize(cross(vNormal, vTangent));

    gl_Position = projMatrix * viewMatrix * worldPosition;
}

#version 410

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

in vec4 position;
in vec3 normal;
in vec2 texcoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main() {
    vPosition = vec3(modelViewMatrix * position);
    vNormal = normalize(mat3(normalMatrix) * normal);
    vTexCoord = texcoord;

    gl_Position = modelViewProjectionMatrix * position;
}

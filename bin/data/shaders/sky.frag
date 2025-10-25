#version 330 core
out vec4 FragColor;

uniform vec3 topColor;
uniform vec3 bottomColor;
uniform vec2 resolution;

in vec2 vScreenPos;

void main() {
    float t = (vScreenPos.y + 1.0) * 0.5;

    vec3 color = mix(bottomColor, topColor, t);
    FragColor = vec4(color, 1.0);
}
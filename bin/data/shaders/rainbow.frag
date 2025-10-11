#version 150

uniform sampler2D tex0;
uniform vec4 color;
uniform float uTime;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    float texMask = texture(tex0, vTexCoord).r;
    float anim = sin(uTime * 1.5) * 0.5 + 0.5;
    float t = mix(texMask, anim, 0.5);
    vec3 cyan = vec3(0.0, 1.0, 1.0);
    vec3 finalColor = mix(color.rgb, cyan, t);
    fragColor = vec4(finalColor, color.a);
}
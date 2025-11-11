#version 120

uniform sampler2D tex0;
uniform vec4 color;
uniform float uTime;

varying vec2 vTexCoord;

void main() {
    vec4 texColor = texture2D(tex0, vTexCoord);

    float hue = uTime * 0.5;
    vec3 rainbow = vec3(
        sin(hue * 6.28318) * 0.5 + 0.5,
        sin(hue * 6.28318 + 2.09439) * 0.5 + 0.5,
        sin(hue * 6.28318 + 4.18879) * 0.5 + 0.5
    );

    float texLuminance = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));

    if (texLuminance > 0.95) {
        gl_FragColor = vec4(color.rgb * rainbow, color.a);
    } else {
        vec3 finalColor = texColor.rgb + rainbow * 0.3;
        gl_FragColor = vec4(finalColor, color.a);
    }
}
#version 120

uniform vec4 color;
uniform float uTime;
uniform sampler2D tex0;
uniform bool hasTexture;

varying vec2 vTexCoord;
varying vec3 vPosition;

// Convert RGB to HSL
vec3 rgb2hsl(vec3 rgb) {
    float maxC = max(max(rgb.r, rgb.g), rgb.b);
    float minC = min(min(rgb.r, rgb.g), rgb.b);
    float delta = maxC - minC;

    float h = 0.0;
    float s = 0.0;
    float l = (maxC + minC) / 2.0;

    if (delta > 0.0001) {
        s = (l < 0.5) ? (delta / (maxC + minC)) : (delta / (2.0 - maxC - minC));

        if (rgb.r >= maxC) {
            h = (rgb.g - rgb.b) / delta;
        } else if (rgb.g >= maxC) {
            h = 2.0 + (rgb.b - rgb.r) / delta;
        } else {
            h = 4.0 + (rgb.r - rgb.g) / delta;
        }

        h /= 6.0;
        if (h < 0.0) h += 1.0;
    }

    return vec3(h, s, l);
}

// Convert HSL to RGB
vec3 hsl2rgb(vec3 hsl) {
    float h = hsl.x;
    float s = hsl.y;
    float l = hsl.z;

    float c = (1.0 - abs(2.0 * l - 1.0)) * s;
    float x = c * (1.0 - abs(mod(h * 6.0, 2.0) - 1.0));
    float m = l - c / 2.0;

    vec3 rgb = vec3(0.0);

    if (h < 1.0/6.0) {
        rgb = vec3(c, x, 0.0);
    } else if (h < 2.0/6.0) {
        rgb = vec3(x, c, 0.0);
    } else if (h < 3.0/6.0) {
        rgb = vec3(0.0, c, x);
    } else if (h < 4.0/6.0) {
        rgb = vec3(0.0, x, c);
    } else if (h < 5.0/6.0) {
        rgb = vec3(x, 0.0, c);
    } else {
        rgb = vec3(c, 0.0, x);
    }

    return rgb + m;
}

void main() {
    // Animated rainbow hue depending on time + vertical position
    float rainbowHue = fract(uTime * 0.1 + vPosition.y * 0.1);
    vec3 rainbowColor = hsl2rgb(vec3(rainbowHue, 0.8, 0.5));

    vec3 finalColor = rainbowColor;

    // Combine with texture if present
    if (hasTexture) {
        vec3 texColor = texture2D(tex0, vTexCoord).rgb;
        finalColor *= texColor;
    }

    gl_FragColor = vec4(finalColor, 1.0);
}

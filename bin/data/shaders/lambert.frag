#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;
uniform sampler2D tex0;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPosition - vPosition);

    float diff = max(dot(N, L), 0.0);

    vec3 ambient = ambientColor;
    vec3 diffuse = lightColor * diff * lightIntensity;

    vec3 lighting = ambient + diffuse;

    vec4 texColor = texture2D(tex0, vTexCoord);
    float brightness = dot(texColor.rgb, vec3(0.333));
    vec3 baseColor = (brightness < 0.01) ? vec3(1.0) : texColor.rgb;

    vec3 finalColor = lighting * baseColor;

    gl_FragColor = vec4(finalColor, 1.0);
}

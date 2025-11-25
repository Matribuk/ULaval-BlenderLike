#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;
uniform float shininess;
uniform sampler2D tex0;
uniform sampler2D normalMap;
uniform float normalStrength;
uniform bool useNormalMap;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vTangent;
varying vec3 vBitangent;

void main()
{
    vec3 N;

    if (useNormalMap) {
        vec3 tangentNormal = texture2D(normalMap, vTexCoord).rgb * 2.0 - 1.0;

        tangentNormal.xy *= normalStrength;
        tangentNormal = normalize(tangentNormal);

        mat3 TBN = mat3(vTangent, vBitangent, vNormal);
        N = normalize(TBN * tangentNormal);
    } else {
        N = normalize(vNormal);
    }

    vec3 L = normalize(lightPosition - vPosition);
    vec3 V = normalize(cameraPosition - vPosition);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), shininess);

    vec3 ambient = max(ambientColor, vec3(0.5));
    vec3 diffuse = lightColor * diff * lightIntensity;
    vec3 specular = lightColor * spec * lightIntensity * 0.5;

    vec4 texColor = texture2D(tex0, vTexCoord);
    float brightness = dot(texColor.rgb, vec3(0.333));
    vec3 baseColor = (brightness < 0.01) ? vec3(0.8) : texColor.rgb;

    vec3 ambientContribution = ambient * baseColor;
    vec3 diffuseContribution = diffuse * baseColor;
    vec3 specularContribution = specular;

    vec3 finalColor = ambientContribution + diffuseContribution + specularContribution;

    gl_FragColor = vec4(finalColor, 1.0);
}

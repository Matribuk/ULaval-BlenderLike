#version 120

uniform vec4 color;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;
uniform float shininess;
uniform sampler2D tex0;
uniform bool hasTexture;
uniform sampler2D normalMap;
uniform float normalStrength;
uniform int useNormalMap;
uniform vec3 ambientReflection;
uniform vec3 diffuseReflection;
uniform vec3 specularReflection;
uniform vec3 emissiveReflection;

#define MAX_LIGHTS 8
uniform int numLights;
uniform int lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
uniform float lightSpotAngles[MAX_LIGHTS];
uniform float lightAttenuations[MAX_LIGHTS];

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;
varying vec3 vTangent;
varying vec3 vBitangent;

void main()
{
    vec3 N;

    if (useNormalMap == 1) {
        vec3 tangentNormal = texture2D(normalMap, vTexCoord).rgb * 2.0 - 1.0;
        tangentNormal.xy *= normalStrength;
        tangentNormal = normalize(tangentNormal);

        mat3 TBN = mat3(normalize(vTangent), normalize(vBitangent), normalize(vNormal));
        N = normalize(TBN * tangentNormal);
    } else {
        N = normalize(vNormal);
    }

    vec3 V = normalize(cameraPosition - vPosition);

    vec3 baseColor;
    if (hasTexture)
        baseColor = texture2D(tex0, vTexCoord).rgb;
    else
        baseColor = color.rgb;

    vec3 ambient = ambientColor * ambientReflection * baseColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < 8; i++) {
        if (i >= numLights) continue;

        vec3 L;
        float attenuation = 1.0;
        int lightType = lightTypes[i];

        if (lightType == 0) {
            ambient += lightColors[i] * lightIntensities[i] * ambientReflection * baseColor;
        } else {
            if (lightType == 1) {
                L = normalize(-lightDirections[i]);
            } else if (lightType == 2) {
                vec3 lightVec = lightPositions[i] - vPosition;
                float distance = length(lightVec);
                L = normalize(lightVec);
                attenuation = 1.0 / (1.0 + lightAttenuations[i] * distance * distance);
            } else if (lightType == 3) {
                vec3 lightVec = lightPositions[i] - vPosition;
                float distance = length(lightVec);
                L = normalize(lightVec);

                float theta = dot(L, normalize(-lightDirections[i]));
                float cutoff = cos(radians(lightSpotAngles[i]));

                if (theta > cutoff) {
                    float spotEffect = pow(theta, 2.0);
                    attenuation = spotEffect / (1.0 + lightAttenuations[i] * distance * distance);
                } else {
                    attenuation = 0.0;
                }
            }

            float diff = max(dot(N, L), 0.0);
            vec3 R = reflect(-L, N);
            float spec = pow(max(dot(R, V), 0.0), shininess);

            diffuse += lightColors[i] * diff * lightIntensities[i] * attenuation * diffuseReflection * baseColor;
            specular += lightColors[i] * spec * lightIntensities[i] * attenuation * specularReflection;
        }
    }

    vec3 emissive = emissiveReflection;
    vec3 finalColor = ambient + diffuse + specular + emissive;

    gl_FragColor = vec4(finalColor, 1.0);
}

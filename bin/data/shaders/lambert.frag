#version 120

uniform vec4 color;
uniform vec3 ambientColor;

uniform sampler2D tex0;
uniform bool hasTexture;

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

void main()
{
    vec3 N = normalize(vNormal);

    vec3 baseColor = hasTexture ? texture2D(tex0, vTexCoord).rgb : color.rgb;
    vec3 ambient = ambientColor * ambientReflection * baseColor;
    vec3 diffuse = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i >= numLights) break;

        int type = lightTypes[i];
        vec3 L = vec3(0.0);
        float attenuation = 1.0;

        if (type == 0) {
            ambient += lightColors[i] * lightIntensities[i] *
                       ambientReflection * baseColor;
            continue;
        }

        if (type == 1) L = normalize(-lightDirections[i]);

        else if (type == 2) {
            vec3 lightVec = lightPositions[i] - vPosition;
            float dist = length(lightVec);
            L = normalize(lightVec);
            attenuation = 1.0 / (1.0 + lightAttenuations[i] * dist * dist);
        }

        else if (type == 3) {
            vec3 lightVec = lightPositions[i] - vPosition;
            float dist = length(lightVec);
            L = normalize(lightVec);

            float theta = dot(L, normalize(-lightDirections[i]));
            float cutoff = cos(radians(lightSpotAngles[i]));

            if (theta > cutoff) {
                float spotEffect = pow(theta, 2.0);
                attenuation = spotEffect / (1.0 + lightAttenuations[i] * dist * dist);
            } else
                attenuation = 0.0;
        }

        float diff = max(dot(N, L), 0.0);

        diffuse += lightColors[i] * diff * lightIntensities[i] *
                   attenuation * diffuseReflection * baseColor;
    }

    vec3 emissive = emissiveReflection;
    vec3 finalColor = ambient + diffuse + emissive;

    gl_FragColor = vec4(finalColor, 1.0);
}

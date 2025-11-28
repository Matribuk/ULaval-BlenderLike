#version 120

uniform vec4 color;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;
uniform float shininess;

uniform sampler2D tex0;
uniform bool hasTexture;
uniform samplerCube envMap;

uniform vec3 ambientReflection;
uniform vec3 diffuseReflection;
uniform vec3 specularReflection;
uniform vec3 emissiveReflection;

// Reflectivity control
uniform float reflectivity;
uniform vec3 reflectionTint;

#define MAX_LIGHTS 8
uniform int numLights;
uniform int lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
uniform float lightSpotAngles[MAX_LIGHTS];
uniform float lightAttenuations[MAX_LIGHTS];

// Shadow mapping uniforms
uniform int numShadowMaps;
uniform sampler2D shadowMaps[MAX_LIGHTS];
uniform mat4 lightSpaceMatrices[MAX_LIGHTS];
uniform int shadowLightIndices[MAX_LIGHTS];
uniform float shadowBias;
uniform int usePCF;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

float calculateShadow(int shadowMapIndex, vec3 fragPos)
{
    if (shadowMapIndex >= numShadowMaps) return 1.0;

    // Transform fragment position to light space
    vec4 fragPosLightSpace = lightSpaceMatrices[shadowMapIndex] * vec4(fragPos, 1.0);

    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Outside shadow map bounds
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) {
        return 1.0;
    }

    // Get closest depth from shadow map
    float closestDepth = texture2D(shadowMaps[shadowMapIndex], projCoords.xy).r;
    float currentDepth = projCoords.z;

    // PCF (Percentage Closer Filtering) for soft shadows
    if (usePCF == 1) {
        float shadow = 0.0;
        vec2 texelSize = vec2(1.0) / vec2(2048.0);

        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth = texture2D(shadowMaps[shadowMapIndex],
                    projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - shadowBias > pcfDepth ? 0.0 : 1.0;
            }
        }
        shadow /= 9.0;
        return shadow;
    } else {
        return currentDepth - shadowBias > closestDepth ? 0.0 : 1.0;
    }
}

void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(cameraPosition - vPosition);

    // Base color from texture or uniform
    vec3 baseColor = hasTexture ? texture2D(tex0, vTexCoord).rgb : color.rgb;

    // === Phong Lighting ===
    vec3 ambient = ambientColor * ambientReflection * baseColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i >= numLights) break;

        int lightType = lightTypes[i];
        vec3 L = vec3(0.0);
        float attenuation = 1.0;
        float shadow = 1.0;

        // Ambient light doesn't cast shadows
        if (lightType == 0) {
            ambient += lightColors[i] * lightIntensities[i] * ambientReflection * baseColor;
            continue;
        }

        // Calculate shadow for this light
        for (int s = 0; s < MAX_LIGHTS; s++) {
            if (s >= numShadowMaps) break;
            if (shadowLightIndices[s] == i) {
                shadow = calculateShadow(s, vPosition);
                break;
            }
        }

        // Directional light
        if (lightType == 1) {
            L = normalize(-lightDirections[i]);
        }
        // Point light
        else if (lightType == 2) {
            vec3 lightVec = lightPositions[i] - vPosition;
            float dist = length(lightVec);
            L = normalize(lightVec);
            attenuation = 1.0 / (1.0 + lightAttenuations[i] * dist * dist);
        }
        // Spot light
        else if (lightType == 3) {
            vec3 lightVec = lightPositions[i] - vPosition;
            float dist = length(lightVec);
            L = normalize(lightVec);

            float theta = dot(L, normalize(-lightDirections[i]));
            float cutoff = cos(radians(lightSpotAngles[i]));

            if (theta > cutoff) {
                float spotEffect = pow(theta, 2.0);
                attenuation = spotEffect / (1.0 + lightAttenuations[i] * dist * dist);
            } else {
                attenuation = 0.0;
            }
        }

        // Diffuse
        float diff = max(dot(N, L), 0.0);

        // Specular (Blinn-Phong)
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), shininess);

        // Apply shadow
        diffuse += lightColors[i] * diff * lightIntensities[i] * attenuation * diffuseReflection * baseColor * shadow;
        specular += lightColors[i] * spec * lightIntensities[i] * attenuation * specularReflection * shadow;
    }

    vec3 emissive = emissiveReflection;
    vec3 phongColor = ambient + diffuse + specular + emissive;

    // === Environment Reflection ===
    vec3 I = normalize(vPosition - cameraPosition);
    vec3 R = reflect(I, N);
    vec3 reflectionColor = textureCube(envMap, R).rgb * reflectionTint;

    // === Mix Phong and Reflection ===
    // reflectivity = 0.0 -> full Phong lighting
    // reflectivity = 1.0 -> full reflection
    vec3 finalColor = mix(phongColor, reflectionColor, reflectivity);

    gl_FragColor = vec4(finalColor, 1.0);
}

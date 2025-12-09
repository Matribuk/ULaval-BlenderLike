#version 120

#define MAX_LIGHTS 16
#define PI 3.14159265359

uniform int numLights;
uniform int lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
uniform float lightSpotAngles[MAX_LIGHTS];
uniform float lightAttenuations[MAX_LIGHTS];

uniform vec3 cameraPosition;
uniform vec3 baseColor;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform sampler2D tex0;
uniform bool hasTexture;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculateLight(int i, vec3 N, vec3 V, vec3 F0, vec3 albedo) {
    vec3 L;
    float attenuation = 1.0;

    if (lightTypes[i] == 0) {
        return lightColors[i] * lightIntensities[i] * albedo * ao;
    }
    else if (lightTypes[i] == 1) {
        L = normalize(-lightDirections[i]);
    }
    else if (lightTypes[i] == 2) {
        L = normalize(lightPositions[i] - vPosition);
        float distance = length(lightPositions[i] - vPosition);
        attenuation = 1.0 / (1.0 + lightAttenuations[i] * distance * distance);
    }
    else if (lightTypes[i] == 3) {
        L = normalize(lightPositions[i] - vPosition);
        float distance = length(lightPositions[i] - vPosition);
        attenuation = 1.0 / (1.0 + lightAttenuations[i] * distance * distance);

        float theta = dot(L, normalize(-lightDirections[i]));
        float epsilon = 0.1;
        float cutoff = cos(radians(lightSpotAngles[i]));
        float intensity = clamp((theta - cutoff) / epsilon, 0.0, 1.0);
        attenuation *= intensity;
    }

    if (lightTypes[i] == 0) {
        return vec3(0.0);
    }

    vec3 H = normalize(V + L);
    vec3 radiance = lightColors[i] * lightIntensities[i] * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(cameraPosition - vPosition);

    vec3 albedo = baseColor;
    if (hasTexture) {
        albedo *= texture2D(tex0, vTexCoord).rgb;
    }

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= numLights) break;
        Lo += calculateLight(i, N, V, F0, albedo);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    gl_FragColor = vec4(color, 1.0);
}

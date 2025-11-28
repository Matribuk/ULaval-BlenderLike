#version 120

uniform vec4 color;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;
uniform float shininess;

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

uniform int numShadowMaps;
uniform sampler2D shadowMaps[MAX_LIGHTS];
uniform mat4 lightSpaceMatrices[MAX_LIGHTS];
uniform int shadowLightIndices[MAX_LIGHTS];
uniform float shadowBias;
uniform int usePCF;

#define MAX_PRIMITIVES 32
uniform int raytracingEnabled;
uniform int raytracedShadows;
uniform int raytracedReflections;
uniform int maxRayBounces;

uniform int numPrimitives;
uniform int primitiveTypes[MAX_PRIMITIVES];
uniform vec3 primitivePositions[MAX_PRIMITIVES];
uniform vec3 primitiveParam1[MAX_PRIMITIVES];
uniform vec3 primitiveParam2[MAX_PRIMITIVES];
uniform vec4 primitiveColors[MAX_PRIMITIVES];
uniform float primitiveReflectivities[MAX_PRIMITIVES];

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct HitInfo {
    bool hit;
    float t;
    vec3 position;
    vec3 normal;
    vec4 color;
    float reflectivity;
};

bool intersectSphere(Ray ray, vec3 center, float radius, out float t) {
    vec3 oc = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) return false;

    float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
    float t2 = (-b + sqrt(discriminant)) / (2.0 * a);

    t = t1 > 0.01 ? t1 : t2;
    return t > 0.01;
}

bool intersectBox(Ray ray, vec3 center, vec3 dimensions, out float t) {
    vec3 boxMin = center - dimensions * 0.5;
    vec3 boxMax = center + dimensions * 0.5;

    vec3 invDir = 1.0 / ray.direction;
    vec3 tMin = (boxMin - ray.origin) * invDir;
    vec3 tMax = (boxMax - ray.origin) * invDir;

    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    if (tNear > tFar || tFar < 0.01) return false;

    t = tNear > 0.01 ? tNear : tFar;
    return t > 0.01;
}

bool intersectPlane(Ray ray, vec3 center, vec3 normal, vec2 size, out float t) {
    float denom = dot(normal, ray.direction);
    if (abs(denom) < 0.0001) return false;

    t = dot(center - ray.origin, normal) / denom;
    if (t < 0.01) return false;

    vec3 hitPoint = ray.origin + ray.direction * t;
    vec3 localHit = hitPoint - center;

    float u = abs(dot(localHit, vec3(1.0, 0.0, 0.0)));
    float v = abs(dot(localHit, vec3(0.0, 0.0, 1.0)));

    return u <= size.x * 0.5 && v <= size.y * 0.5;
}

HitInfo traceRay(Ray ray, int skipPrimitive) {
    HitInfo closest;
    closest.hit = false;
    closest.t = 1000000.0;

    for (int i = 0; i < MAX_PRIMITIVES; i++) {
        if (i >= numPrimitives) break;
        if (i == skipPrimitive) continue;

        float t;
        bool hit = false;
        vec3 normal;

        if (primitiveTypes[i] == 0) {
            float radius = primitiveParam1[i].x;
            if (intersectSphere(ray, primitivePositions[i], radius, t)) {
                vec3 hitPos = ray.origin + ray.direction * t;
                normal = normalize(hitPos - primitivePositions[i]);
                hit = true;
            }
        }
        else if (primitiveTypes[i] == 1) {
            if (intersectBox(ray, primitivePositions[i], primitiveParam1[i], t)) {
                vec3 hitPos = ray.origin + ray.direction * t;
                vec3 localHit = hitPos - primitivePositions[i];
                vec3 dims = primitiveParam1[i] * 0.5;

                vec3 absLocal = abs(localHit);
                if (absLocal.x > absLocal.y && absLocal.x > absLocal.z)
                    normal = vec3(sign(localHit.x), 0.0, 0.0);
                else if (absLocal.y > absLocal.z)
                    normal = vec3(0.0, sign(localHit.y), 0.0);
                else
                    normal = vec3(0.0, 0.0, sign(localHit.z));

                hit = true;
            }
        }
        else if (primitiveTypes[i] == 2) {
            vec3 planeNormal = primitiveParam2[i];
            vec2 planeSize = primitiveParam1[i].xy;
            if (intersectPlane(ray, primitivePositions[i], planeNormal, planeSize, t)) {
                normal = planeNormal;
                hit = true;
            }
        }

        if (hit && t < closest.t) {
            closest.hit = true;
            closest.t = t;
            closest.position = ray.origin + ray.direction * t;
            closest.normal = normal;
            closest.color = primitiveColors[i];
            closest.reflectivity = primitiveReflectivities[i];
        }
    }

    return closest;
}

float calculateRaytracedShadow(vec3 fragPos, vec3 lightDir, float maxDist) {
    Ray shadowRay;
    shadowRay.origin = fragPos + vNormal * 0.1;
    shadowRay.direction = lightDir;

    HitInfo hit = traceRay(shadowRay, -1);

    if (hit.hit && hit.t < maxDist) {
        return 0.0;
    }

    return 1.0;
}

vec3 calculateRaytracedReflection(vec3 fragPos, vec3 viewDir, vec3 normal, int bounces) {
    if (bounces <= 0) return vec3(0.0);

    vec3 finalColor = vec3(0.0);
    float reflectionWeight = 1.0;

    vec3 currentOrigin = fragPos;
    vec3 currentViewDir = viewDir;
    vec3 currentNormal = normal;

    for (int bounce = 0; bounce < 3; bounce++) {
        if (bounce >= bounces) break;

        Ray reflectRay;
        reflectRay.origin = currentOrigin + currentNormal * 0.1;
        reflectRay.direction = reflect(-currentViewDir, currentNormal);

        HitInfo hit = traceRay(reflectRay, -1);

        if (!hit.hit) break;

        vec3 reflectedColor = hit.color.rgb * 0.2;

        for (int i = 0; i < MAX_LIGHTS; i++) {
            if (i >= numLights) break;

            if (lightTypes[i] == 2) {
                vec3 L = normalize(lightPositions[i] - hit.position);
                float diff = max(dot(hit.normal, L), 0.0);
                reflectedColor += hit.color.rgb * diff * lightColors[i] * lightIntensities[i];
            }
        }

        finalColor += reflectedColor * reflectionWeight;

        if (hit.reflectivity > 0.0) {
            reflectionWeight *= hit.reflectivity;
            currentOrigin = hit.position;
            currentViewDir = normalize(reflectRay.origin - hit.position);
            currentNormal = hit.normal;
        } else {
            break;
        }
    }

    return finalColor;
}

float calculateShadow(int shadowMapIndex, vec3 fragPos) {
    if (shadowMapIndex >= numShadowMaps) return 1.0;

    vec4 fragPosLightSpace = lightSpaceMatrices[shadowMapIndex] * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) {
        return 1.0;
    }

    float closestDepth = texture2D(shadowMaps[shadowMapIndex], projCoords.xy).r;
    float currentDepth = projCoords.z;

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
        return shadow / 9.0;
    }

    return currentDepth - shadowBias > closestDepth ? 0.0 : 1.0;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(cameraPosition - vPosition);

    vec3 baseColor = hasTexture ? texture2D(tex0, vTexCoord).rgb : color.rgb;
    vec3 ambient = ambientColor * ambientReflection * baseColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i >= numLights) break;

        int lightType = lightTypes[i];
        vec3 L = vec3(0.0);
        float attenuation = 1.0;
        float shadow = 1.0;

        if (lightType == 0) {
            ambient += lightColors[i] * lightIntensities[i] * ambientReflection * baseColor;
            continue;
        }

        if (raytracingEnabled == 1 && raytracedShadows == 1) {
            if (lightType == 1) {
                shadow = calculateRaytracedShadow(vPosition, normalize(-lightDirections[i]), 1000.0);
            } else if (lightType == 2) {
                vec3 lightDir = normalize(lightPositions[i] - vPosition);
                float distToLight = length(lightPositions[i] - vPosition);
                shadow = calculateRaytracedShadow(vPosition, lightDir, distToLight);
            }
        } else {
            for (int s = 0; s < MAX_LIGHTS; s++) {
                if (s >= numShadowMaps) break;
                if (shadowLightIndices[s] == i) {
                    shadow = calculateShadow(s, vPosition);
                    break;
                }
            }
        }

        if (lightType == 1) {
            L = normalize(-lightDirections[i]);
        } else if (lightType == 2) {
            vec3 lightVec = lightPositions[i] - vPosition;
            float dist = length(lightVec);
            L = normalize(lightVec);
            attenuation = 1.0 / (1.0 + lightAttenuations[i] * dist * dist);
        } else if (lightType == 3) {
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

        float diff = max(dot(N, L), 0.0);
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), shininess);

        diffuse += lightColors[i] * diff * lightIntensities[i] * attenuation * diffuseReflection * baseColor * shadow;
        specular += lightColors[i] * spec * lightIntensities[i] * attenuation * specularReflection * shadow;
    }

    vec3 emissive = emissiveReflection;
    vec3 phongColor = ambient + diffuse + specular + emissive;

    vec3 finalColor = phongColor;
    if (raytracingEnabled == 1 && raytracedReflections == 1) {
        vec3 reflectionColor = calculateRaytracedReflection(vPosition, V, N, maxRayBounces);
        float reflectivity = 0.5;
        finalColor = mix(phongColor, reflectionColor, reflectivity);
    }

    gl_FragColor = vec4(finalColor, 1.0);
}

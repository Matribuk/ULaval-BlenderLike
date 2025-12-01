#version 120

uniform vec3 cameraPosition;
uniform samplerCube envMap;

#define MAX_PRIMITIVES 32
#define MAX_LIGHTS 8
uniform int raytracingEnabled;

uniform int numPrimitives;
uniform int primitiveTypes[MAX_PRIMITIVES];
uniform vec3 primitivePositions[MAX_PRIMITIVES];
uniform vec3 primitiveParam1[MAX_PRIMITIVES];
uniform vec3 primitiveParam2[MAX_PRIMITIVES];
uniform vec4 primitiveColors[MAX_PRIMITIVES];
uniform float primitiveReflectivities[MAX_PRIMITIVES];

uniform int numLights;
uniform int lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];

varying vec3 vNormal;
varying vec3 vPosition;

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

    t = t1 > 0.1 ? t1 : t2;
    return t > 0.1;
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

    if (tNear > tFar || tFar < 0.1) return false;

    t = tNear > 0.1 ? tNear : tFar;
    return t > 0.1;
}

bool intersectPlane(Ray ray, vec3 center, vec3 normal, vec2 size, out float t) {
    float denom = dot(normal, ray.direction);
    if (abs(denom) < 0.0001) return false;

    t = dot(center - ray.origin, normal) / denom;
    if (t < 0.1) return false;

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
        reflectRay.origin = currentOrigin + currentNormal * 0.5;
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

void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(cameraPosition - vPosition);
    vec3 I = normalize(vPosition - cameraPosition);
    vec3 R = reflect(I, N);

    vec3 reflectionColor = textureCube(envMap, R).rgb;

    if (raytracingEnabled == 1) {
        vec3 raytracedReflectionColor = calculateRaytracedReflection(vPosition, V, N, 2);
        reflectionColor = mix(reflectionColor, raytracedReflectionColor, 0.5);
    }

    gl_FragColor = vec4(reflectionColor, 1.0);
}
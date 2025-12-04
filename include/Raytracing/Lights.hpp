#pragma once

#include "Vec3.hpp"
#include "Interval.hpp"
#include "Hittable.hpp"
#include "LightSource.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

struct RtLight {
    LightType type;
    point3 position;
    Vec3 direction;
    Color colorValue;
    double intensity;
    double spotAngle;
    double attenuation;

    RtLight(const LightSource& light, const glm::mat4& transform = glm::mat4(1.0f));

    Color computeLighting(
        const point3& hitPoint,
        const Vec3& normal,
        const Vec3& viewDir,
        const Color& albedo,
        double shininess,
        const Hittable& world
    ) const;
};

struct sceneLights {
    std::vector<RtLight> lights;

    void add(const RtLight& light);
    void clear();
    Color computeTotalLighting(
        const point3& hitPoint,
        const Vec3& normal,
        const Vec3& viewDir,
        const Color& albedo,
        double shininess,
        const Hittable& world
    ) const;
};

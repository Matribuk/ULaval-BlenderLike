#include "Lights.hpp"

RtLight::RtLight(const LightSource& light, const glm::mat4& transform)
{
    this->type = light.type;

    glm::vec4 pos4 = transform * glm::vec4(light.position, 1.0f);
    this->position = point3(pos4.x, pos4.y, pos4.z);

    glm::vec4 dir4 = transform * glm::vec4(light.direction, 0.0f);
    this->direction = unitVector(Vec3(dir4.x, dir4.y, dir4.z));

    this->colorValue = Color(light.color.r, light.color.g, light.color.b);
    this->intensity = light.intensity;
    this->spotAngle = glm::radians(light.spotAngle);
    this->attenuation = light.attenuation;
}

Color RtLight::computeLighting(
    const point3& hitPoint,
    const Vec3& normal,
    const Vec3& viewDir,
    const Color& albedo,
    double shininess,
    const Hittable& world
) const {
    Color result(0, 0, 0);

    if (this->type == LightType::AMBIENT)
        result = this->colorValue * this->intensity * albedo;
    else if (this->type == LightType::DIRECTIONAL) {
        Vec3 lightDir = unitVector(-this->direction);

        Ray shadowRay(hitPoint, lightDir);
        HitRecord shadowRec;
        if (world.hit(shadowRay, Interval(0.001, INFINITY), shadowRec))
            return Color(0, 0, 0);

        double diff = std::fmax(0.0, dot(normal, lightDir));
        Color diffuse = this->colorValue * this->intensity * diff * albedo;

        Vec3 halfDir = unitVector(lightDir + viewDir);
        double spec = std::pow(std::fmax(0.0, dot(normal, halfDir)), shininess);
        Color specular = this->colorValue * this->intensity * spec;

        result = diffuse + specular;
    } else if (this->type == LightType::POINT) {
        Vec3 lightDir = this->position - hitPoint;
        double distance = lightDir.length();
        lightDir = lightDir / distance;

        Ray shadowRay(hitPoint, lightDir);
        HitRecord shadowRec;
        if (world.hit(shadowRay, Interval(0.001, distance - 0.001), shadowRec))
            return Color(0, 0, 0);

        double att = 1.0 / (1.0 + this->attenuation * distance * distance);

        double diff = std::fmax(0.0, dot(normal, lightDir));
        Color diffuse = this->colorValue * this->intensity * att * diff * albedo;

        Vec3 halfDir = unitVector(lightDir + viewDir);
        double spec = std::pow(std::fmax(0.0, dot(normal, halfDir)), shininess);
        Color specular = this->colorValue * this->intensity * att * spec;

        result = diffuse + specular;
    } else if (this->type == LightType::SPOT) {
        Vec3 lightDir = this->position - hitPoint;
        double distance = lightDir.length();
        lightDir = lightDir / distance;

        double theta = dot(lightDir, unitVector(-this->direction));
        double cutoff = std::cos(this->spotAngle);

        if (theta < cutoff) return Color(0, 0, 0);

        Ray shadowRay(hitPoint, lightDir);
        HitRecord shadowRec;
        if (world.hit(shadowRay, Interval(0.001, distance - 0.001), shadowRec))
            return Color(0, 0, 0);

        double att = 1.0 / (1.0 + this->attenuation * distance * distance);

        double epsilon = 0.1;
        double spotIntensity = (theta - cutoff) / epsilon;
        spotIntensity = std::clamp(spotIntensity, 0.0, 1.0);

        double diff = std::fmax(0.0, dot(normal, lightDir));
        Color diffuse = this->colorValue * this->intensity * att * spotIntensity * diff * albedo;

        Vec3 halfDir = unitVector(lightDir + viewDir);
        double spec = std::pow(std::fmax(0.0, dot(normal, halfDir)), shininess);
        Color specular = this->colorValue * this->intensity * att * spotIntensity * spec;

        result = diffuse + specular;
    }

    return result;
}


void sceneLights::add(const RtLight& light)
{
    this->lights.push_back(light);
}

void sceneLights::clear()
{
    this->lights.clear();
}

Color sceneLights::computeTotalLighting(
    const point3& hitPoint,
    const Vec3& normal,
    const Vec3& viewDir,
    const Color& albedo,
    double shininess,
    const Hittable& world
) const {
    Color total(0, 0, 0);

    for (const auto& light : this->lights)
        total += light.computeLighting(hitPoint, normal, viewDir, albedo, shininess, world);

    return total;
}

#include "Materials.hpp"

Lambertian::Lambertian(const Color& albedo, ofTexture* texture)
{
    this->_albedo = albedo;
    this->_texture = texture;

    if (texture && texture->isAllocated()) {
        texture->readToPixels(this->_cachedPixels);
    }
}

bool Lambertian::scatter(
    const Ray& rIn,
    const HitRecord& rec,
    Color& attenuation,
    Ray& scattered
) const {
    auto scatterDirection = rec.normal + randomUnitVector();

    if (scatterDirection.nearZero()) scatterDirection = rec.normal;

    scattered = Ray(rec.p, scatterDirection);

    if (this->_cachedPixels.isAllocated()) {
        int x = static_cast<int>(rec.u * (this->_cachedPixels.getWidth() - 1));
        int y = static_cast<int>(rec.v * (this->_cachedPixels.getHeight() - 1));
        x = std::max(0, std::min(x, static_cast<int>(this->_cachedPixels.getWidth() - 1)));
        y = std::max(0, std::min(y, static_cast<int>(this->_cachedPixels.getHeight() - 1)));

        ofColor texColor = this->_cachedPixels.getColor(x, y);

        attenuation = Color(
            texColor.r / 255.0 * this->_albedo.x(),
            texColor.g / 255.0 * this->_albedo.y(),
            texColor.b / 255.0 * this->_albedo.z()
        );
    } else {
        attenuation = this->_albedo;
    }

    return true;
}


Metal::Metal(const Color& albedo, double fuzz, ofTexture* texture)
{
    this->_albedo = albedo;
    this->_fuzz = (fuzz < 1.0) ? fuzz : 1.0;
    this->_texture = texture;

    if (texture && texture->isAllocated()) {
        texture->readToPixels(this->_cachedPixels);
    }
}

bool Metal::scatter(
    const Ray& rIn,
    const HitRecord& rec,
    Color& attenuation,
    Ray& scattered
) const {
    Vec3 reflected = reflect(rIn.direction(), rec.normal);
    reflected = unitVector(reflected) + (this->_fuzz * randomUnitVector());
    scattered = Ray(rec.p, reflected);

    if (this->_cachedPixels.isAllocated()) {
        int x = static_cast<int>(rec.u * (this->_cachedPixels.getWidth() - 1));
        int y = static_cast<int>(rec.v * (this->_cachedPixels.getHeight() - 1));
        x = std::max(0, std::min(x, static_cast<int>(this->_cachedPixels.getWidth() - 1)));
        y = std::max(0, std::min(y, static_cast<int>(this->_cachedPixels.getHeight() - 1)));

        ofColor texColor = this->_cachedPixels.getColor(x, y);

        attenuation = Color(
            texColor.r / 255.0 * this->_albedo.x(),
            texColor.g / 255.0 * this->_albedo.y(),
            texColor.b / 255.0 * this->_albedo.z()
        );
    } else {
        attenuation = this->_albedo;
    }

    return dot(scattered.direction(), rec.normal) > 0;
}

Dielectric::Dielectric(double refractionIndex)
{
    this->_refractionIndex = refractionIndex;
}

bool Dielectric::scatter(
    const Ray& rIn,
    const HitRecord& rec,
    Color& attenuation,
    Ray& scattered
) const {
    attenuation = Color(1.0, 1.0, 1.0);
    double ri = rec.frontFace ? (1.0 / this->_refractionIndex) : this->_refractionIndex;

    Vec3 unitDirection = unitVector(rIn.direction());
    double cosTheta = std::fmin(dot(-unitDirection, rec.normal), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = ri * sinTheta > 1.0;
    Vec3 direction;

    if (cannotRefract || _reflectance(cosTheta, ri) > _randomDouble())
        direction = reflect(unitDirection, rec.normal);
    else direction = refract(unitDirection, rec.normal, ri);

    scattered = Ray(rec.p, direction);
    return true;
}


double Dielectric::_randomDouble()
{
    return std::rand() / (RAND_MAX + 1.0);
}

double Dielectric::_reflectance(double cosine, double refractionIndex)
{
    auto r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
}

DiffuseLight::DiffuseLight(const Color& emit)
{
    this->_emit = emit;
}

Color DiffuseLight::emitted() const
{
    return this->_emit;
}

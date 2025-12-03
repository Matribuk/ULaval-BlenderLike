#pragma once

#include "Ray.hpp"
#include "Hittable.hpp"
#include "Interval.hpp"

#include <cmath>
#include <cstdlib>

class Materials {
    public:
        virtual ~Materials() = default;

        virtual bool scatter(
            const Ray& rIn,
            const HitRecord& rec,
            Color& attenuation,
            Ray& scattered
        ) const {
            return false;
        }
};

class Lambertian : public Materials {
    public:
        Lambertian(const Color& albedo);

        bool scatter(
            const Ray& rIn,
            const HitRecord& rec,
            Color& attenuation,
            Ray& scattered
        ) const override;

    private:
        Color _albedo;
};

class Metal : public Materials {
    public:
        Metal(const Color& albedo, double fuzz);

        bool scatter(
            const Ray& rIn,
            const HitRecord& rec,
            Color& attenuation,
            Ray& scattered
        ) const override;

    private:
        Color _albedo;
        double _fuzz;
};

class Dielectric : public Materials {
    public:
        Dielectric(double refractionIndex);

        bool scatter(
            const Ray& rIn,
            const HitRecord& rec,
            Color& attenuation,
            Ray& scattered
        ) const override;

    private:
        double _refractionIndex;

        static double _randomDouble();
        static double _reflectance(double cosine, double refractionIndex);
};

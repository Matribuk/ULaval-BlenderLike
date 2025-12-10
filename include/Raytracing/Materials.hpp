#pragma once

#include "Ray.hpp"
#include "Hittable.hpp"
#include "Interval.hpp"
#include <ofMain.h>

#include <cmath>
#include <cstdlib>

class ofTexture;

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

        virtual Color emitted() const {
            return Color(0, 0, 0);
        }
};

class Lambertian : public Materials {
    public:
        Lambertian(const Color& albedo, ofTexture* texture = nullptr);

        bool scatter(
            const Ray& rIn,
            const HitRecord& rec,
            Color& attenuation,
            Ray& scattered
        ) const override;

    private:
        Color _albedo;
        ofTexture* _texture;
};

class Metal : public Materials {
    public:
        Metal(const Color& albedo, double fuzz, ofTexture* texture = nullptr);

        bool scatter(
            const Ray& rIn,
            const HitRecord& rec,
            Color& attenuation,
            Ray& scattered
        ) const override;

    private:
        Color _albedo;
        double _fuzz;
        ofTexture* _texture;
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

class DiffuseLight : public Materials {
    public:
        DiffuseLight(const Color& emit);

        Color emitted() const override;

    private:
        Color _emit;
};

#pragma once

#include "Hittable.hpp"
#include "Materials.hpp"
#include "Lights.hpp"
#include "SkyboxSampler.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <vector>

class CameraWithLights {
    public:
        void render(const Hittable& world, std::vector<unsigned char>& pixels);

        double aspectRatio = 16.0 / 9.0;
        int imageWidth = 400;
        int samplesPerPixel = 10;
        int maxDepth = 10;

        point3 lookFrom = point3(0, 0, 0);
        point3 lookAt = point3(0, 0, -1);
        Vec3 vup = Vec3(0, 1, 0);

        double vfov = 90;

        sceneLights* lights = nullptr;
        SkyboxSampler* skybox = nullptr;

    private:
        int _imageHeight;
        double _pixelSamplesScale;
        point3 _center;
        point3 _pixel00Loc;
        Vec3 _pixelDeltaU;
        Vec3 _pixelDeltaV;
        Vec3 _u, _v, _w;

        double _focusDist = 10.0;

        void _initialize();
        Ray _getRay(int i, int j) const;
        Vec3 _sampleSquare() const;
        Color _rayColor(const Ray& r, int depth, const Hittable& world) const;

        static double _randomDouble();
        static double _degreesToRadians(double degrees);
        static double _linearToGamma(double linearComponent);
};

#pragma once

#include "Vec3.hpp"
#include <ofImage.h>
#include <string>

class SkyboxSampler {
    public:
        SkyboxSampler();

        bool loadFromFolder(const std::string& folderPath, const std::string& extension = "jpg");
        Color sample(const Vec3& direction) const;
        bool isLoaded() const;

    private:
        ofImage _right;
        ofImage _left;
        ofImage _top;
        ofImage _bottom;
        ofImage _front;
        ofImage _back;

        bool _loaded;

        Color _sampleFace(const ofImage& face, float u, float v) const;
};

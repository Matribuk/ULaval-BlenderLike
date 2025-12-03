#include "SkyboxSampler.hpp"
#include <cmath>
#include <algorithm>

SkyboxSampler::SkyboxSampler()
    : _loaded(false)
{}

bool SkyboxSampler::loadFromFolder(const std::string& folderPath, const std::string& extension)
{
    std::string right  = folderPath + "/right."  + extension;
    std::string left   = folderPath + "/left."   + extension;
    std::string top    = folderPath + "/top."    + extension;
    std::string bottom = folderPath + "/bottom." + extension;
    std::string front  = folderPath + "/front."  + extension;
    std::string back   = folderPath + "/back."   + extension;

    ofFile testFile(right);
    if (!testFile.exists()) {
        std::string altExt = (extension == "jpg") ? "png" : "jpg";
        right  = folderPath + "/right."  + altExt;
        left   = folderPath + "/left."   + altExt;
        top    = folderPath + "/top."    + altExt;
        bottom = folderPath + "/bottom." + altExt;
        front  = folderPath + "/front."  + altExt;
        back   = folderPath + "/back."   + altExt;
    }

    bool success = true;
    success &= this->_right.load(right);
    success &= this->_left.load(left);
    success &= this->_top.load(top);
    success &= this->_bottom.load(bottom);
    success &= this->_front.load(front);
    success &= this->_back.load(back);

    this->_loaded = success;
    return success;
}

Color SkyboxSampler::sample(const Vec3& direction) const
{
    if (!this->_loaded) {
        auto a = 0.5 * (direction.y() + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }

    Vec3 dir = unitVector(direction);
    float absX = std::abs(dir.x());
    float absY = std::abs(dir.y());
    float absZ = std::abs(dir.z());

    float u, v;
    const ofImage* face;

    if (absX >= absY && absX >= absZ) {
        if (dir.x() > 0) {
            face = &this->_right;
            u = -dir.z() / absX;
            v = -dir.y() / absX;
        } else {
            face = &this->_left;
            u = dir.z() / absX;
            v = -dir.y() / absX;
        }
    }
    else if (absY >= absX && absY >= absZ) {
        if (dir.y() > 0) {
            face = &this->_top;
            u = dir.x() / absY;
            v = dir.z() / absY;
        } else {
            face = &this->_bottom;
            u = dir.x() / absY;
            v = -dir.z() / absY;
        }
    }
    else {
        if (dir.z() > 0) {
            face = &this->_front;
            u = dir.x() / absZ;
            v = -dir.y() / absZ;
        } else {
            face = &this->_back;
            u = -dir.x() / absZ;
            v = -dir.y() / absZ;
        }
    }

    u = (u + 1.0f) * 0.5f;
    v = (v + 1.0f) * 0.5f;

    return this->_sampleFace(*face, u, v);
}

Color SkyboxSampler::_sampleFace(const ofImage& face, float u, float v) const
{
    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * (face.getWidth() - 1));
    int y = static_cast<int>(v * (face.getHeight() - 1));

    ofColor color = face.getColor(x, y);

    return Color(color.r / 255.0, color.g / 255.0, color.b / 255.0);
}

bool SkyboxSampler::isLoaded() const
{
    return this->_loaded;
}

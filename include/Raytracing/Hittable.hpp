#pragma once

#include <memory>

#include "Ray.hpp"
#include "Interval.hpp"

class Materials;
class Aabb;

struct HitRecord {
    point3 p;
    Vec3 normal;
    std::shared_ptr<Materials> mat;
    double t;
    bool frontFace;

    void setFaceNormal(const Ray& r, const Vec3& outwardNormal);
};

class Hittable {
    public:
        virtual ~Hittable() = default;

        virtual bool hit(const Ray& r, Interval rayT, HitRecord& rec) const = 0;
        virtual Aabb boundingBox() const = 0;
};

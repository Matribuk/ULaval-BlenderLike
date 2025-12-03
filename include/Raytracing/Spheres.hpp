#pragma once

#include "Hittable.hpp"
#include "Vec3.hpp"
#include "Aabb.hpp"

#include <memory>
#include <algorithm>
#include <cmath>


class Spheres : public Hittable {
    public:
        Spheres(const point3& center, double radius, std::shared_ptr<Materials> mat);

        bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override;
        Aabb boundingBox() const override;

    private:
        point3 _center;
        double _radius;
        Aabb _bbox;
        std::shared_ptr<Materials> _mat;
};

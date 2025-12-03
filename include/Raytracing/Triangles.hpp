#pragma once

#include "Hittable.hpp"
#include "Vec3.hpp"
#include "Aabb.hpp"

#include <memory>
#include <algorithm>
#include <cmath>

class Triangles : public Hittable {
    public:
        Triangles(const point3& v0, const point3& v1, const point3& v2, std::shared_ptr<Materials> mat);

        bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override;
        Aabb boundingBox() const override;

    private:
        point3 _v0, _v1, _v2;
        Vec3 _normal;
        Aabb _bbox;
        std::shared_ptr<Materials> _mat;
};

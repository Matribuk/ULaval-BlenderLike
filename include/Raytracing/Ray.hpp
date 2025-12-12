#pragma once

#include "Vec3.hpp"

class Ray {
    public:
        Ray() = default;
        Ray(const point3& origin, const Vec3& direction);

        const point3& origin() const;
        const Vec3& direction() const;
        point3 at(double t) const;

    private:
        point3 _orig;
        Vec3 _dir;
};

#pragma once

#include "Ray.hpp"
#include "Interval.hpp"
#include <cmath>

class Aabb {
    public:
        Interval x;
        Interval y;
        Interval z;

        Aabb() = default;
        Aabb(const Interval& x, const Interval& y, const Interval& z);
        Aabb(const point3& a, const point3& b);

        const Interval& axisInterval(int n) const;
        bool hit(const Ray& r, Interval rayT) const;

        static Aabb surroundingBox(const Aabb& box0, const Aabb& box1);

        double area() const;
        int longestAxis() const;

    private:
        void _padToMinimums();
};

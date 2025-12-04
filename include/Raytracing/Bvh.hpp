#pragma once

#include "Hittable.hpp"
#include "HittableList.hpp"
#include "Aabb.hpp"

#include <algorithm>
#include <memory>
#include <vector>

class HittableList;

class Bvh : public Hittable {
    public:
        Bvh(const HittableList& list);
        Bvh(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end);

        bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override;
        Aabb boundingBox() const override;

    private:
        std::shared_ptr<Hittable> _left;
        std::shared_ptr<Hittable> _right;
        Aabb _bbox;

        static Aabb _computeBoundingBox(
            const std::vector<std::shared_ptr<Hittable>>& objects,
            size_t start,
            size_t end
        );

        static bool _boxCompare(
            const std::shared_ptr<Hittable> a,
            const std::shared_ptr<Hittable> b,
            int axisIndex
        );

        static bool _boxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
        static bool _boxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
        static bool _boxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

        static Aabb _getBox(const std::shared_ptr<Hittable>& obj);
};

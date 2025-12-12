#pragma once

#include <memory>
#include <vector>

#include "Hittable.hpp"
#include "Aabb.hpp"

class HittableList : public Hittable {
    public:
        std::vector<std::shared_ptr<Hittable>> objects;

        HittableList() = default;
        HittableList(std::shared_ptr<Hittable> object);

        void clear();
        void add(std::shared_ptr<Hittable> object);

        bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override;
        Aabb boundingBox() const override;
};

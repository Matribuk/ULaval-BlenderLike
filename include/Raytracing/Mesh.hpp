#pragma once

#include "Hittable.hpp"
#include "Triangles.hpp"
#include "Bvh.hpp"

#include <vector>
#include <memory>

class Mesh : public Hittable {
    public:
        Mesh(std::shared_ptr<Materials> mat);

        void addTriangle(const point3& v0, const point3& v1, const point3& v2);
        void buildBVH();

        bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override;
        Aabb boundingBox() const override;
        size_t triangleCount() const;

    private:
        std::vector<std::shared_ptr<Triangles>> _triangles;
        std::shared_ptr<Materials> _mat;
        std::shared_ptr<Bvh> _bvhRoot;
        Aabb _bbox;
        bool _bvhBuilt;
};

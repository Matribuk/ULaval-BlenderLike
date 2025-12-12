#include "Mesh.hpp"

Mesh::Mesh(std::shared_ptr<Materials> mat)
{
    this->_mat = mat;
    this->_bvhBuilt = false;
}

void Mesh::addTriangle(const point3& v0, const point3& v1, const point3& v2)
{
    this->_triangles.push_back(std::make_shared<Triangles>(v0, v1, v2, this->_mat));
}

void Mesh::buildBVH()
{
    if (this->_triangles.empty()) return;

    this->_bbox = this->_triangles[0]->boundingBox();
    for (size_t i = 1; i < this->_triangles.size(); i++)
        this->_bbox = Aabb::surroundingBox(this->_bbox, this->_triangles[i]->boundingBox());

    HittableList triangleList;
    for (auto& tri : this->_triangles) triangleList.add(tri);

    this->_bvhRoot = std::make_shared<Bvh>(triangleList);
    this->_bvhBuilt = true;
}

bool Mesh::hit(const Ray& r, Interval rayT, HitRecord& rec) const
{
    if (!this->_bvhBuilt) return false;

    return this->_bvhRoot->hit(r, rayT, rec);
}

Aabb Mesh::boundingBox() const
{
    return this->_bbox;
}

size_t Mesh::triangleCount() const
{
    return this->_triangles.size();
}

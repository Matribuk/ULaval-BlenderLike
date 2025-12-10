#include "Triangles.hpp"

Triangles::Triangles(const point3& v0, const point3& v1, const point3& v2, std::shared_ptr<Materials> mat)
{
    this->_v0 = v0;
    this->_v1 = v1;
    this->_v2 = v2;
    this->_mat = mat;

    Vec3 edge1 = this->_v1 - this->_v0;
    Vec3 edge2 = this->_v2 - this->_v0;
    this->_normal = unitVector(cross(edge1, edge2));

    double minX = std::fmin(this->_v0.x(), std::fmin(this->_v1.x(), this->_v2.x()));
    double minY = std::fmin(this->_v0.y(), std::fmin(this->_v1.y(), this->_v2.y()));
    double minZ = std::fmin(this->_v0.z(), std::fmin(this->_v1.z(), this->_v2.z()));

    double maxX = std::fmax(this->_v0.x(), std::fmax(this->_v1.x(), this->_v2.x()));
    double maxY = std::fmax(this->_v0.y(), std::fmax(this->_v1.y(), this->_v2.y()));
    double maxZ = std::fmax(this->_v0.z(), std::fmax(this->_v1.z(), this->_v2.z()));

    this->_bbox = Aabb(point3(minX, minY, minZ), point3(maxX, maxY, maxZ));
}

bool Triangles::hit(const Ray& r, Interval rayT, HitRecord& rec) const
{
    const double EPSILON = 1e-7;

    Vec3 edge1 = this->_v1 - this->_v0;
    Vec3 edge2 = this->_v2 - this->_v0;
    Vec3 h = cross(r.direction(), edge2);
    double a = dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    double f = 1.0 / a;
    Vec3 s = r.origin() - this->_v0;
    double u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    Vec3 q = cross(s, edge1);
    double v = f * dot(r.direction(), q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    double t = f * dot(edge2, q);

    if (!rayT.surrounds(t))
        return false;

    rec.t = t;
    rec.p = r.at(t);
    rec.setFaceNormal(r, this->_normal);
    rec.mat = this->_mat;
    rec.u = u;
    rec.v = v;

    return true;
}

Aabb Triangles::boundingBox() const {
    return this->_bbox;
}

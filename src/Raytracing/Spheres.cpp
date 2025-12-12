#include "Spheres.hpp"

Spheres::Spheres(const point3& center, double radius, std::shared_ptr<Materials> mat)
{
    this->_center = center;
    this->_radius = std::fmax(0.0, radius);
    this->_mat = mat;

    Vec3 rVec(this->_radius, this->_radius, this->_radius);
    this->_bbox = Aabb(this->_center - rVec, this->_center + rVec);
}

bool Spheres::hit(const Ray& r, Interval rayT, HitRecord& rec) const
{
    Vec3 oc = this->_center - r.origin();
    auto a = r.direction().lengthSquared();
    auto h = dot(r.direction(), oc);
    auto c = oc.lengthSquared() - this->_radius * this->_radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0) return false;

    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (!rayT.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!rayT.surrounds(root)) return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3 outwardNormal = (rec.p - this->_center) / this->_radius;
    rec.setFaceNormal(r, outwardNormal);
    rec.mat = this->_mat;

    double theta = std::atan2(-outwardNormal.z(), outwardNormal.x()) + M_PI;
    double phi = std::acos(-outwardNormal.y());
    rec.u = theta / (2.0 * M_PI);
    rec.v = phi / M_PI;

    return true;
}

Aabb Spheres::boundingBox() const
{
    return this->_bbox;
}

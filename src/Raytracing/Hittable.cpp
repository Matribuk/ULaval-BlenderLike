#include "Hittable.hpp"

void HitRecord::setFaceNormal(const Ray& r, const Vec3& outwardNormal)
{
    frontFace = dot(r.direction(), outwardNormal) < 0;
    normal = frontFace ? outwardNormal : -outwardNormal;
}

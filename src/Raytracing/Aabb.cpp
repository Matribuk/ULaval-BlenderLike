#include "Aabb.hpp"


Aabb::Aabb(const Interval& x, const Interval& y, const Interval& z)
{
    this->x = x;
    this->y = y;
    this->z = z;

    this->_padToMinimums();
}

Aabb::Aabb(const point3& a, const point3& b)
{
    this->x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
    this->y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
    this->z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

    this->_padToMinimums();
}

const Interval& Aabb::axisInterval(int n) const
{
    if (n == 1) return this->y;
    if (n == 2) return this->z;
    return this->x;
}

bool Aabb::hit(const Ray& r, Interval rayT) const
{
    const point3& rayOrig = r.origin();
    const Vec3& rayDir = r.direction();

    for (int axis = 0; axis < 3; axis++) {
        const Interval& ax = this->axisInterval(axis);
        const double adinv = 1.0 / rayDir[axis];

        auto t0 = (ax.min - rayOrig[axis]) * adinv;
        auto t1 = (ax.max - rayOrig[axis]) * adinv;

        if (t0 < t1) {
            if (t0 > rayT.min) rayT.min = t0;
            if (t1 < rayT.max) rayT.max = t1;
        } else {
            if (t1 > rayT.min) rayT.min = t1;
            if (t0 < rayT.max) rayT.max = t0;
        }

        if (rayT.max <= rayT.min) return false;
    }

    return true;
}

Aabb Aabb::surroundingBox(const Aabb& box0, const Aabb& box1)
{
    Interval x(
        std::fmin(box0.x.min, box1.x.min),
        std::fmax(box0.x.max, box1.x.max)
    );

    Interval y(
        std::fmin(box0.y.min, box1.y.min),
        std::fmax(box0.y.max, box1.y.max)
    );

    Interval z(
        std::fmin(box0.z.min, box1.z.min),
        std::fmax(box0.z.max, box1.z.max)
    );

    return Aabb(x, y, z);
}

double Aabb::area() const
{
    auto a = this->x.size();
    auto b = this->y.size();
    auto c = this->z.size();

    return 2.0 * (a * b + b * c + c * a);
}

int Aabb::longestAxis() const
{
    if (this->x.size() > this->y.size()) return (this->x.size() > this->z.size()) ? 0 : 2;
    else return (this->y.size() > this->z.size()) ? 1 : 2;
}

void Aabb::_padToMinimums()
{
    const double delta = 0.0001;

    if (this->x.size() < delta) this->x = Interval(this->x.min - delta / 2, this->x.min + delta / 2);
    if (this->y.size() < delta) this->y = Interval(this->y.min - delta / 2, this->y.min + delta / 2);
    if (this->z.size() < delta) this->z = Interval(this->z.min - delta / 2, this->z.min + delta / 2);
}

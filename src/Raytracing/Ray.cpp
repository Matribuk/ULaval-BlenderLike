#include "Ray.hpp"

Ray::Ray(const point3& origin, const Vec3& direction)
{
    this->_orig = origin;
    this->_dir = direction;
}

const point3& Ray::origin() const
{
    return this->_orig;
}

const Vec3& Ray::direction() const
{
    return this->_dir;
}

point3 Ray::at(double t) const
{
    return this->_orig + t * this->_dir;
}

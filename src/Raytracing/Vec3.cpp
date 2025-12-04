#include "Vec3.hpp"

Vec3::Vec3() : _e{0,0,0} {}
Vec3::Vec3(double e0, double e1, double e2) : _e{e0, e1, e2} {}

double Vec3::x() const
{
    return this->_e[0];
}

double Vec3::y() const
{
    return this->_e[1];
}

double Vec3::z() const
{
    return this->_e[2];
}


Vec3 Vec3::operator-() const
{
    return Vec3(-this->_e[0], -this->_e[1], -this->_e[2]);
}

double Vec3::operator[](int i) const
{
    return this->_e[i];
}

double& Vec3::operator[](int i)
{
    return this->_e[i];
}

Vec3& Vec3::operator+=(const Vec3& v)
{
    this->_e[0] += v._e[0];
    this->_e[1] += v._e[1];
    this->_e[2] += v._e[2];
    return *this;
}

Vec3& Vec3::operator*=(double t)
{
    this->_e[0] *= t;
    this->_e[1] *= t;
    this->_e[2] *= t;
    return *this;
}

Vec3& Vec3::operator/=(double t)
{
    return *this *= 1/t;
}

double Vec3::length() const
{
    return std::sqrt(this->lengthSquared());
}

double Vec3::lengthSquared() const
{
    return this->_e[0]*this->_e[0] + this->_e[1]*this->_e[1] + this->_e[2]*this->_e[2];
}

bool Vec3::nearZero() const
{
    auto s = 1e-8;
    return (std::fabs(this->_e[0]) < s) && (std::fabs(this->_e[1]) < s) && (std::fabs(this->_e[2]) < s);
}

Vec3 Vec3::random()
{
    return Vec3(randomDouble(), randomDouble(), randomDouble());
}

Vec3 Vec3::random(double min, double max)
{
    return Vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

double Vec3::randomDouble()
{
    return std::rand() / (RAND_MAX + 1.0);
}

double Vec3::randomDouble(double min, double max)
{
    return min + (max - min) * randomDouble();
}

std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

Vec3 operator+(const Vec3& u, const Vec3& v)
{
    return Vec3(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

Vec3 operator-(const Vec3& u, const Vec3& v)
{
    return Vec3(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

Vec3 operator*(const Vec3& u, const Vec3& v)
{
    return Vec3(u[0]*v[0], u[1]*v[1], u[2]*v[2]);
}

Vec3 operator*(double t, const Vec3& v)
{
    return Vec3(t*v[0], t*v[1], t*v[2]);
}

Vec3 operator*(const Vec3& v, double t)
{
    return t*v;
}

Vec3 operator/(const Vec3& v, double t)
{
    return (1/t)*v;
}

double dot(const Vec3& u, const Vec3& v)
{
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

Vec3 cross(const Vec3& u, const Vec3& v)
{
    return Vec3(u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0]);
}

Vec3 unitVector(const Vec3& v)
{
    return v / v.length();
}

Vec3 randomInUnitSphere()
{
    while(true){
        auto p = Vec3::random(-1,1);
        if (p.lengthSquared() < 1) return p;
    }
}

Vec3 randomUnitVector()
{
    return unitVector(randomInUnitSphere());
}

Vec3 randomOnHemisphere(const Vec3& normal)
{
    Vec3 inUnitSphere = randomUnitVector();
    if (dot(inUnitSphere, normal) > 0.0) return inUnitSphere;
    else return -inUnitSphere;
}

Vec3 reflect(const Vec3& v, const Vec3& n)
{
    return v - 2*dot(v,n)*n;
}

Vec3 refract(const Vec3& uv, const Vec3& n, double etaiOverEtat)
{
    auto cosTheta = std::fmin(dot(-uv, n), 1.0);
    Vec3 rOutPerp = etaiOverEtat * (uv + cosTheta*n);
    Vec3 rOutParallel = -std::sqrt(std::fabs(1.0 - rOutPerp.lengthSquared())) * n;
    return rOutPerp + rOutParallel;
}

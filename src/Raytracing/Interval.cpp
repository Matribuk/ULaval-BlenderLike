#include "Interval.hpp"

Interval::Interval() : min(+INFINITY), max(-INFINITY) {}

Interval::Interval(double min, double max)
{
    this->min = min;
    this->max = max;
}

double Interval::size() const
{
    return this->max - this->min;
}

bool Interval::contains(double x) const
{
    return this->min <= x && x <= this->max;
}

bool Interval::surrounds(double x) const
{
    return this->min < x && x < this->max;
}

double Interval::clamp(double x) const
{
    if (x < this->min) return this->min;
    if (x > this->max) return this->max;
    return x;
}

const Interval Interval::empty = Interval(+INFINITY, -INFINITY);
const Interval Interval::universe = Interval(-INFINITY, +INFINITY);

double linearToGamma(double linearComponent)
{
    if (linearComponent > 0.0) return std::sqrt(linearComponent);
    return 0.0;
}

void writeColor(std::ostream& out, const Color& pixelColor)
{
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    r = linearToGamma(r);
    g = linearToGamma(g);
    b = linearToGamma(b);

    int rByte = int(256 * std::clamp(r, 0.0, 0.999));
    int gByte = int(256 * std::clamp(g, 0.0, 0.999));
    int bByte = int(256 * std::clamp(b, 0.0, 0.999));
}

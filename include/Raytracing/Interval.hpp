#pragma once

#include "Vec3.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>

using Color = Vec3;

struct Interval {
    double min, max;

    Interval();
    Interval(double min, double max);

    double size() const;
    bool contains(double x) const;
    bool surrounds(double x) const;
    double clamp(double x) const;

    static const Interval empty;
    static const Interval universe;
};

double linearToGamma(double linearComponent);
void writeColor(std::ostream& out, const Color& pixelColor);

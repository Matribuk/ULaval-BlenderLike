#pragma once

#include "Core/Entity.hpp"
#include <ofMain.h>

struct DisplacementMap {
    float strength = 0.5f;
    bool needsRegeneration = false;
    int subdivisionLevel = 3;

    DisplacementMap() = default;
    DisplacementMap(float str, int subdivLevel = 3)
        : strength(str), subdivisionLevel(subdivLevel) {}
};

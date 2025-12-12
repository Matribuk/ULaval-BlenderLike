#pragma once

#include <ofMain.h>

enum class LightType {
    AMBIENT,
    DIRECTIONAL,
    POINT,
    SPOT
};

struct LightSource {
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float spotAngle;
    float attenuation;
    bool enabled;

    LightSource()
        : type(LightType::POINT),
          position(0.0f, 5.0f, 0.0f),
          direction(0.0f, -1.0f, 0.0f),
          color(1.0f, 1.0f, 1.0f),
          intensity(1.0f),
          spotAngle(45.0f),
          attenuation(1.0f),
          enabled(true) {}

    LightSource(LightType t, glm::vec3 pos, glm::vec3 col, float intens)
        : type(t),
          position(pos),
          direction(0.0f, -1.0f, 0.0f),
          color(col),
          intensity(intens),
          spotAngle(45.0f),
          attenuation(1.0f),
          enabled(true) {}
};

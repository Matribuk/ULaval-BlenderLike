#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct Camera {
    glm::vec3 position{0.0f, 0.0f, 5.0f};
    glm::vec3 forward{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    bool focusMode{false};
    glm::vec3 target{0.0f, 0.0f, 0.0f};

    float fov{60.0f};
    float nearClip{0.1f};
    float farClip{1000.0f};

    float aspectRatio{1.0f};

    glm::mat4 viewMatrix{1.0f};
    glm::mat4 projMatrix{1.0f};

    Camera() = default;
};
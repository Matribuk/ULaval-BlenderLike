#pragma once

#include <ofMain.h>

struct Material {
    ofShader* shader = nullptr;
    ofTexture* texture = nullptr;

    float scale = 5.0f;
    float octaves = 4.0f;
    float persistence = 0.5f;
    float borderWidth = 0.05f;

    glm::vec3 lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float lightIntensity = 1.0f;
    glm::vec3 ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
    float shininess = 32.0f;

    float reflectivity = 0.0f;
    glm::vec3 reflectionTint = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct Renderable {
    ofMesh mesh;
    ofColor color{255, 255, 255};
    bool visible{true};
    Material* material{nullptr};

    bool showOutline{false};
    bool isPrimitive{false};

    Renderable() = default;
    Renderable(const ofMesh& m, const ofColor& c = ofColor(255, 255, 255), bool v = true, ofShader* s = nullptr, ofTexture* t = nullptr, bool primitive = false)
        : mesh(m), color(c), visible(v), material(new Material{s, t}), isPrimitive(primitive) {}
};


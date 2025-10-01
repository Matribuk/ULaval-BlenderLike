#pragma once

#include <ofMain.h>

struct Material {
    ofShader* shader = nullptr;
    ofTexture* texture = nullptr;
};

struct Renderable {
    ofMesh mesh;
    ofColor color{255, 255, 255};
    bool visible{true};
    Material* material{nullptr};

    Renderable() = default;
    Renderable(const ofMesh& m, const ofColor& c = ofColor(255, 255, 255), bool v = true)
        : mesh(m), color(c), visible(v) {}
};


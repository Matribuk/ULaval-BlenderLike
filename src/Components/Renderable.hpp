#pragma once

#include "ofMesh.h"
#include "ofColor.h"

struct Renderable {
    ofMesh mesh;
    ofColor color{255, 255, 255};
    bool visible{true};

    Renderable() = default;
    Renderable(const ofMesh& m, const ofColor& c = ofColor(255, 255, 255), bool v = true)
        : mesh(m), color(c), visible(v) {}
};


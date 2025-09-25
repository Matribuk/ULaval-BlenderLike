#include <iostream>
#include <cassert>
#include "../Components/Renderable.hpp"

void printTest(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
}

void testRenderableDefaults() {
    std::cout << "\n=== Test 1: Renderable valeurs par défaut ===\n";
    Renderable r;

    printTest("Visible = true", r.visible == true);
    printTest("Couleur par défaut = blanc", r.color == ofColor(255,255,255));
}

void testRenderableCustom() {
    std::cout << "\n=== Test 2: Constructeur custom ===\n";
    ofMesh mesh = ofMesh::box(1,1,1,1,1,1);
    Renderable r(mesh, ofColor::red, false);

    printTest("Mesh assigné", r.mesh.getNumVertices() > 0);
    printTest("Couleur rouge", r.color == ofColor::red);
    printTest("Visible = false", r.visible == false);
}

int main() {
    std::cout << "🧪 TESTS RENDERABLE\n";
    std::cout << "===================\n";

    testRenderableDefaults();
    testRenderableCustom();

    std::cout << "\n🎯 Tests Renderable terminés\n";
    return 0;
}

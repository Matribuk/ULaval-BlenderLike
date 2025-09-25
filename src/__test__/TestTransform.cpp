#include <iostream>
#include <cassert>
#include "../Components/Transform.hpp"

void printTest(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
    if (!passed)
        std::cout << "   ⚠️ Vérifiez votre implémentation !" << std::endl;
}

void testTransformDefaults() {
    std::cout << "\n=== Test 1: Transform valeurs par défaut ===\n";
    Transform t;

    printTest("Position par défaut = (0,0,0)", t.position == glm::vec3(0.0f));
    printTest("Rotation par défaut = (0,0,0)", t.rotation == glm::vec3(0.0f));
    printTest("Scale par défaut = (1,1,1)", t.scale == glm::vec3(1.0f,1.0f,1.0f));
}

void testTransformCustom() {
    std::cout << "\n=== Test 2: Constructeurs ===\n";
    Transform t1({1,2,3});
    Transform t2({1,2,3},{90,0,0},{2,2,2});

    printTest("Constructeur position ok", t1.position == glm::vec3(1,2,3));
    printTest("Constructeur complet ok", t2.scale == glm::vec3(2,2,2));
}

int main() {
    std::cout << "🧪 TESTS TRANSFORM\n";
    std::cout << "==================\n";

    testTransformDefaults();
    testTransformCustom();

    std::cout << "\n🎯 Tests Transform terminés\n";
    return 0;
}

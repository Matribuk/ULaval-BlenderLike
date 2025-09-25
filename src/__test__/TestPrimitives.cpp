#include <iostream>
#include <cassert>
#include "../Components/Primitive/Box.hpp"
#include "../Components/Primitive/Sphere.hpp"
#include "../Components/Primitive/Plane.hpp"

void printTest(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
}

void testBox() {
    std::cout << "\n=== Test 1: Box ===\n";
    Box b({2,3,4});

    printTest("Dimensions correctes", b.dimensions == glm::vec3(2,3,4));
}

void testSphere() {
    std::cout << "\n=== Test 2: Sphere ===\n";
    Sphere s(5.0f);

    printTest("Radius correct", s.radius == 5.0f);
}

void testPlane() {
    std::cout << "\n=== Test 3: Plane ===\n";
    Plane p({2,5});

    printTest("Taille correcte", p.size == glm::vec2(2,5));
}

int main() {
    std::cout << "🧪 TESTS PRIMITIVES\n";
    std::cout << "===================\n";

    testBox();
    testSphere();
    testPlane();

    std::cout << "\n🎯 Tests Primitives terminés\n";
    return 0;
}

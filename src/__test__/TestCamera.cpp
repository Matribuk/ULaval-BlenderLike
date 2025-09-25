#include <iostream>
#include <cassert>
#include "../Components/Camera.hpp"

void printTest(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
}

void testCameraDefaults() {
    std::cout << "\n=== Test 1: Camera valeurs par défaut ===\n";
    Camera cam;

    printTest("FOV = 60°", cam.fov == 60.0f);
    printTest("Near clip = 0.1f", cam.nearClip == 0.1f);
    printTest("Far clip = 1000.0f", cam.farClip == 1000.0f);
    printTest("Aspect ratio initial = 1.0f", cam.aspectRatio == 1.0f);
}

void testCameraMatrices() {
    std::cout << "\n=== Test 2: Matrices par défaut ===\n";
    Camera cam;

    printTest("viewMatrix identitée ?", cam.viewMatrix == glm::mat4(1.0f));
    printTest("projMatrix identitée ?", cam.projMatrix == glm::mat4(1.0f));
}

int main() {
    std::cout << "🧪 TESTS CAMERA\n";
    std::cout << "===============\n";

    testCameraDefaults();
    testCameraMatrices();

    std::cout << "\n🎯 Tests Camera terminés\n";
    return 0;
}

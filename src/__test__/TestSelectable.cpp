#include <iostream>
#include <cassert>
#include "../Components/Selectable.hpp"

void printTest(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
}

void testSelectableDefaults() {
    std::cout << "\n=== Test 1: Selectable valeurs par défaut ===\n";
    Selectable s;

    printTest("isSelected = false", s.isSelected == false);
    printTest("Couleur normale = blanc", s.normalColor == ofColor::white);
}

void testSelectableCustom() {
    std::cout << "\n=== Test 2: Modification ===\n";
    Selectable s;
    s.isSelected = true;
    s.selectedColor = ofColor::green;

    printTest("isSelected = true", s.isSelected == true);
    printTest("selectedColor = vert", s.selectedColor == ofColor::green);
}

int main() {
    std::cout << "🧪 TESTS SELECTABLE\n";
    std::cout << "===================\n";

    testSelectableDefaults();
    testSelectableCustom();

    std::cout << "\n🎯 Tests Selectable terminés\n";
    return 0;
}

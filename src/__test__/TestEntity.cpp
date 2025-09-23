#include <iostream>
#include <cassert>
#include "../Core/Entity/Entity.hpp"

void printTest(const std::string& testName, bool passed)
{
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
    if (!passed) {
        std::cout << "   ⚠️  Ce test a échoué - vérifiez votre implémentation!" << std::endl;
    }
}

void testEntityCreation()
{
    std::cout << "\n=== Test 1: Création d'entités ===\n";

    Entity e1(1);
    Entity e2(2);
    Entity e3(42);

    printTest("e1 est valide", e1.isValid());
    printTest("e2 est valide", e2.isValid());
    printTest("e3 est valide", e3.isValid());
    printTest("e3 a bien l'ID 42", e3.getId() == 42);
    printTest("e1 et e2 ont des IDs distincts", e1.getId() != e2.getId());
}

void testEntityEquality()
{
    std::cout << "\n=== Test 2: Comparaison d'entités ===\n";

    Entity e1(10);
    Entity e2(10);
    Entity e3(11);

    printTest("Deux entités avec le même ID sont égales", e1 == e2);
    printTest("Deux entités avec des IDs différents ne sont pas égales", !(e1 == e3));
    printTest("Ordre strict : e1 < e3", e1 < e3);
    printTest("Ordre strict : !(e3 < e1)", !(e3 < e1));
}

void testInvalidEntity()
{
    std::cout << "\n=== Test 3: Entité invalide ===\n";

    Entity invalid(INVALID_ENTITY);
    printTest("Entité avec INVALID_ENTITY n'est pas valide", !invalid.isValid());
}

int main()
{
    std::cout << "🧪 TESTS ENTITY\n";
    std::cout << "===============\n";

    testEntityCreation();
    testEntityEquality();
    testInvalidEntity();

    std::cout << "\n🎯 Tests terminés !\n";
    std::cout << "Si tous les tests sont ✅ PASS, votre Entity fonctionne correctement.\n";
    std::cout << "Si des tests sont ❌ FAIL, vérifiez votre implémentation.\n\n";

    return 0;
}

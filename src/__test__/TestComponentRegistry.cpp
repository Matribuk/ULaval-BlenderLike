#include <iostream>
#include <cassert>
#include "../Core/ComponentRegistry/ComponentRegistry.hpp"

struct Transform {
    float x, y, z;
    Transform(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    bool operator==(const Transform& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct Health {
    int hp;
    int maxHp;
    Health(int hp = 100, int maxHp = 100) : hp(hp), maxHp(maxHp) {}

    bool operator==(const Health& other) const {
        return hp == other.hp && maxHp == other.maxHp;
    }
};

struct Name {
    std::string value;
    Name(const std::string& name = "") : value(name) {}

    bool operator==(const Name& other) const {
        return value == other.value;
    }
};

void printTest(const std::string& testName, bool passed)
{
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
    if (!passed) {
        std::cout << "   ⚠️  Ce test a échoué - vérifiez votre implémentation!" << std::endl;
    }
}

void testBasicComponentOperations()
{
    std::cout << "\n=== Test 1: Opérations de base ===\n";

    ComponentRegistry registry;

    registry.registerComponent<Transform>(1, Transform(1.0f, 2.0f, 3.0f));
    bool test1 = registry.hasComponent<Transform>(1);
    printTest("Enregistrer et vérifier composant", test1);

    Transform* transform = registry.getComponent<Transform>(1);
    bool test2 = (transform != nullptr) && (*transform == Transform(1.0f, 2.0f, 3.0f));
    printTest("Récupérer composant correct", test2);

    Transform* nonExistent = registry.getComponent<Transform>(999);
    bool test3 = (nonExistent == nullptr);
    printTest("Composant inexistant retourne nullptr", test3);

    Health* healthNonExistent = registry.getComponent<Health>(1);
    bool test4 = (healthNonExistent == nullptr);
    printTest("Type de composant inexistant retourne nullptr", test4);
}

void testMultipleEntities()
{
    std::cout << "\n=== Test 2: Entités multiples ===\n";

    ComponentRegistry registry;

    registry.registerComponent<Transform>(1, Transform(1.0f, 1.0f, 1.0f));
    registry.registerComponent<Transform>(2, Transform(2.0f, 2.0f, 2.0f));
    registry.registerComponent<Transform>(3, Transform(3.0f, 3.0f, 3.0f));

    Transform* t1 = registry.getComponent<Transform>(1);
    Transform* t2 = registry.getComponent<Transform>(2);
    Transform* t3 = registry.getComponent<Transform>(3);

    bool test1 = t1 && (*t1 == Transform(1.0f, 1.0f, 1.0f));
    bool test2 = t2 && (*t2 == Transform(2.0f, 2.0f, 2.0f));
    bool test3 = t3 && (*t3 == Transform(3.0f, 3.0f, 3.0f));

    printTest("Entité 1 garde son Transform", test1);
    printTest("Entité 2 garde son Transform", test2);
    printTest("Entité 3 garde son Transform", test3);

    bool test4 = registry.hasComponent<Transform>(1) &&
                 registry.hasComponent<Transform>(2) &&
                 registry.hasComponent<Transform>(3);
    printTest("Aucune entité n'écrase les autres", test4);
}

void testMultipleComponentTypes()
{
    std::cout << "\n=== Test 3: Types de composants multiples ===\n";

    ComponentRegistry registry;

    registry.registerComponent<Transform>(1, Transform(5.0f, 6.0f, 7.0f));
    registry.registerComponent<Health>(1, Health(150, 200));
    registry.registerComponent<Name>(1, Name("Player"));

    bool test1 = registry.hasComponent<Transform>(1) &&
                 registry.hasComponent<Health>(1) &&
                 registry.hasComponent<Name>(1);
    printTest("Entité peut avoir plusieurs types de composants", test1);

    Transform* transform = registry.getComponent<Transform>(1);
    Health* health = registry.getComponent<Health>(1);
    Name* name = registry.getComponent<Name>(1);

    bool test2 = transform && (*transform == Transform(5.0f, 6.0f, 7.0f));
    bool test3 = health && (*health == Health(150, 200));
    bool test4 = name && (*name == Name("Player"));

    printTest("Transform récupéré correctement", test2);
    printTest("Health récupéré correctement", test3);
    printTest("Name récupéré correctement", test4);
}

void testComponentRemoval()
{
    std::cout << "\n=== Test 4: Suppression de composants ===\n";

    ComponentRegistry registry;

    registry.registerComponent<Transform>(1, Transform(1.0f, 2.0f, 3.0f));
    registry.registerComponent<Health>(1, Health(100, 100));
    registry.registerComponent<Transform>(2, Transform(4.0f, 5.0f, 6.0f));

    bool removed = registry.removeComponent<Transform>(1);
    bool test1 = removed && !registry.hasComponent<Transform>(1);
    printTest("Suppression composant spécifique", test1);

    bool test2 = registry.hasComponent<Health>(1);
    printTest("Autres composants de l'entité préservés", test2);

    bool test3 = registry.hasComponent<Transform>(2);
    printTest("Autres entités non affectées", test3);

    bool removedNonExistent = registry.removeComponent<Name>(1);
    bool test4 = !removedNonExistent;
    printTest("Suppression composant inexistant retourne false", test4);
}

void testRemoveAllComponents()
{
    std::cout << "\n=== Test 5: Suppression complète ===\n";

    ComponentRegistry registry;

    registry.registerComponent<Transform>(1, Transform(1.0f, 2.0f, 3.0f));
    registry.registerComponent<Health>(1, Health(100, 100));
    registry.registerComponent<Name>(1, Name("ToDelete"));
    registry.registerComponent<Transform>(2, Transform(4.0f, 5.0f, 6.0f));

    registry.removeAllComponents(1);

    bool test1 = !registry.hasComponent<Transform>(1) &&
                 !registry.hasComponent<Health>(1) &&
                 !registry.hasComponent<Name>(1);
    printTest("Tous les composants de l'entité supprimés", test1);

    bool test2 = registry.hasComponent<Transform>(2);
    printTest("Autres entités préservées", test2);
}

void testComponentModification()
{
    std::cout << "\n=== Test 6: Modification de composants ===\n";

    ComponentRegistry registry;

    registry.registerComponent<Transform>(1, Transform(1.0f, 2.0f, 3.0f));

    Transform* transform = registry.getComponent<Transform>(1);
    if (transform) {
        transform->x = 99.0f;
        transform->y = 88.0f;
        transform->z = 77.0f;
    }

    Transform* modifiedTransform = registry.getComponent<Transform>(1);
    bool test1 = modifiedTransform && (*modifiedTransform == Transform(99.0f, 88.0f, 77.0f));
    printTest("Modification via pointeur fonctionne", test1);

    registry.registerComponent<Transform>(1, Transform(11.0f, 22.0f, 33.0f));
    Transform* replacedTransform = registry.getComponent<Transform>(1);
    bool test2 = replacedTransform && (*replacedTransform == Transform(11.0f, 22.0f, 33.0f));
    printTest("Remplacement complet du composant", test2);
}

void runStressTest()
{
    std::cout << "\n=== Test 7: Test de stress ===\n";

    ComponentRegistry registry;

    const int numEntities = 1000;

    for (int i = 1; i <= numEntities; i++) {
        registry.registerComponent<Transform>(i, Transform(i * 1.0f, i * 2.0f, i * 3.0f));
        if (i % 2 == 0) {
            registry.registerComponent<Health>(i, Health(i, i * 2));
        }
    }

    bool allTransformsOk = true;
    bool healthsOk = true;

    for (int i = 1; i <= numEntities; i++) {
        Transform* t = registry.getComponent<Transform>(i);
        if (!t || !(*t == Transform(i * 1.0f, i * 2.0f, i * 3.0f))) {
            allTransformsOk = false;
            break;
        }

        if (i % 2 == 0) {
            Health* h = registry.getComponent<Health>(i);
            if (!h || !(*h == Health(i, i * 2))) {
                healthsOk = false;
                break;
            }
        }
    }

    printTest("1000 entités avec Transform OK", allTransformsOk);
    printTest("500 entités avec Health OK", healthsOk);
}

int main()
{
    std::cout << "🧪 TESTS COMPONENTREGISTRY\n";
    std::cout << "==========================\n";

    testBasicComponentOperations();
    testMultipleEntities();
    testMultipleComponentTypes();
    testComponentRemoval();
    testRemoveAllComponents();
    testComponentModification();
    runStressTest();

    std::cout << "\n🎯 Tests terminés !\n";
    std::cout << "Si tous les tests sont ✅ PASS, votre ComponentRegistry fonctionne correctement.\n";
    std::cout << "Si des tests sont ❌ FAIL, vérifiez votre implémentation.\n\n";

    return 0;
}
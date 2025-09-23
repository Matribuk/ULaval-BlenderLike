#include <iostream>
#include <cassert>
#include "../Core/EntityManager/EntityManager.hpp"

void printTest(const std::string& testName, bool passed)
{
    std::cout << "[" << (passed ? "✅ PASS" : "❌ FAIL") << "] " << testName << std::endl;
    if (!passed) {
        std::cout << "   ⚠️  Ce test a échoué - vérifiez votre implémentation!" << std::endl;
    }
}

void testEntityManagerCreation()
{
    std::cout << "\n=== Test 1: Création d'entités via EntityManager ===\n";

    EntityManager manager;
    Entity e1 = manager.createEntity();
    Entity e2 = manager.createEntity();

    printTest("e1 est valide", manager.isEntityValid(e1.getId()));
    printTest("e2 est valide", manager.isEntityValid(e2.getId()));
    printTest("Les entités ont des IDs distincts", e1.getId() != e2.getId());
}

void testEntityManagerDestruction()
{
    std::cout << "\n=== Test 2: Destruction d'entités ===\n";

    EntityManager manager;
    Entity e1 = manager.createEntity();
    EntityID id1 = e1.getId();

    manager.destroyEntity(id1);

    printTest("Entité supprimée devient invalide", !manager.isEntityValid(id1));
    printTest("Compteur d'entités est bien décrémenté", manager.getEntityCount() == 0);
}

void testEntityManagerRecycling()
{
    std::cout << "\n=== Test 3: Recyclage des IDs ===\n";

    EntityManager manager;
    Entity e1 = manager.createEntity();
    EntityID oldId = e1.getId();

    manager.destroyEntity(oldId);

    Entity e2 = manager.createEntity();
    EntityID newId = e2.getId();

    printTest("ID recyclé correctement", newId == oldId);
}

void testEntityManagerList()
{
    std::cout << "\n=== Test 4: Liste et compteur d'entités ===\n";

    EntityManager manager;
    Entity e1 = manager.createEntity();
    Entity e2 = manager.createEntity();
    Entity e3 = manager.createEntity();

    const auto& entities = manager.getAllEntities();
    printTest("Compteur retourne 3 entités", manager.getEntityCount() == 3);
    printTest("Liste contient bien 3 IDs", entities.size() == 3);
    printTest("La liste contient e1", entities[0] == e1.getId());
    printTest("La liste contient e2", entities[1] == e2.getId());
    printTest("La liste contient e3", entities[2] == e3.getId());
}

int main()
{
    std::cout << "🧪 TESTS ENTITYMANAGER\n";
    std::cout << "======================\n";

    testEntityManagerCreation();
    testEntityManagerDestruction();
    testEntityManagerRecycling();
    testEntityManagerList();

    std::cout << "\n🎯 Tests terminés !\n";
    std::cout << "Si tous les tests sont ✅ PASS, votre EntityManager fonctionne correctement.\n";
    std::cout << "Si des tests sont ❌ FAIL, vérifiez votre implémentation.\n\n";

    return 0;
}

#pragma once

#include "Box.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Renderable.hpp"
#include "ComponentRegistry.hpp"
#include "EntityManager.hpp"

class PrimitiveSystem {
    public:
        PrimitiveSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~PrimitiveSystem() = default;

        void generateMeshes();

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;

        ofMesh generateBoxMesh(const glm::vec3& dims);
        ofMesh generateSphereMesh(float radius);
        ofMesh generatePlaneMesh(const glm::vec2& size);
};

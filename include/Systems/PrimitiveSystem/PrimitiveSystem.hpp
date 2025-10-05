#pragma once

// Components
#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Renderable.hpp"

// Core
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"

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

#pragma once

#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Renderable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

class PrimitiveSystem {
    public:
        PrimitiveSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~PrimitiveSystem() = default;

        void generateMeshes();

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;

        ofMesh _generateBoxMesh(const glm::vec3& dims);
        ofMesh _generateSphereMesh(float radius);
        ofMesh _generatePlaneMesh(const glm::vec2& size);
        void _generateDefaultTexCoords(ofMesh& mesh, const std::string& mode = "box");
};

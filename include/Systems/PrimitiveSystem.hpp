#pragma once

#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Triangle.hpp"
#include "Components/Primitive/Circle.hpp"
#include "Components/Primitive/Line.hpp"
#include "Components/Primitive/Rectangle.hpp"
#include "Components/Primitive/Point.hpp"
#include "Components/Renderable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"
#include <cmath>

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
        ofMesh _generateTriangleMesh(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
        ofMesh _generateCircleMesh(float radius, int segments);
        ofMesh _generateLineMesh(const glm::vec3& start, const glm::vec3& end);
        ofMesh _generateRectangleMesh(float width, float height);
        ofMesh _generatePointMesh(float size);
};

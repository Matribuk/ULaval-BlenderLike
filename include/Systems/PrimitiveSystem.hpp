#pragma once

#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Triangle.hpp"
#include "Components/Primitive/Circle.hpp"
#include "Components/Primitive/Line.hpp"
#include "Components/Primitive/Rectangle.hpp"
#include "Components/Primitive/Point.hpp"
#include "Components/Primitive/DelaunayMesh.hpp"
#include "Components/Primitive/ParametricCurve.hpp"
#include "Components/Renderable.hpp"
#include "Components/CustomBounds.hpp"
#include "Components/Transform.hpp"
#include "Components/DisplacementMap.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"
#include "Algorithms/Delaunay.hpp"
#include "Algorithms/BezierCurve.hpp"
#include "Algorithms/CatmullRomCurve.hpp"
#include <cmath>
#include <random>

class ResourceManager;

class PrimitiveSystem {
    public:
        PrimitiveSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~PrimitiveSystem() = default;

        void setResourceManager(ResourceManager* resourceManager);
        void generateMeshes();
        void updateControlPointBasedMeshes();
        void applyDisplacement(EntityID entityId);
        void regenerateMesh(EntityID entityId);

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;
        ResourceManager* _resourceManager = nullptr;

        ofMesh _generateBoxMesh(const glm::vec3& dims);
        ofMesh _generateSphereMesh(float radius);
        ofMesh _generatePlaneMesh(const glm::vec2& size);
        ofMesh _generateTriangleMesh(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
        ofMesh _generateCircleMesh(float radius, int segments);
        ofMesh _generateLineMesh(const glm::vec3& start, const glm::vec3& end);
        ofMesh _generateRectangleMesh(float width, float height);
        ofMesh _generatePointMesh(float size);
        ofMesh _generateDelaunayMesh(const DelaunayMesh& delaunay, EntityID entityId);
        ofMesh _generateParametricCurveMesh(const ParametricCurve& curve, EntityID entityId);

        std::vector<glm::vec2> _generateRandomPoints(int count, const glm::vec2& bounds, unsigned int seed);
        std::vector<glm::vec2> _generateGridPoints(int resolution, const glm::vec2& bounds, float perturbation, unsigned int seed);
        ofColor _generateColorFromPosition(const glm::vec2& pos);

        void _updateDelaunayFromControlPoints(EntityID delaunayId, DelaunayMesh& delaunay);
        void _updateCurveFromControlPoints(EntityID curveId, ParametricCurve& curve);

        std::vector<glm::vec2> _extractDelaunayControlPoints(const DelaunayMesh& delaunay, const glm::mat4& inverseMatrix);
        std::vector<glm::vec3> _extractCurveControlPoints(const ParametricCurve& curve, const glm::mat4& inverseMatrix);
        bool _needsDelaunayUpdate(const std::vector<glm::vec2>& currentPoints, const DelaunayMesh& delaunay);
        bool _needsCurveUpdate(const std::vector<glm::vec3>& currentPoints, const ParametricCurve& curve);
        void _addDelaunayTriangles(ofMesh& mesh, const std::vector<Triangle2D>& triangles, const DelaunayMesh& delaunay);
        void _addVoronoiCells(ofMesh& mesh, const std::vector<VoronoiCell>& cells, const DelaunayMesh& delaunay);

        ofMesh _subdivideMesh(const ofMesh& mesh, int level);
        void _displaceVertex(glm::vec3& vertex, const glm::vec3& normal, float height, float strength);
};

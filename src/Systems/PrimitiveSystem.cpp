#include "Systems/PrimitiveSystem.hpp"

PrimitiveSystem::PrimitiveSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void PrimitiveSystem::generateMeshes() {
    for (EntityID id : this->_entityManager.getAllEntities()) {
        Renderable* render = this->_registry.getComponent<Renderable>(id);
        if (!render || !render->isPrimitive) continue;

        if (Box* box = this->_registry.getComponent<Box>(id)) {
            render->mesh = this->_generateBoxMesh(box->dimensions);
        }
        else if (Sphere* sphere = this->_registry.getComponent<Sphere>(id)) {
            render->mesh = this->_generateSphereMesh(sphere->radius);
        }
        else if (Plane* plane = this->_registry.getComponent<Plane>(id)) {
            render->mesh = this->_generatePlaneMesh(plane->size);
        }
        else if (Triangle* triangle = this->_registry.getComponent<Triangle>(id)) {
            render->mesh = this->_generateTriangleMesh(triangle->vertex1, triangle->vertex2, triangle->vertex3);
        }
        else if (Circle* circle = this->_registry.getComponent<Circle>(id)) {
            render->mesh = this->_generateCircleMesh(circle->radius, circle->segments);
        }
        else if (Line* line = this->_registry.getComponent<Line>(id)) {
            render->mesh = this->_generateLineMesh(line->start, line->end);
        }
        else if (Rectangle* rectangle = this->_registry.getComponent<Rectangle>(id)) {
            render->mesh = this->_generateRectangleMesh(rectangle->width, rectangle->height);
        }
        else if (Point* point = this->_registry.getComponent<Point>(id)) {
            render->mesh = this->_generatePointMesh(point->size);
        }
        else if (DelaunayMesh* delaunay = this->_registry.getComponent<DelaunayMesh>(id)) {
            render->mesh = this->_generateDelaunayMesh(*delaunay, id);
        }
        else if (ParametricCurve* curve = this->_registry.getComponent<ParametricCurve>(id)) {
            render->mesh = this->_generateParametricCurveMesh(*curve, id);
        }
    }
}

void PrimitiveSystem::updateControlPointBasedMeshes()
{
    for (EntityID id : this->_entityManager.getAllEntities()) {
        DelaunayMesh* delaunay = this->_registry.getComponent<DelaunayMesh>(id);
        if (delaunay && delaunay->mode == DelaunayMesh::GenerationMode::CUSTOM && !delaunay->controlPointEntities.empty()) {
            bool needsUpdate = false;
            std::vector<glm::vec2> currentPoints;
            currentPoints.reserve(delaunay->controlPointEntities.size());

            Transform* delaunayTransform = this->_registry.getComponent<Transform>(id);
            glm::mat4 delaunayInverseMatrix = glm::mat4(1.0f);

            if (delaunayTransform) {
                delaunayInverseMatrix = glm::inverse(delaunayTransform->globalMatrix);
            }

            for (EntityID pointId : delaunay->controlPointEntities) {
                Transform* transform = this->_registry.getComponent<Transform>(pointId);
                if (transform) {
                    glm::vec3 globalPos = glm::vec3(transform->globalMatrix[3]);

                    glm::vec4 localPos4 = delaunayInverseMatrix * glm::vec4(globalPos, 1.0f);
                    glm::vec3 localPos = glm::vec3(localPos4);

                    currentPoints.push_back(glm::vec2(localPos.x, localPos.y));
                }
            }

            if (currentPoints.size() == delaunay->points.size()) {
                for (size_t i = 0; i < currentPoints.size(); ++i) {
                    float dx = std::abs(currentPoints[i].x - delaunay->points[i].x);
                    float dy = std::abs(currentPoints[i].y - delaunay->points[i].y);
                    if (dx > 0.0001f || dy > 0.0001f) {
                        needsUpdate = true;
                        break;
                    }
                }
            } else {
                needsUpdate = true;
            }

            if (needsUpdate || delaunay->needsRegeneration) {
                this->_updateDelaunayFromControlPoints(id, *delaunay);
                delaunay->needsRegeneration = false;
            }
        }

        ParametricCurve* curve = this->_registry.getComponent<ParametricCurve>(id);
        if (curve && !curve->controlPointEntities.empty()) {
            bool needsUpdate = false;
            std::vector<glm::vec3> currentPoints;
            currentPoints.reserve(curve->controlPointEntities.size());

            Transform* curveTransform = this->_registry.getComponent<Transform>(id);
            glm::mat4 curveInverseMatrix = glm::mat4(1.0f);

            if (curveTransform) {
                curveInverseMatrix = glm::inverse(curveTransform->globalMatrix);
            }

            for (EntityID pointId : curve->controlPointEntities) {
                Transform* transform = this->_registry.getComponent<Transform>(pointId);
                if (transform) {
                    glm::vec3 globalPos = glm::vec3(transform->globalMatrix[3]);

                    glm::vec4 localPos4 = curveInverseMatrix * glm::vec4(globalPos, 1.0f);
                    glm::vec3 localPos = glm::vec3(localPos4);

                    currentPoints.push_back(localPos);
                }
            }

            if (currentPoints.size() == curve->controlPoints.size()) {
                for (size_t i = 0; i < currentPoints.size(); ++i) {
                    float dx = std::abs(currentPoints[i].x - curve->controlPoints[i].x);
                    float dy = std::abs(currentPoints[i].y - curve->controlPoints[i].y);
                    float dz = std::abs(currentPoints[i].z - curve->controlPoints[i].z);
                    if (dx > 0.0001f || dy > 0.0001f || dz > 0.0001f) {
                        needsUpdate = true;
                        break;
                    }
                }
            } else {
                needsUpdate = true;
            }

            if (needsUpdate || curve->needsRegeneration) {
                this->_updateCurveFromControlPoints(id, *curve);
                curve->needsRegeneration = false;
            }
        }
    }
}

ofMesh PrimitiveSystem::_generateBoxMesh(const glm::vec3& dims)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    float halfWidth = dims.x * 0.5f;
    float halfHeight = dims.y * 0.5f;
    float halfDeep = dims.z * 0.5f;

    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, 0, 1));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, 0, -1));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addVertex(glm::vec3( halfWidth, -halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight,  halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(1, 0, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth,  halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth,  halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(-1, 0, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, 1, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, -1, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    for (int i = 0; i < 6; ++i) {
        int offset = i * 4;
        mesh.addIndex(offset + 0);
        mesh.addIndex(offset + 1);
        mesh.addIndex(offset + 2);
        mesh.addIndex(offset + 0);
        mesh.addIndex(offset + 2);
        mesh.addIndex(offset + 3);
    }

    return mesh;
}

ofMesh PrimitiveSystem::_generateSphereMesh(float radius)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    const int rings = 20;
    const int sectors = 20;
    const float M_PI_CUSTOM = 3.14159265359f;

    for (int ring = 0; ring <= rings; ++ring) {
        float theta = ring * M_PI_CUSTOM / rings;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int sector = 0; sector <= sectors; ++sector) {
            float phi = sector * 2.0f * M_PI_CUSTOM / sectors;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            mesh.addVertex(glm::vec3(x * radius, y * radius, z * radius));
            mesh.addNormal(glm::vec3(x, y, z));

            float u = (float)sector / (float)sectors;
            float v = (float)ring / (float)rings;
            mesh.addTexCoord(glm::vec2(u, v));
        }
    }

    for (int ring = 0; ring < rings; ++ring) {
        for (int sector = 0; sector < sectors; ++sector) {
            int current = ring * (sectors + 1) + sector;
            int next = current + sectors + 1;

            mesh.addIndex(current);
            mesh.addIndex(next);
            mesh.addIndex(current + 1);

            mesh.addIndex(current + 1);
            mesh.addIndex(next);
            mesh.addIndex(next + 1);
        }
    }

    return mesh;
}

ofMesh PrimitiveSystem::_generatePlaneMesh(const glm::vec2& size)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    float halfW = size.x * 0.5f;
    float halfH = size.y * 0.5f;

    mesh.addVertex(glm::vec3(-halfW, -halfH, 0.0f));
    mesh.addVertex(glm::vec3(halfW, -halfH, 0.0f));
    mesh.addVertex(glm::vec3(halfW, halfH, 0.0f));
    mesh.addVertex(glm::vec3(-halfW, halfH, 0.0f));

    glm::vec3 normal(0.0f, 0.0f, 1.0f);
    mesh.addNormal(normal);
    mesh.addNormal(normal);
    mesh.addNormal(normal);
    mesh.addNormal(normal);

    mesh.addTexCoord(glm::vec2(0, 1));
    mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0));
    mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(2);
    mesh.addIndex(0);
    mesh.addIndex(2);
    mesh.addIndex(3);

    return mesh;
}

ofMesh PrimitiveSystem::_generateTriangleMesh(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    mesh.addVertex(v1);
    mesh.addVertex(v2);
    mesh.addVertex(v3);

    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    mesh.addNormal(normal);
    mesh.addNormal(normal);
    mesh.addNormal(normal);

    mesh.addTexCoord(glm::vec2(0.5f, 0.0f));
    mesh.addTexCoord(glm::vec2(0.0f, 1.0f));
    mesh.addTexCoord(glm::vec2(1.0f, 1.0f));

    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(2);

    return mesh;
}

ofMesh PrimitiveSystem::_generateCircleMesh(float radius, int segments)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    mesh.addVertex(glm::vec3(0.0f, 0.0f, 0.0f));
    mesh.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));
    mesh.addTexCoord(glm::vec2(0.5f, 0.5f));

    const float pi_f = 3.14159265359f;
    for (int i = 0; i <= segments; i++) {
        float angle = (2.0f * pi_f * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        mesh.addVertex(glm::vec3(x, y, 0.0f));
        mesh.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));

        float u = 0.5f + 0.5f * std::cos(angle);
        float v = 0.5f + 0.5f * std::sin(angle);
        mesh.addTexCoord(glm::vec2(u, v));

        mesh.addIndex(0);
        if (i > 0) {
            mesh.addIndex(i);
            mesh.addIndex(i + 1);
        }
    }

    return mesh;
}

ofMesh PrimitiveSystem::_generateLineMesh(const glm::vec3& start, const glm::vec3& end)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINES);

    mesh.addVertex(start);
    mesh.addVertex(end);

    glm::vec3 direction = glm::normalize(end - start);
    mesh.addNormal(direction);
    mesh.addNormal(direction);

    mesh.addTexCoord(glm::vec2(0, 0));
    mesh.addTexCoord(glm::vec2(1, 1));

    mesh.addIndex(0);
    mesh.addIndex(1);

    return mesh;
}

ofMesh PrimitiveSystem::_generateRectangleMesh(float width, float height)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    float halfW = width * 0.5f;
    float halfH = height * 0.5f;

    mesh.addVertex(glm::vec3(-halfW, -halfH, 0.0f));
    mesh.addVertex(glm::vec3(halfW, -halfH, 0.0f));
    mesh.addVertex(glm::vec3(halfW, halfH, 0.0f));
    mesh.addVertex(glm::vec3(-halfW, halfH, 0.0f));

    glm::vec3 normal(0.0f, 0.0f, 1.0f);
    mesh.addNormal(normal);
    mesh.addNormal(normal);
    mesh.addNormal(normal);
    mesh.addNormal(normal);

    mesh.addTexCoord(glm::vec2(0, 1));
    mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0));
    mesh.addTexCoord(glm::vec2(0, 0));

    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(2);
    mesh.addIndex(0);
    mesh.addIndex(2);
    mesh.addIndex(3);

    return mesh;
}

ofMesh PrimitiveSystem::_generatePointMesh(float size)
{
    ofMesh mesh = ofMesh::sphere(size * 0.05f, 8);

    int numVertices = mesh.getNumVertices();
    mesh.clearTexCoords();
    for (int i = 0; i < numVertices; i++) {
        mesh.addTexCoord(glm::vec2(0.5f, 0.5f));
    }

    return mesh;
}

ofMesh PrimitiveSystem::_generateDelaunayMesh(const DelaunayMesh& delaunay, EntityID entityId)
{
    ofMesh mesh;

    std::vector<glm::vec2> points;

    switch (delaunay.mode) {
        case DelaunayMesh::GenerationMode::RANDOM:
            points = this->_generateRandomPoints(delaunay.numRandomPoints, delaunay.bounds, delaunay.seed);
            break;

        case DelaunayMesh::GenerationMode::GRID:
            points = this->_generateGridPoints(delaunay.gridResolution, delaunay.bounds, delaunay.gridPerturbation, delaunay.seed);
            break;

        case DelaunayMesh::GenerationMode::CUSTOM:
            points = delaunay.points;
            break;
    }

    if (points.size() < 3) {
        return mesh;
    }

    if (DelaunayMesh* delaunayMut = this->_registry.getComponent<DelaunayMesh>(entityId)) {
        delaunayMut->generatedPoints = points;
    }

    Delaunay delaunayAlgo;

    if (delaunay.displayMode == DelaunayMesh::DisplayMode::DELAUNAY_ONLY ||
        delaunay.displayMode == DelaunayMesh::DisplayMode::BOTH) {
        std::vector<Triangle2D> triangles = delaunayAlgo.triangulate(points);
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);

        for (const auto& triangle : triangles) {
            mesh.addVertex(glm::vec3(triangle.p1.x, triangle.p1.y, 0.0f));
            mesh.addVertex(glm::vec3(triangle.p2.x, triangle.p2.y, 0.0f));
            mesh.addVertex(glm::vec3(triangle.p3.x, triangle.p3.y, 0.0f));

            glm::vec3 normal(0.0f, 0.0f, 1.0f);
            mesh.addNormal(normal);
            mesh.addNormal(normal);
            mesh.addNormal(normal);

            float uScale = 1.0f / delaunay.bounds.x;
            float vScale = 1.0f / delaunay.bounds.y;

            mesh.addTexCoord(glm::vec2(
                (triangle.p1.x + delaunay.bounds.x * 0.5f) * uScale,
                (triangle.p1.y + delaunay.bounds.y * 0.5f) * vScale
            ));
            mesh.addTexCoord(glm::vec2(
                (triangle.p2.x + delaunay.bounds.x * 0.5f) * uScale,
                (triangle.p2.y + delaunay.bounds.y * 0.5f) * vScale
            ));
            mesh.addTexCoord(glm::vec2(
                (triangle.p3.x + delaunay.bounds.x * 0.5f) * uScale,
                (triangle.p3.y + delaunay.bounds.y * 0.5f) * vScale
            ));

            int baseIndex = mesh.getNumVertices() - 3;
            mesh.addIndex(baseIndex);
            mesh.addIndex(baseIndex + 1);
            mesh.addIndex(baseIndex + 2);
        }
    }

    if (delaunay.displayMode == DelaunayMesh::DisplayMode::VORONOI_ONLY ||
        delaunay.displayMode == DelaunayMesh::DisplayMode::BOTH) {
        std::vector<VoronoiCell> cells = delaunayAlgo.computeVoronoi(points);

        if (delaunay.displayMode == DelaunayMesh::DisplayMode::VORONOI_ONLY) {
            mesh.setMode(OF_PRIMITIVE_LINES);
        }

        for (const auto& cell : cells) {
            if (cell.vertices.size() < 2) continue;

            ofColor cellColor = this->_generateColorFromPosition(cell.site);

            for (size_t i = 0; i < cell.vertices.size(); ++i) {
                const glm::vec2& v1 = cell.vertices[i];
                const glm::vec2& v2 = cell.vertices[(i + 1) % cell.vertices.size()];

                mesh.addVertex(glm::vec3(v1.x, v1.y, 0.01f));
                mesh.addVertex(glm::vec3(v2.x, v2.y, 0.01f));

                mesh.addColor(cellColor);
                mesh.addColor(cellColor);

                glm::vec3 normal(0.0f, 0.0f, 1.0f);
                mesh.addNormal(normal);
                mesh.addNormal(normal);

                mesh.addTexCoord(glm::vec2(0.5f, 0.5f));
                mesh.addTexCoord(glm::vec2(0.5f, 0.5f));

                if (delaunay.displayMode == DelaunayMesh::DisplayMode::VORONOI_ONLY) {
                    int baseIdx = mesh.getNumVertices() - 2;
                    mesh.addIndex(baseIdx);
                    mesh.addIndex(baseIdx + 1);
                }
            }
        }
    }

    return mesh;
}

std::vector<glm::vec2> PrimitiveSystem::_generateRandomPoints(int count, const glm::vec2& bounds, unsigned int seed)
{
    std::vector<glm::vec2> points;
    points.reserve(count);

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> distX(-bounds.x * 0.5f, bounds.x * 0.5f);
    std::uniform_real_distribution<float> distY(-bounds.y * 0.5f, bounds.y * 0.5f);

    for (int i = 0; i < count; ++i) {
        points.push_back(glm::vec2(distX(rng), distY(rng)));
    }

    return points;
}

std::vector<glm::vec2> PrimitiveSystem::_generateGridPoints(int resolution, const glm::vec2& bounds, float perturbation, unsigned int seed)
{
    std::vector<glm::vec2> points;
    points.reserve(resolution * resolution);

    std::mt19937 rng(seed);
    float cellWidth = bounds.x / (resolution - 1);
    float cellHeight = bounds.y / (resolution - 1);
    float maxPerturbX = cellWidth * perturbation * 0.5f;
    float maxPerturbY = cellHeight * perturbation * 0.5f;

    std::uniform_real_distribution<float> distX(-maxPerturbX, maxPerturbX);
    std::uniform_real_distribution<float> distY(-maxPerturbY, maxPerturbY);

    for (int row = 0; row < resolution; ++row) {
        for (int col = 0; col < resolution; ++col) {
            float x = -bounds.x * 0.5f + col * cellWidth;
            float y = -bounds.y * 0.5f + row * cellHeight;

            x += distX(rng);
            y += distY(rng);

            points.push_back(glm::vec2(x, y));
        }
    }

    return points;
}

ofColor PrimitiveSystem::_generateColorFromPosition(const glm::vec2& pos)
{
    constexpr float HASH_X = 12.9898f;
    constexpr float HASH_Y = 78.233f;
    constexpr float HASH_SCALE = 43758.5453123f;
    constexpr float HUE_SCALE = 360.0f;
    constexpr float SAT_BASE = 0.6f;
    constexpr float SAT_RANGE = 0.3f;
    constexpr float VAL_BASE = 0.7f;
    constexpr float VAL_RANGE = 0.3f;

    float hue = std::fmod(std::fmod(pos.x * HASH_X + pos.y * HASH_Y, 1.0f) * HASH_SCALE, HUE_SCALE);
    float sat = SAT_BASE + std::fmod(pos.x * 23.142f + pos.y * 45.632f, 1.0f) * SAT_RANGE;
    float val = VAL_BASE + std::fmod(pos.x * 34.521f + pos.y * 67.854f, 1.0f) * VAL_RANGE;

    float c = val * sat;
    float x = c * (1.0f - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));
    float m = val - c;

    float r, g, b;
    if (hue < 60.0f) { r = c; g = x; b = 0.0f; }
    else if (hue < 120.0f) { r = x; g = c; b = 0.0f; }
    else if (hue < 180.0f) { r = 0.0f; g = c; b = x; }
    else if (hue < 240.0f) { r = 0.0f; g = x; b = c; }
    else if (hue < 300.0f) { r = x; g = 0.0f; b = c; }
    else { r = c; g = 0.0f; b = x; }

    return ofColor((r + m) * 255.0f, (g + m) * 255.0f, (b + m) * 255.0f);
}

void PrimitiveSystem::_updateDelaunayFromControlPoints(EntityID delaunayId, DelaunayMesh& delaunay)
{
    // Gather current positions of control points
    std::vector<glm::vec2> points;
    points.reserve(delaunay.controlPointEntities.size());

    // Get the Delaunay's transform to convert global positions to local space
    Transform* delaunayTransform = this->_registry.getComponent<Transform>(delaunayId);
    glm::mat4 delaunayInverseMatrix = glm::mat4(1.0f);

    if (delaunayTransform) {
        delaunayInverseMatrix = glm::inverse(delaunayTransform->globalMatrix);
    }

    for (EntityID pointId : delaunay.controlPointEntities) {
        Transform* transform = this->_registry.getComponent<Transform>(pointId);
        if (transform) {
            // Get global position
            glm::vec3 globalPos = glm::vec3(transform->globalMatrix[3]);

            // Convert to Delaunay's local space
            glm::vec4 localPos4 = delaunayInverseMatrix * glm::vec4(globalPos, 1.0f);
            glm::vec3 localPos = glm::vec3(localPos4);

            points.push_back(glm::vec2(localPos.x, localPos.y));
        }
    }

    if (points.size() < 3) {
        return;
    }

    // Update the DelaunayMesh component with new points (in local space)
    delaunay.points = points;

    // Regenerate the mesh
    Renderable* render = this->_registry.getComponent<Renderable>(delaunayId);
    if (render) {
        render->mesh = this->_generateDelaunayMesh(delaunay, delaunayId);
    }
}

// ==================== PARAMETRIC CURVES ====================

ofMesh PrimitiveSystem::_generateParametricCurveMesh(const ParametricCurve& curve, EntityID entityId)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);

    if (curve.controlPoints.size() < 3) {
        return mesh;
    }

    std::vector<glm::vec3> curvePoints;

    // Generate curve points based on type
    if (curve.type == ParametricCurve::Type::BEZIER_CUBIC) {
        if (curve.controlPoints.size() >= 4) {
            curvePoints = BezierCurve::generateCurve(curve.controlPoints, curve.resolution);
        }
    } else if (curve.type == ParametricCurve::Type::CATMULL_ROM) {
        if (curve.controlPoints.size() >= 4) {
            curvePoints = CatmullRomCurve::generateCurve(curve.controlPoints, curve.resolution);
        }
    }

    // Add points to mesh
    for (const auto& point : curvePoints) {
        mesh.addVertex(point);
        mesh.addNormal(glm::vec3(0.0f, 1.0f, 0.0f));
        mesh.addTexCoord(glm::vec2(0.0f, 0.0f));
    }

    return mesh;
}

void PrimitiveSystem::_updateCurveFromControlPoints(EntityID curveId, ParametricCurve& curve)
{
    // Gather current positions of control points
    std::vector<glm::vec3> points;
    points.reserve(curve.controlPointEntities.size());

    // Get the Curve's transform to convert global positions to local space
    Transform* curveTransform = this->_registry.getComponent<Transform>(curveId);
    glm::mat4 curveInverseMatrix = glm::mat4(1.0f);

    if (curveTransform) {
        curveInverseMatrix = glm::inverse(curveTransform->globalMatrix);
    }

    for (EntityID pointId : curve.controlPointEntities) {
        Transform* transform = this->_registry.getComponent<Transform>(pointId);
        if (transform) {
            // Get global position
            glm::vec3 globalPos = glm::vec3(transform->globalMatrix[3]);

            // Convert to Curve's local space
            glm::vec4 localPos4 = curveInverseMatrix * glm::vec4(globalPos, 1.0f);
            glm::vec3 localPos = glm::vec3(localPos4);

            points.push_back(localPos);
        }
    }

    int minPoints = (curve.type == ParametricCurve::Type::BEZIER_CUBIC) ? 4 : 4;
    if (points.size() < static_cast<size_t>(minPoints)) {
        return;
    }

    // Update the curve component with new points (in local space)
    curve.controlPoints = points;

    // Regenerate the mesh
    Renderable* render = this->_registry.getComponent<Renderable>(curveId);
    if (render) {
        render->mesh = this->_generateParametricCurveMesh(curve, curveId);
    }
}

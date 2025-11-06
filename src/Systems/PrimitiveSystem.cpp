#include "Systems/PrimitiveSystem.hpp"
#include <cmath>

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
    }
}

ofMesh PrimitiveSystem::_generateBoxMesh(const glm::vec3& dims)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    float halfWidth = dims.x * 0.5f;
    float halfHeight = dims.y * 0.5f;
    float halfDeep = dims.z * 0.5f;

    // Front face (+Z)
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, 0, 1));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    // Back face (-Z)
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, 0, -1));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    // Right face (+X)
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight,  halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(1, 0, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    // Left face (-X)
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth,  halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth,  halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(-1, 0, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    // Top face (+Y)
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, 1, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    // Bottom face (-Y)
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, halfDeep));
    for (int i = 0; i < 4; i++) mesh.addNormal(glm::vec3(0, -1, 0));
    mesh.addTexCoord(glm::vec2(0, 1)); mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(1, 0)); mesh.addTexCoord(glm::vec2(0, 0));

    // Indices for all 6 faces
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

    // Generate vertices with proper UV coordinates
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

            // Proper spherical UV mapping
            float u = (float)sector / (float)sectors;
            float v = (float)ring / (float)rings;
            mesh.addTexCoord(glm::vec2(u, v));
        }
    }

    // Generate indices
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

    // Proper UV mapping for plane
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

    // UV mapping for triangle (3 corners)
    mesh.addTexCoord(glm::vec2(0.5f, 0.0f));  // Top
    mesh.addTexCoord(glm::vec2(0.0f, 1.0f));  // Bottom left
    mesh.addTexCoord(glm::vec2(1.0f, 1.0f));  // Bottom right

    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(2);

    return mesh;
}

ofMesh PrimitiveSystem::_generateCircleMesh(float radius, int segments)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    // Center vertex
    mesh.addVertex(glm::vec3(0.0f, 0.0f, 0.0f));
    mesh.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));
    mesh.addTexCoord(glm::vec2(0.5f, 0.5f)); // Center of texture

    const float pi_f = 3.14159265359f;
    for (int i = 0; i <= segments; i++) {
        float angle = (2.0f * pi_f * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        mesh.addVertex(glm::vec3(x, y, 0.0f));
        mesh.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));

        // UV coordinates from center (0.5, 0.5) to edge
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

    // Simple UV for line (not really used for lines but good to have)
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

    // Proper UV mapping for rectangle
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
    // Render point as a small sphere for visibility
    ofMesh mesh = ofMesh::sphere(size * 0.05f, 8);

    // Generate simple UV coords for the point sphere
    int numVertices = mesh.getNumVertices();
    mesh.clearTexCoords();
    for (int i = 0; i < numVertices; i++) {
        mesh.addTexCoord(glm::vec2(0.5f, 0.5f)); // Center texture on point
    }

    return mesh;
}

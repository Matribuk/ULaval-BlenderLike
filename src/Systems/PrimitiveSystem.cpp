#include "Systems/PrimitiveSystem.hpp"
#include <cmath>

PrimitiveSystem::PrimitiveSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void PrimitiveSystem::generateMeshes() {
    for (EntityID id : this->_entityManager.getAllEntities()) {
        if (Box* box = this->_registry.getComponent<Box>(id)) {
            if (Renderable* render = this->_registry.getComponent<Renderable>(id))
                render->mesh = this->_generateBoxMesh(box->dimensions);
        }
        if (Sphere* sphere = this->_registry.getComponent<Sphere>(id)) {
            if (Renderable* render = this->_registry.getComponent<Renderable>(id))
                render->mesh = this->_generateSphereMesh(sphere->radius);
        }
        if (Plane* plane = this->_registry.getComponent<Plane>(id)) {
            if (Renderable* render = this->_registry.getComponent<Renderable>(id))
                render->mesh = this->_generatePlaneMesh(plane->size);
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
    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(0, 0, 1));

    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(0, 0, -1));

    mesh.addVertex(glm::vec3( halfWidth, -halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight,  halfDeep));
    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(1, 0, 0));

    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth,  halfHeight,  halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth,  halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(-1, 0, 0));


    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, halfHeight, -halfDeep));
    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(0, 1, 0));

    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, -halfDeep));
    mesh.addVertex(glm::vec3( halfWidth, -halfHeight, halfDeep));
    mesh.addVertex(glm::vec3(-halfWidth, -halfHeight, halfDeep));
    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(0, -1, 0));

    for (int i = 0; i < 6; ++i) {
        int offset = i * 4;
        mesh.addIndex(offset + 0);
        mesh.addIndex(offset + 1);
        mesh.addIndex(offset + 2);
        mesh.addIndex(offset + 0);
        mesh.addIndex(offset + 2);
        mesh.addIndex(offset + 3);
    }
    this->_generateDefaultTexCoords(mesh, "box");

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

    this->_generateDefaultTexCoords(mesh, "sphere");

    return mesh;
}

ofMesh PrimitiveSystem::_generatePlaneMesh(const glm::vec2& size)
{
    ofMesh mesh = ofMesh::plane(size.x, size.y, 2, 2);
    this->_generateDefaultTexCoords(mesh, "box");
    return mesh;
}

void PrimitiveSystem::_generateDefaultTexCoords(ofMesh& mesh, const std::string& mode)
{
    const float pi_f = 3.14159265359f;
    auto verts = mesh.getVertices();
    if (verts.empty()) return;

    mesh.clearTexCoords();

    if (mode == "sphere") {
        for (auto &v : verts) {
            glm::vec3 n = glm::normalize(v);
            float u = 0.5f + (std::atan2(n.z, n.x) / (2.0f * pi_f));
            float vcoord = 0.5f - (std::asin(n.y) / pi_f);
            mesh.addTexCoord(glm::vec2(u, vcoord));
        }
        return;
    }

    if (mode == "box") {
        glm::vec3 minb(FLT_MAX, FLT_MAX, FLT_MAX), maxb(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto &v : verts) {
            minb.x = std::min(minb.x, v.x);
            minb.y = std::min(minb.y, v.y);
            minb.z = std::min(minb.z, v.z);
            maxb.x = std::max(maxb.x, v.x);
            maxb.y = std::max(maxb.y, v.y);
            maxb.z = std::max(maxb.z, v.z);
        }
    
        float spanX = maxb.x - minb.x;
        float spanZ = maxb.z - minb.z;
        if (spanX <= 0.f) spanX = 1.f;
        if (spanZ <= 0.f) spanZ = 1.f;
    
        for (auto &v : verts) {
            float u = (v.x - minb.x) / spanX;
            float vcoord = (v.z - minb.z) / spanZ;
            mesh.addTexCoord(ofVec2f(u, vcoord));
        }
        return;
    }
}

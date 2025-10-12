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

    return mesh;
}

ofMesh PrimitiveSystem::_generatePlaneMesh(const glm::vec2& size)
{
    return ofMesh::plane(size.x, size.y, 2, 2);
}

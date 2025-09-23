#include "PrimitiveSystem.hpp"

PrimitiveSystem::PrimitiveSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void PrimitiveSystem::generateMeshes() {
    for (EntityID id : _entityManager.getAllEntities()) {
        if (Box* box = _registry.getComponent<Box>(id)) {
            if (Renderable* render = _registry.getComponent<Renderable>(id))
                render->mesh = generateBoxMesh(box->dimensions);
        }
        if (Sphere* sphere = _registry.getComponent<Sphere>(id)) {
            if (Renderable* render = _registry.getComponent<Renderable>(id))
                render->mesh = generateSphereMesh(sphere->radius);
        }
        if (Plane* plane = _registry.getComponent<Plane>(id)) {
            if (Renderable* render = _registry.getComponent<Renderable>(id))
                render->mesh = generatePlaneMesh(plane->size);
        }
    }
}

ofMesh PrimitiveSystem::generateBoxMesh(const glm::vec3& dims)
{
    return ofMesh::box(dims.x, dims.y, dims.z, 2, 2, 2);
}

ofMesh PrimitiveSystem::generateSphereMesh(float radius)
{
    return ofMesh::sphere(radius, 12);
}

ofMesh PrimitiveSystem::generatePlaneMesh(const glm::vec2& size)
{
    return ofMesh::plane(size.x, size.y, 2, 2);
}

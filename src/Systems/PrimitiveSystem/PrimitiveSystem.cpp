#include "Systems/PrimitiveSystem/PrimitiveSystem.hpp"

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
    return ofMesh::box(dims.x, dims.y, dims.z, 2, 2, 2);
}

ofMesh PrimitiveSystem::_generateSphereMesh(float radius)
{
    return ofMesh::sphere(radius, 12);
}

ofMesh PrimitiveSystem::_generatePlaneMesh(const glm::vec2& size)
{
    return ofMesh::plane(size.x, size.y, 2, 2);
}

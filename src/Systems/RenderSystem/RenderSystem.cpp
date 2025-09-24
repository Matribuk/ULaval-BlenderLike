#include "RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void RenderSystem::render()
{
    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        Renderable* render = this->_registry.getComponent<Renderable>(id);
        if (transform && render && render->visible)
            drawMesh(render->mesh, transform->matrix, render->color);
    }
}

void RenderSystem::drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color)
{
    ofPushMatrix();
    ofMultMatrix(transform);
    ofSetColor(color);
    mesh.draw();
    ofPopMatrix();
}
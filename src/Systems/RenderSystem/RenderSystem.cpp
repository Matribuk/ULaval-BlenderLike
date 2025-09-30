#include "RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void RenderSystem::render()
{
    Camera* activeCamera = getActiveCameraObject();
    if (!activeCamera) return;

    ofCamera cam;
    cam.setPosition(activeCamera->position);
    cam.lookAt(activeCamera->target, activeCamera->up);
    cam.setFov(activeCamera->fov);
    cam.setNearClip(activeCamera->nearClip);
    cam.setFarClip(activeCamera->farClip);
    cam.setAspectRatio(activeCamera->aspectRatio);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    ofEnableDepthTest();

    cam.begin();

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        Renderable* render = this->_registry.getComponent<Renderable>(id);

        if (!transform || !render || !render->visible) continue;

        drawMesh(render->mesh, transform->matrix, render->color, render->material);
    }

    cam.end();
}
void RenderSystem::setActiveCamera(EntityID cameraEntity)
{
    if (this->_registry.hasComponent<Camera>(cameraEntity)) {
        this->_activeCamera = cameraEntity;
    } else {
        std::cout << "Entity " << cameraEntity << " does not have a camera component." << std::endl;
        this->_activeCamera = INVALID_ENTITY;
    }
}

Camera* RenderSystem::getActiveCameraObject() const
{
    if (this->_activeCamera == INVALID_ENTITY) return nullptr;
    return this->_registry.getComponent<Camera>(this->_activeCamera);
}

EntityID RenderSystem::getActiveCameraId() const
{
    return this->_activeCamera;
}

void RenderSystem::drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material *material)
{
    ofPushMatrix();
    ofMultMatrix(transform);
    ofSetColor(color);

    if (material && material->shader) {
        material->shader->begin();

        material->shader->setUniformMatrix4f("modelMatrix", transform);
        if (material->texture)
            material->shader->setUniformTexture("tex0", *material->texture, 0);

        mesh.draw();

        material->shader->end();
    } else {
        mesh.draw();
    }
    ofPopMatrix();
}
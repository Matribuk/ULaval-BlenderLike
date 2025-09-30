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

        const auto& verts = render->mesh.getVertices();
        if (verts.empty()) continue;

        glm::vec3 vmin(std::numeric_limits<float>::max());
        glm::vec3 vmax(-std::numeric_limits<float>::max());

        for (const auto &v : verts) {
            glm::vec4 worldV4 = transform->matrix * glm::vec4(v.x, v.y, v.z, 1.0f);
            glm::vec3 worldV(worldV4.x, worldV4.y, worldV4.z);
            vmin.x = std::min(vmin.x, worldV.x);
            vmin.y = std::min(vmin.y, worldV.y);
            vmin.z = std::min(vmin.z, worldV.z);
            vmax.x = std::max(vmax.x, worldV.x);
            vmax.y = std::max(vmax.y, worldV.y);
            vmax.z = std::max(vmax.z, worldV.z);
        }

        glm::vec3 center = (vmin + vmax) * 0.5f;
        glm::vec3 size = vmax - vmin;

        glm::vec3 screenCenter = cam.worldToScreen(center);
        glm::vec3 offsetWorld = center + glm::vec3(size.x * 0.5f, 0, 0);
        glm::vec3 screenOffset = cam.worldToScreen(offsetWorld);
        float radiusScreen = glm::distance(screenCenter, screenOffset);

        if (screenCenter.x + radiusScreen < 0 || screenCenter.x - radiusScreen > ofGetWidth() ||
            screenCenter.y + radiusScreen < 0 || screenCenter.y - radiusScreen > ofGetHeight()) continue;

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
        _activeCamera = INVALID_ENTITY;
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
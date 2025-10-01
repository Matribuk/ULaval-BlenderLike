#include "RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void RenderSystem::render()
{
    Camera* activeCamera = getActiveCameraObject();
    if (!activeCamera) return;

    Transform* camTransform = _registry.getComponent<Transform>(_activeCamera);
    if (!camTransform) {
        std::cout << "Active camera has no Transform component" << std::endl;
        return;
    }

    ofCamera cam = buildCameraFromComponents(*activeCamera, *camTransform);

    setupRenderState();

    cam.begin();

    renderEntities(cam);

    cam.end();
}

void RenderSystem::setupRenderState()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    ofEnableDepthTest();
}

ofCamera RenderSystem::buildCameraFromComponents(Camera& camera, const Transform& transform)
{
    glm::vec3 camPos = glm::vec3(transform.matrix[3]);
    glm::vec3 forward = glm::normalize(glm::vec3(transform.matrix * glm::vec4(0, 0, -1, 0)));
    glm::vec3 up = glm::normalize(glm::vec3(transform.matrix * glm::vec4(0, 1, 0, 0)));

    glm::vec3 lookTarget = (camera.focusMode) ? camera.target : camPos + forward;

    camera.viewMatrix = glm::lookAt(camPos, lookTarget, up);
    if (camera.aspectRatio <= 0.0f) camera.aspectRatio = static_cast<float>(ofGetWidth()) / static_cast<float>(ofGetHeight());
    camera.projMatrix = glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearClip, camera.farClip);

    ofCamera cam;
    cam.setPosition(camPos);
    cam.lookAt(lookTarget, up);
    cam.setFov(camera.fov);
    cam.setNearClip(camera.nearClip);
    cam.setFarClip(camera.farClip);
    cam.setAspectRatio(camera.aspectRatio);

    return cam;
}

void RenderSystem::renderEntities(ofCamera& cam)
{
    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = _registry.getComponent<Transform>(id);
        Renderable* render   = _registry.getComponent<Renderable>(id);

        if (!transform || !render || !render->visible) continue;

        drawMesh(render->mesh, transform->matrix, render->color, render->material);
    }
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
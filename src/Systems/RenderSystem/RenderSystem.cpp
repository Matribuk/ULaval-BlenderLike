#include "RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr, CameraManager &cameraManager)
    : _registry(registry), _entityManager(entityMgr), _cameraManager(cameraManager) {}

void RenderSystem::render()
{
    Camera* activeCamera = this->_cameraManager.getActiveCamera();
    EntityID activeCameraId = this->_cameraManager.getActiveCameraId();
    if (activeCameraId == INVALID_ENTITY || !activeCamera) return;

    Transform* camTransform = _registry.getComponent<Transform>(_activeCamera);
    if (!camTransform) {
        std::cout << "Active camera has no Transform component" << std::endl;
        return;
    }

    ofCamera cam = buildCameraFromComponents(*activeCamera, *camTransform);

    setupRenderState();

    cam.begin();

    ofSetColor(255);
    ofDrawAxis(100);

    ofPushStyle();
    ofSetColor(100);
    for (int i = -10; i <= 10; ++i) {
        ofDrawLine(-10, 0, i, 10, 0, i);
        ofDrawLine(i, 0, -10, i, 0, 10);
    }
    ofPopStyle();

    renderEntities();

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
    glm::vec3 camPos = transform.position;
    glm::vec3 forward = glm::normalize(camera.forward);
    glm::vec3 up = glm::normalize(-camera.up);

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

void RenderSystem::renderEntities()
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

        Camera* activeCam = this->_cameraManager.getActiveCamera();
        if (activeCam) {
            material->shader->setUniformMatrix4f("viewMatrix", activeCam->viewMatrix);
            material->shader->setUniformMatrix4f("projMatrix", activeCam->projMatrix);
        }

        if (material->texture)
            material->shader->setUniformTexture("tex0", *material->texture, 0);

        mesh.draw();

        material->shader->end();
    } else {
        mesh.draw();
    }
    ofPopMatrix();
}
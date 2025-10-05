#include "RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr, CameraSystem &camSystem)
    : _registry(registry), _entityManager(entityMgr), _cameraSystem(camSystem) {}

void RenderSystem::render()
{
    Camera* activeCamera = this->_cameraSystem.getActiveCamera();
    EntityID activeCameraId = this->_cameraSystem.getActiveCameraId();
    if (activeCameraId == INVALID_ENTITY) return;

    Transform* camTransform = this->_registry.getComponent<Transform>(activeCameraId);
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
    glm::vec3 camPos = transform.position;
    glm::vec3 forward = glm::normalize(camera.forward);
    glm::vec3 up = glm::normalize(camera.up);

    glm::vec3 lookTarget = (camera.focusMode ? camera.target : camPos + forward);

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
        Transform* transform = this->_registry.getComponent<Transform>(id);
        Renderable* render   = this->_registry.getComponent<Renderable>(id);

        if (!transform || !render || !render->visible) continue;

        drawMesh(render->mesh, transform->matrix, render->color, render->material);
    }
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

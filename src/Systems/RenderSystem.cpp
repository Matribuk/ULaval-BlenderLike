#include "Systems/RenderSystem.hpp"
#include <glm/gtc/type_ptr.hpp>

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr)
{
    this->_initSkybox();
}

void RenderSystem::render()
{
    Camera* activeCamera = this->_cameraManager->getActiveCamera();
    EntityID activeCameraId = this->_cameraManager->getActiveCameraId();
    if (activeCameraId == INVALID_ENTITY || !activeCamera) return;

    Transform* camTransform = this->_registry.getComponent<Transform>(activeCameraId);
    if (!camTransform) {
        std::cout << "Active camera has no Transform component" << std::endl;
        return;
    }

    this->_setupRenderState();
    this->_renderSkybox();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(activeCamera->projMatrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(activeCamera->viewMatrix));

    ofSetColor(255);
    ofDrawAxis(100);

    ofPushStyle();
    ofSetColor(100);
    for (int i = -10; i <= 10; ++i) {
        ofDrawLine(-10, 0, i, 10, 0, i);
        ofDrawLine(i, 0, -10, i, 0, 10);
    }
    ofPopStyle();

    this->_renderEntities();
}

void RenderSystem::setSkyColors(const glm::vec3& top, const glm::vec3& bottom)
{
    _skyTopColor = top;
    _skyBottomColor = bottom;
}

void RenderSystem::_setupRenderState()
{
    glDisable(GL_CULL_FACE);
}

ofCamera RenderSystem::_buildCameraFromComponents(Camera& camera, const Transform& transform)
{
    glm::vec3 camPos = transform.position;
    glm::vec3 forward = glm::normalize(camera.forward);
    glm::vec3 up = glm::normalize(-camera.up);

    glm::vec3 lookTarget = (camera.focusMode) ? (camera.target) : (camPos + forward);

    camera.viewMatrix = glm::lookAt(camPos, lookTarget, up);
    if (camera.aspectRatio <= 0.0f) camera.aspectRatio = static_cast<float>(ofGetWidth()) / static_cast<float>(ofGetHeight());
    camera.projMatrix = glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearClip, camera.farClip);

    ofCamera cam;
    cam.setPosition(camPos);
    cam.lookAt(lookTarget, up);
    cam.setNearClip(camera.nearClip);
    cam.setFarClip(camera.farClip);
    cam.setAspectRatio(camera.aspectRatio);

    if (camera.isOrtho) {
        float halfWidth = camera.orthoScale * camera.aspectRatio;
        float halfHeight = camera.orthoScale;
        camera.projMatrix = glm::ortho(
            -halfWidth, halfWidth,
            -halfHeight, halfHeight,
            camera.nearClip,
            camera.farClip
        );

        cam.enableOrtho();
        cam.setupPerspective(false);
    } else {
        camera.projMatrix = glm::perspective(
            glm::radians(camera.fov),
            camera.aspectRatio,
            camera.nearClip,
            camera.farClip
        );

        cam.disableOrtho();
        cam.setFov(camera.fov);
    }

    return cam;
}

void RenderSystem::_renderEntities()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem->getSelectedEntities();

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        Renderable* render   = this->_registry.getComponent<Renderable>(id);

        if (!transform || !render || !render->visible) continue;

        bool isSelected = (selectedEntities.find(id) != selectedEntities.end());
        this->_drawMesh(render->mesh, transform->matrix, render->color, render->material, isSelected);
    }
}

void RenderSystem::_drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material* material, bool isSelected)
{
    ofPushMatrix();
    ofMultMatrix(transform);
    ofSetColor(color);

    if (material && material->shader) {
        material->shader->begin();

        material->shader->setUniformMatrix4f("modelMatrix", transform);

        Camera* activeCam = this->_cameraManager->getActiveCamera();
        if (activeCam) {
            material->shader->setUniformMatrix4f("viewMatrix", activeCam->viewMatrix);
            material->shader->setUniformMatrix4f("projMatrix", activeCam->projMatrix);
        }
        material->shader->setUniform4f("color", ofFloatColor(color));
        material->shader->setUniform1f("uTime", ofGetElapsedTimef());

        if (material->texture)
            material->shader->setUniformTexture("tex0", *material->texture, 0);

        mesh.draw();

        material->shader->end();
    } else if (material && material->texture && !material->shader) {
        material->texture->bind();
        mesh.draw();
        material->texture->unbind();
    } else {
        mesh.draw();
    }

    if (isSelected) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(ofColor::yellow);
        ofSetLineWidth(this->_boundingBoxSize);

        ofScale(1.01f, 1.01f, 1.01f);
        mesh.drawWireframe();

        ofPopStyle();
    }
    ofPopMatrix();
}

void RenderSystem::_initSkybox()
{
    bool loaded = this->_skyShader.load("shaders/sky");

    if (!loaded) {
        std::cout << "[SKYBOX] Sky shader failed to load" << std::endl;
        return;
    }

    this->_skyQuad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    this->_skyQuad.addVertex(glm::vec3(-1, -1, 0.999));
    this->_skyQuad.addVertex(glm::vec3( 1, -1, 0.999));
    this->_skyQuad.addVertex(glm::vec3(-1,  1, 0.999));
    this->_skyQuad.addVertex(glm::vec3( 1,  1, 0.999));

    this->_skyInitialized = true;
}

void RenderSystem::_renderSkybox()
{
    if (!_skyInitialized)
        return;

    glClearColor(this->_skyBottomColor.r, this->_skyBottomColor.g, this->_skyBottomColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    this->_skyShader.begin();

    this->_skyShader.setUniform3f("topColor", this->_skyTopColor);
    this->_skyShader.setUniform3f("bottomColor", this->_skyBottomColor);

    this->_skyQuad.draw();
    this->_skyShader.end();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::setup(CameraManager& cameraManager, SelectionSystem& selectionSystem)
{
    this->_cameraManager = &cameraManager;
    this->_selectionSystem = &selectionSystem;
}

#include "Systems/RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr)
{
    this->_initSkybox();
    this->loadCubemap("cubemaps/parc");
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
    this->_renderSkyboxCubemap();

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
    } else if (material && material->texture) {
        material->texture->bind();
        mesh.draw();
        material->texture->unbind();
    } else
        mesh.draw();

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
    bool cubeLoaded = this->_skyCubeShader.load("shaders/skycube");
    if (!cubeLoaded) {
        std::cout << "[SKYBOX] Skycube shader failed to load" << std::endl;
        return;
    }

    this->_skyQuad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    this->_skyQuad.addVertex(glm::vec3(-1, -1, 0.999));
    this->_skyQuad.addVertex(glm::vec3( 1, -1, 0.999));
    this->_skyQuad.addVertex(glm::vec3(-1,  1, 0.999));
    this->_skyQuad.addVertex(glm::vec3( 1,  1, 0.999));

    this->_skyInitialized = true;
}

void RenderSystem::_renderSkyboxCubemap()
{
    if (!this->_skyInitialized || !this->_skyboxCubemap.isLoaded()) return;

    Camera* activeCamera = this->_cameraManager->getActiveCamera();
    if (!activeCamera) return;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    this->_skyCubeShader.begin();

    glm::mat4 invProj = glm::inverse(activeCamera->projMatrix);
    glm::mat4 invView = glm::inverse(glm::mat4(glm::mat3(activeCamera->viewMatrix)));

    this->_skyCubeShader.setUniformMatrix4f("inverseProjectionMatrix", invProj);
    this->_skyCubeShader.setUniformMatrix4f("inverseViewMatrix", invView);

    this->_skyboxCubemap.bind(0);
    this->_skyCubeShader.setUniform1i("cubeMap", 0);

    this->_skyQuad.draw();

    this->_skyboxCubemap.unbind();
    this->_skyCubeShader.end();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::loadCubemap(const std::string& folderPath)
{
    bool success = this->_skyboxCubemap.loadFromFolder(folderPath);
    if (!success) std::cout << "[SKYBOX] Failed to load cubemap from: " << folderPath << std::endl;
}

void RenderSystem::setup(CameraManager& cameraManager, SelectionSystem& selectionSystem)
{
    this->_cameraManager = &cameraManager;
    this->_selectionSystem = &selectionSystem;
}

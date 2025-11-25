#include "Systems/RenderSystem.hpp"

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr)
{
    this->_initSkybox();
    this->loadCubemap("cubemaps/parc");
    this->_initWhiteTexture();
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
    glEnable(GL_NORMALIZE);
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
        this->_drawMesh(render->mesh, transform->matrix, render->color, render->material, false);

        if (isSelected) {
            BoundingBoxVisualization* bboxVis = this->_registry.getComponent<BoundingBoxVisualization>(id);

            if (!bboxVis) {
                BoundingBoxVisualization defaultBBox;
                defaultBBox.type = BoundingBoxVisualization::Type::AABB;
                defaultBBox.visible = true;
                defaultBBox.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
                this->_drawBoundingBox(id, *transform, defaultBBox);
            } else if (bboxVis->type != BoundingBoxVisualization::Type::NONE) {
                this->_drawBoundingBox(id, *transform, *bboxVis);
            } else {
                BoundingBoxVisualization defaultBBox;
                defaultBBox.type = BoundingBoxVisualization::Type::AABB;
                defaultBBox.visible = true;
                defaultBBox.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
                this->_drawBoundingBox(id, *transform, defaultBBox);
            }
        }
    }
}

void RenderSystem::_drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material* material, bool isSelected)
{
    if (!material) {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);
        mesh.draw();
        ofPopMatrix();
        return;
    }

    material->shaderPipeline.clear();
    if (material->illuminationShader)
        material->shaderPipeline.push_back(material->illuminationShader);
    if (material->shader)
        material->shaderPipeline.push_back(material->shader);

    if (material->shaderPipeline.size() > 1)
        this->_drawMeshMultiPass(mesh, transform, color, material);

    else if (material->illuminationShader) {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);

        this->_drawMeshSinglePass(mesh, transform, color, material, material->illuminationShader, true);

        ofPopMatrix();
    } else if (material->shader) {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);

        this->_drawMeshSinglePass(mesh, transform, color, material, material->shader, false);

        ofPopMatrix();
    } else {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);

        if (material->texture) {
            material->texture->bind();
            mesh.draw();
            material->texture->unbind();
        } else
            mesh.draw();

        ofPopMatrix();
    }
}

void RenderSystem::_drawMeshSinglePass(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material* material, ofShader* shader, bool isIllumination)
{
    if (!shader) return;

    shader->begin();

    shader->setUniformMatrix4f("modelMatrix", transform);

    Camera* activeCam = this->_cameraManager->getActiveCamera();
    EntityID activeCameraId = this->_cameraManager->getActiveCameraId();
    if (activeCam) {
        shader->setUniformMatrix4f("viewMatrix", activeCam->viewMatrix);
        shader->setUniformMatrix4f("projMatrix", activeCam->projMatrix);

        Transform* camTransform = this->_registry.getComponent<Transform>(activeCameraId);
        if (camTransform)
            shader->setUniform3f("cameraPosition", camTransform->position);
    }

    if (isIllumination) {
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        shader->setUniformMatrix3f("normalMatrix", normalMatrix);

        shader->setUniform3f("lightPosition", material->lightPosition);
        shader->setUniform3f("lightColor", material->lightColor);
        shader->setUniform1f("lightIntensity", material->lightIntensity);
        shader->setUniform3f("ambientColor", material->ambientColor);
        shader->setUniform1f("shininess", material->shininess);

        shader->setUniform3f("ambientReflection", material->ambientReflection);
        shader->setUniform3f("diffuseReflection", material->diffuseReflection);
        shader->setUniform3f("specularReflection", material->specularReflection);
        shader->setUniform3f("emissiveReflection", material->emissiveReflection);
    }

    shader->setUniform4f("color", ofFloatColor(color));
    shader->setUniform1f("uTime", ofGetElapsedTimef());

    bool hasTexture = (material->texture != nullptr);
    shader->setUniform1i("hasTexture", hasTexture ? 1 : 0);

    if (material->texture)
        shader->setUniformTexture("tex0", *material->texture, 0);
    else
        shader->setUniformTexture("tex0", this->_whiteTexture, 0);

    glEnableClientState(GL_NORMAL_ARRAY);
    mesh.draw();
    glDisableClientState(GL_NORMAL_ARRAY);

    shader->end();
}
void RenderSystem::_drawMeshMultiPass(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color, Material* material)
{
    if (material->shaderPipeline.empty()) return;

    Camera* activeCam = this->_cameraManager->getActiveCamera();
    EntityID activeCameraId = this->_cameraManager->getActiveCameraId();

    ofPushMatrix();
    ofMultMatrix(transform);
    ofSetColor(color);

    glEnable(GL_BLEND);

    for (size_t i = 0; i < material->shaderPipeline.size(); ++i) {
        ofShader* shader = material->shaderPipeline[i];
        bool isIllumination = (shader == material->illuminationShader);

        if (i == 0) {
            glBlendFunc(GL_ONE, GL_ZERO);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
        } else {
            glBlendFunc(GL_ONE, GL_ONE);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }

        shader->begin();

        shader->setUniformMatrix4f("modelMatrix", transform);

        if (activeCam) {
            shader->setUniformMatrix4f("viewMatrix", activeCam->viewMatrix);
            shader->setUniformMatrix4f("projMatrix", activeCam->projMatrix);

            Transform* camTransform = this->_registry.getComponent<Transform>(activeCameraId);
            if (camTransform)
                shader->setUniform3f("cameraPosition", camTransform->position);
        }

        if (isIllumination) {
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
            shader->setUniformMatrix3f("normalMatrix", normalMatrix);

            shader->setUniform3f("lightPosition", material->lightPosition);
            shader->setUniform3f("lightColor", material->lightColor);
            shader->setUniform1f("lightIntensity", material->lightIntensity);
            shader->setUniform3f("ambientColor", material->ambientColor);
            shader->setUniform1f("shininess", material->shininess);

            // Réflections matérielles de base
            shader->setUniform3f("ambientReflection", material->ambientReflection);
            shader->setUniform3f("diffuseReflection", material->diffuseReflection);
            shader->setUniform3f("specularReflection", material->specularReflection);
            shader->setUniform3f("emissiveReflection", material->emissiveReflection);
            
            // Support des réflections miroir/cubemap
            shader->setUniform1f("reflectivity", material->reflectivity);
            shader->setUniform3f("reflectionTint", material->reflectionTint);
        }

        shader->setUniform4f("color", ofFloatColor(color));
        shader->setUniform1f("uTime", ofGetElapsedTimef());

        bool hasTexture = (material->texture != nullptr);
        shader->setUniform1i("hasTexture", hasTexture ? 1 : 0);

        if (material->texture)
            shader->setUniformTexture("tex0", *material->texture, 0);
        else
            shader->setUniformTexture("tex0", this->_whiteTexture, 0);

        // Support de la cubemap d'environnement
        bool hasEnvMap = false;
        GLint envMapLoc = glGetUniformLocation(shader->getProgram(), "envMap");
        if (envMapLoc != -1 && this->_skyboxCubemap.isLoaded()) {
            this->_skyboxCubemap.bind(1);
            shader->setUniform1i("envMap", 1);
            hasEnvMap = true;
        }

        glEnableClientState(GL_NORMAL_ARRAY);
        mesh.draw();
        glDisableClientState(GL_NORMAL_ARRAY);

        shader->end();
        
        // Nettoyage des textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (hasEnvMap) {
            glActiveTexture(GL_TEXTURE1);
            this->_skyboxCubemap.unbind();
            glActiveTexture(GL_TEXTURE0);
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

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
    if (!success)
        ofLogError("RenderSystem") << "Failed to load cubemap from: " << folderPath;
}

void RenderSystem::_initWhiteTexture()
{
    ofPixels pixels;
    pixels.allocate(1, 1, OF_PIXELS_RGB);
    pixels.setColor(0, 0, ofColor::white);
    this->_whiteTexture.allocate(1, 1, GL_RGB);
    this->_whiteTexture.loadData(pixels);
}

void RenderSystem::setup(CameraManager& cameraManager, SelectionSystem& selectionSystem)
{
    this->_cameraManager = &cameraManager;
    this->_selectionSystem = &selectionSystem;
}

void RenderSystem::_drawBoundingBox(EntityID entityId, const Transform& transform, const BoundingBoxVisualization& bboxVis)
{
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofFloatColor(bboxVis.color.r, bboxVis.color.g, bboxVis.color.b, bboxVis.color.a));
    ofSetLineWidth(2.0f);

    glm::vec3 localMin(0.0f), localMax(0.0f);
    bool hasBounds = false;

    Renderable* renderable = this->_registry.getComponent<Renderable>(entityId);
    if (renderable && renderable->mesh.getNumVertices() > 0) {
        const auto& vertices = renderable->mesh.getVertices();
        glm::vec3 minVert(std::numeric_limits<float>::max());
        glm::vec3 maxVert(std::numeric_limits<float>::lowest());

        for (const auto& v : vertices) {
            minVert.x = std::min(minVert.x, v.x);
            minVert.y = std::min(minVert.y, v.y);
            minVert.z = std::min(minVert.z, v.z);
            maxVert.x = std::max(maxVert.x, v.x);
            maxVert.y = std::max(maxVert.y, v.y);
            maxVert.z = std::max(maxVert.z, v.z);
        }

        localMin = minVert;
        localMax = maxVert;
        hasBounds = true;
    }
    else if (CustomBounds* customBounds = this->_registry.getComponent<CustomBounds>(entityId)) {
        localMin = customBounds->min;
        localMax = customBounds->max;
        hasBounds = true;
    }

    if (!hasBounds) {
        ofPopStyle();
        return;
    }

    switch (bboxVis.type) {
        case BoundingBoxVisualization::Type::AABB: {
            glm::vec3 center = (localMin + localMax) * 0.5f;
            glm::vec3 size = localMax - localMin;

            ofPushMatrix();
            ofMultMatrix(transform.matrix);
            ofDrawBox(center, size.x, size.y, size.z);
            ofPopMatrix();
            break;
        }

        case BoundingBoxVisualization::Type::SPHERE: {
            glm::vec3 center = (localMin + localMax) * 0.5f;
            float radius = glm::length(localMax - center);

            ofPushMatrix();
            ofMultMatrix(transform.matrix);
            ofDrawSphere(center, radius);
            ofPopMatrix();
            break;
        }

        case BoundingBoxVisualization::Type::NONE:
        default:
            break;
    }

    ofPopStyle();
}

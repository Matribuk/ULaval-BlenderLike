#include "Systems/RenderSystem.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Plane.hpp"

struct RaytracingPrimitive {
    int type;
    glm::vec3 position;
    glm::vec3 param1;
    glm::vec3 param2;
    glm::vec4 color;
    float reflectivity;
};

RenderSystem::RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr)
{
    this->_initSkybox();
    this->loadCubemap("cubemaps/parc");
    this->_initWhiteTexture();
    this->_initShadowSystem();
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

    if (this->_shadowsEnabled && this->_shadowSystemInitialized) {
        std::vector<LightSource> lights;
        this->_collectLights(lights);
        this->_updateShadowMaps(lights);
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_NORMALIZE);
}

void RenderSystem::_renderEntities()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem->getSelectedEntities();

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        if (!transform) continue;

        Renderable* render = this->_registry.getComponent<Renderable>(id);
        bool isSelected = (selectedEntities.find(id) != selectedEntities.end());

        if (render && render->visible)
            this->_drawMesh(render->mesh, transform->matrix, render->color, render->material, false);

        LightSource* light = this->_registry.getComponent<LightSource>(id);
        if (light) {
            if (light->type == LightType::DIRECTIONAL || light->type == LightType::SPOT) {
                if (light->enabled) {
                    glm::mat4 lightTransform = glm::translate(glm::mat4(1.0f), transform->position);
                    this->_drawLightDirectionIndicator(*light, lightTransform);
                }
            }
        }

        if (isSelected && render) {
            BoundingBoxVisualization* bboxVis = this->_registry.getComponent<BoundingBoxVisualization>(id);

            if (!bboxVis) {
                BoundingBoxVisualization defaultBBox;
                defaultBBox.type = BoundingBoxVisualization::Type::AABB;
                defaultBBox.visible = true;
                defaultBBox.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
                this->_drawBoundingBox(id, *transform, defaultBBox);
            } else if (bboxVis->type != BoundingBoxVisualization::Type::NONE)
                this->_drawBoundingBox(id, *transform, *bboxVis);
            else {
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
    for (ofShader* effect : material->effects) material->shaderPipeline.push_back(effect);

    if (material->shaderPipeline.size() > 1)
        this->_drawMeshMultiPass(mesh, transform, color, material);
    else if (material->illuminationShader) {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);

        this->_drawMeshSinglePass(mesh, transform, color, material, material->illuminationShader, true);

        ofPopMatrix();
    } else if (!material->effects.empty()) {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);

        this->_drawMeshSinglePass(mesh, transform, color, material, material->effects[0], false);

        ofPopMatrix();
    } else {
        ofPushMatrix();
        ofMultMatrix(transform);
        ofSetColor(color);

        if (material->texture) {
            material->texture->bind();
            mesh.draw();
            material->texture->unbind();
        } else mesh.draw();

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
        if (camTransform) shader->setUniform3f("cameraPosition", camTransform->position);
    }

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    shader->setUniformMatrix3f("normalMatrix", normalMatrix);

    if (isIllumination) {
        std::vector<LightSource> lights;
        this->_collectLights(lights);
        this->_setLightUniforms(shader, lights);
        this->_setShadowUniforms(shader, lights);

        this->_setPrimitiveUniforms(shader);
        this->_setRaytracingUniforms(shader);

        shader->setUniform3f("ambientColor", material->ambientColor);
        shader->setUniform1f("shininess", material->shininess);

        shader->setUniform3f("ambientReflection", material->ambientReflection);
        shader->setUniform3f("diffuseReflection", material->diffuseReflection);
        shader->setUniform3f("specularReflection", material->specularReflection);
        shader->setUniform3f("emissiveReflection", material->emissiveReflection);

        shader->setUniform1f("reflectivity", material->reflectivity);
        shader->setUniform3f("reflectionTint", material->reflectionTint);
    }

    shader->setUniform4f("color", ofFloatColor(color));
    shader->setUniform1f("uTime", ofGetElapsedTimef());

    bool hasTexture = (material->texture != nullptr);
    shader->setUniform1i("hasTexture", hasTexture ? 1 : 0);

    if (material->texture) shader->setUniformTexture("tex0", *material->texture, 0);
    else shader->setUniformTexture("tex0", this->_whiteTexture, 0);

    if (this->_skyboxCubemap.isLoaded()) {
        this->_skyboxCubemap.bind(1);
        shader->setUniform1i("envMap", 1);
    }

    if (material->normalMap) {
        shader->setUniformTexture("normalMap", *material->normalMap, 2);
        shader->setUniform1i("useNormalMap", 1);
        shader->setUniform1f("normalStrength", material->normalStrength);
    } else
        shader->setUniform1i("useNormalMap", 0);

    glEnableClientState(GL_NORMAL_ARRAY);
    mesh.draw();
    glDisableClientState(GL_NORMAL_ARRAY);

    shader->end();

    if (this->_skyboxCubemap.isLoaded()) {
        this->_skyboxCubemap.unbind();
    }
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
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
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

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        shader->setUniformMatrix3f("normalMatrix", normalMatrix);

        std::vector<LightSource> lights;
        this->_collectLights(lights);
        this->_setLightUniforms(shader, lights);
        this->_setShadowUniforms(shader, lights);

        shader->setUniform3f("ambientColor", material->ambientColor);
        shader->setUniform1f("shininess", material->shininess);
        shader->setUniform3f("ambientReflection", material->ambientReflection);
        shader->setUniform3f("diffuseReflection", material->diffuseReflection);
        shader->setUniform3f("specularReflection", material->specularReflection);
        shader->setUniform3f("emissiveReflection", material->emissiveReflection);

        if (isIllumination) {
            shader->setUniform1f("reflectivity", material->reflectivity);
            shader->setUniform3f("reflectionTint", material->reflectionTint);
        }

        shader->setUniform4f("color", ofFloatColor(color));
        shader->setUniform1f("uTime", ofGetElapsedTimef());

        bool hasTexture = (material->texture != nullptr);
        shader->setUniform1i("hasTexture", hasTexture ? 1 : 0);

        if (material->texture)
            shader->setUniformTexture("tex0", *material->texture, 0);
        else shader->setUniformTexture("tex0", this->_whiteTexture, 0);

        bool hasEnvMap = false;
        GLint envMapLoc = glGetUniformLocation(shader->getProgram(), "envMap");
        if (envMapLoc != -1 && this->_skyboxCubemap.isLoaded()) {
            this->_skyboxCubemap.bind(1);
            shader->setUniform1i("envMap", 1);
            hasEnvMap = true;
        }

        if (material->normalMap) {
            shader->setUniformTexture("normalMap", *material->normalMap, 2);
            shader->setUniform1i("useNormalMap", 1);
            shader->setUniform1f("normalStrength", material->normalStrength);
        } else shader->setUniform1i("useNormalMap", 0);

        glEnableClientState(GL_NORMAL_ARRAY);
        mesh.draw();
        glDisableClientState(GL_NORMAL_ARRAY);

        shader->end();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (hasEnvMap) {
            glActiveTexture(GL_TEXTURE1);
            this->_skyboxCubemap.unbind();
        }

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE0);
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
            glm::vec3 halfExtents = localMax - center;
            float radius = glm::max(glm::max(halfExtents.x, halfExtents.y), halfExtents.z);

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

void RenderSystem::_collectLights(std::vector<LightSource>& lights)
{
    lights.clear();

    for (EntityID id : this->_entityManager.getAllEntities()) {
        LightSource* light = this->_registry.getComponent<LightSource>(id);
        if (light && light->enabled) {
            Transform* transform = this->_registry.getComponent<Transform>(id);
            if (transform) light->position = transform->position;

            lights.push_back(*light);
        }

        Renderable* renderable = this->_registry.getComponent<Renderable>(id);
        if (renderable && renderable->material && renderable->material->isLightSource) {
            float emissiveIntensity = (renderable->material->emissiveReflection.x +
                                       renderable->material->emissiveReflection.y +
                                       renderable->material->emissiveReflection.z) / 3.0f;

            if (emissiveIntensity > 0.0f) {
                Transform* transform = this->_registry.getComponent<Transform>(id);
                glm::vec3 lightPos(0.0f);
                if (transform) lightPos = transform->position;

                LightSource emissiveLight(LightType::POINT, lightPos,
                                          renderable->material->emissiveReflection,
                                          emissiveIntensity);
                lights.push_back(emissiveLight);
            }
        }
    }

}

void RenderSystem::_setLightUniforms(ofShader* shader, const std::vector<LightSource>& lights)
{
    int numLights = std::min(static_cast<int>(lights.size()), 8);
    shader->setUniform1i("numLights", numLights);

    for (int i = 0; i < numLights; ++i) {
        const LightSource& light = lights[i];

        std::string indexStr = "[" + std::to_string(i) + "]";

        shader->setUniform1i("lightTypes" + indexStr, static_cast<int>(light.type));
        shader->setUniform3f("lightPositions" + indexStr, light.position);
        shader->setUniform3f("lightDirections" + indexStr, light.direction);
        shader->setUniform3f("lightColors" + indexStr, light.color);
        shader->setUniform1f("lightIntensities" + indexStr, light.intensity);
        shader->setUniform1f("lightSpotAngles" + indexStr, light.spotAngle);
        shader->setUniform1f("lightAttenuations" + indexStr, light.attenuation);
    }
}

void RenderSystem::_drawLightDirectionIndicator(const LightSource& light, const glm::mat4& transform)
{
    ofPushMatrix();
    ofMultMatrix(transform);

    glm::vec3 lightColor = light.color * 255.0f;
    ofSetColor(lightColor.x, lightColor.y, lightColor.z);
    ofSetLineWidth(2.0f);

    glm::vec3 direction = glm::normalize(light.direction);
    if (glm::length(direction) < 0.01f) direction = glm::vec3(0, -1, 0);

    float arrowLength = 3.0f;
    float arrowHeadSize = 0.3f;

    if (light.type == LightType::DIRECTIONAL) {
        for (int i = 0; i < 3; i++) {
            float offset = (i - 1) * 0.3f;

            glm::vec3 start(offset, 0, 0);
            glm::vec3 end = start + direction * arrowLength;

            ofDrawLine(start, end);

            glm::vec3 perpX(arrowHeadSize, 0, 0);
            glm::vec3 perpY(0, arrowHeadSize, 0);

            ofDrawLine(end, end - direction * arrowHeadSize + perpX);
            ofDrawLine(end, end - direction * arrowHeadSize - perpX);
        }
    } else if (light.type == LightType::SPOT) {
        float coneAngle = glm::radians(glm::clamp(light.spotAngle, 1.0f, 89.0f));
        float radius = tan(coneAngle) * arrowLength;
        int numArrows = 8;

        glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));
        if (glm::length(right) < 0.01f) right = glm::normalize(glm::cross(direction, glm::vec3(1, 0, 0)));
        glm::vec3 up = glm::normalize(glm::cross(right, direction));

        for (int i = 0; i < numArrows; i++) {
            float angle = (i / (float)numArrows) * TWO_PI;

            glm::vec3 offset = (right * cos(angle) + up * sin(angle)) * radius;
            glm::vec3 endPos = direction * arrowLength + offset;

            ofDrawLine(glm::vec3(0, 0, 0), endPos);

            glm::vec3 arrowDir = glm::normalize(endPos);
            glm::vec3 perpX = glm::normalize(glm::cross(arrowDir, up)) * arrowHeadSize;

            ofDrawLine(endPos, endPos - arrowDir * arrowHeadSize + perpX);
            ofDrawLine(endPos, endPos - arrowDir * arrowHeadSize - perpX);
        }
    }

    ofPopMatrix();
}

void RenderSystem::_initShadowSystem()
{
    if (this->_shadowSystemInitialized) return;

    this->_depthShader.load("shaders/depth.vert", "shaders/depth.frag");

    if (!this->_depthShader.isLoaded()) {
        ofLogError("RenderSystem") << "Failed to load depth shader for shadow mapping";
        this->_shadowsEnabled = false;
        return;
    }

    ofLogNotice("RenderSystem") << "Shadow mapping system initialized";
    this->_shadowSystemInitialized = true;
}

void RenderSystem::setShadowMapResolution(int resolution)
{
    if (resolution < 512 || resolution > 8192) {
        ofLogWarning("RenderSystem") << "Shadow map resolution must be between 512 and 8192";
        return;
    }

    this->_shadowMapResolution = resolution;

    for (auto& shadowMap : this->_shadowMaps) {
        shadowMap.fbo.clear();
        ofFbo::Settings settings;
        settings.width = this->_shadowMapResolution;
        settings.height = this->_shadowMapResolution;
        settings.useDepth = true;
        settings.depthStencilAsTexture = true;
        settings.textureTarget = GL_TEXTURE_2D;
        settings.internalformat = GL_RGBA;
        settings.minFilter = GL_LINEAR;
        settings.maxFilter = GL_LINEAR;
        settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
        settings.wrapModeVertical = GL_CLAMP_TO_EDGE;

        shadowMap.fbo.allocate(settings);
    }
}

void RenderSystem::_updateShadowMaps(const std::vector<LightSource>& lights)
{
    if (!this->_shadowsEnabled || !this->_shadowSystemInitialized) return;

    this->_shadowMaps.clear();

    for (size_t i = 0; i < lights.size(); ++i) {
        const LightSource& light = lights[i];

        if (!light.enabled) continue;
        if (light.type == LightType::AMBIENT) continue;

        ShadowMap shadowMap;
        shadowMap.lightIndex = i;

        ofFbo::Settings settings;
        settings.width = this->_shadowMapResolution;
        settings.height = this->_shadowMapResolution;
        settings.useDepth = true;
        settings.depthStencilAsTexture = true;
        settings.textureTarget = GL_TEXTURE_2D;
        settings.internalformat = GL_RGBA;
        settings.minFilter = GL_LINEAR;
        settings.maxFilter = GL_LINEAR;
        settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
        settings.wrapModeVertical = GL_CLAMP_TO_EDGE;

        shadowMap.fbo.allocate(settings);

        shadowMap.lightSpaceMatrix = this->_computeLightSpaceMatrix(light);

        this->_renderSceneToShadowMap(shadowMap);

        this->_shadowMaps.push_back(shadowMap);
    }
}

glm::mat4 RenderSystem::_computeLightSpaceMatrix(const LightSource& light)
{
    glm::mat4 lightProjection;
    glm::mat4 lightView;

    if (light.type == LightType::DIRECTIONAL) {
        float shadowVolume = 50.0f;
        lightProjection = glm::ortho(
            -shadowVolume, shadowVolume,
            -shadowVolume, shadowVolume,
            -shadowVolume, shadowVolume * 2.0f
        );

        glm::vec3 lightPos = -light.direction * shadowVolume;
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        if (glm::abs(glm::dot(light.direction, up)) > 0.99f) {
            up = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        lightView = glm::lookAt(lightPos, target, up);
    }
    else if (light.type == LightType::SPOT) {
        float fov = light.spotAngle * 2.0f;
        lightProjection = glm::perspective(
            glm::radians(fov),
            1.0f,
            0.1f,
            100.0f
        );

        glm::vec3 target = light.position + light.direction;
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        if (glm::abs(glm::dot(light.direction, up)) > 0.99f) {
            up = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        lightView = glm::lookAt(light.position, target, up);
    }
    else if (light.type == LightType::POINT) {
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        lightProjection = glm::perspective(
            glm::radians(90.0f),
            1.0f,
            nearPlane,
            farPlane
        );

        glm::vec3 target = light.position + glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
        lightView = glm::lookAt(light.position, target, up);
    }

    return lightProjection * lightView;
}

void RenderSystem::_renderSceneToShadowMap(ShadowMap& shadowMap)
{
    shadowMap.fbo.begin();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    this->_depthShader.begin();

    this->_depthShader.setUniformMatrix4f("lightViewMatrix", shadowMap.lightSpaceMatrix);
    this->_depthShader.setUniformMatrix4f("lightProjMatrix", glm::mat4(1.0f));

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        if (!transform) continue;

        Renderable* render = this->_registry.getComponent<Renderable>(id);
        if (!render || !render->visible) continue;

        this->_depthShader.setUniformMatrix4f("modelMatrix", transform->matrix);

        render->mesh.draw();
    }

    this->_depthShader.end();

    shadowMap.fbo.end();
}

void RenderSystem::_setShadowUniforms(ofShader* shader, const std::vector<LightSource>& lights)
{
    if (!this->_shadowsEnabled || this->_shadowMaps.empty()) {
        shader->setUniform1i("numShadowMaps", 0);
        return;
    }

    int numShadowMaps = std::min(static_cast<int>(this->_shadowMaps.size()), 8);
    shader->setUniform1i("numShadowMaps", numShadowMaps);
    shader->setUniform1f("shadowBias", this->_shadowBias);
    shader->setUniform1i("usePCF", this->_usePCF ? 1 : 0);

    for (int i = 0; i < numShadowMaps; ++i) {
        const ShadowMap& shadowMap = this->_shadowMaps[i];

        std::string indexStr = "[" + std::to_string(i) + "]";

        shadowMap.fbo.getDepthTexture().bind(10 + i);
        shader->setUniformTexture("shadowMaps" + indexStr, shadowMap.fbo.getDepthTexture(), 10 + i);

        shader->setUniformMatrix4f("lightSpaceMatrices" + indexStr, shadowMap.lightSpaceMatrix);

        shader->setUniform1i("shadowLightIndices" + indexStr, shadowMap.lightIndex);
    }
}

void RenderSystem::_setPrimitiveUniforms(ofShader* shader) {
    if (!this->_raytracingEnabled) {
        shader->setUniform1i("numPrimitives", 0);
        return;
    }

    std::vector<RaytracingPrimitive> primitives;

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        Renderable* renderable = this->_registry.getComponent<Renderable>(id);

        if (!transform || !renderable || !renderable->visible) continue;

        RaytracingPrimitive prim;
        prim.position = transform->position;
        ofFloatColor col = renderable->color;
        prim.color = glm::vec4(col.r, col.g, col.b, col.a);
        prim.reflectivity = renderable->material ? renderable->material->reflectivity : 0.0f;

        if (Sphere* sphere = this->_registry.getComponent<Sphere>(id)) {
            prim.type = 0;
            prim.param1 = glm::vec3(sphere->radius * transform->scale.x, 0.0f, 0.0f);
            prim.param2 = glm::vec3(0.0f);
            primitives.push_back(prim);
        }
        else if (Box* box = this->_registry.getComponent<Box>(id)) {
            prim.type = 1;
            prim.param1 = box->dimensions * transform->scale;
            prim.param2 = glm::vec3(0.0f);
            primitives.push_back(prim);
        }
        else if (Plane* plane = this->_registry.getComponent<Plane>(id)) {
            prim.type = 2;
            prim.param1 = glm::vec3(plane->size.x * transform->scale.x,
                                    plane->size.y * transform->scale.y, 0.0f);
            prim.param2 = glm::vec3(0.0f, 1.0f, 0.0f);
            primitives.push_back(prim);
        }

        if (primitives.size() >= (size_t)this->_maxPrimitives) break;
    }

    int numPrimitives = std::min(static_cast<int>(primitives.size()), this->_maxPrimitives);
    shader->setUniform1i("numPrimitives", numPrimitives);

    std::vector<int> types(this->_maxPrimitives, 0);
    std::vector<float> positions(this->_maxPrimitives * 3, 0.0f);
    std::vector<float> param1(this->_maxPrimitives * 3, 0.0f);
    std::vector<float> param2(this->_maxPrimitives * 3, 0.0f);
    std::vector<float> colors(this->_maxPrimitives * 4, 0.0f);
    std::vector<float> reflectivities(this->_maxPrimitives, 0.0f);

    for (int i = 0; i < numPrimitives; ++i) {
        types[i] = primitives[i].type;
        positions[i * 3 + 0] = primitives[i].position.x;
        positions[i * 3 + 1] = primitives[i].position.y;
        positions[i * 3 + 2] = primitives[i].position.z;
        param1[i * 3 + 0] = primitives[i].param1.x;
        param1[i * 3 + 1] = primitives[i].param1.y;
        param1[i * 3 + 2] = primitives[i].param1.z;
        param2[i * 3 + 0] = primitives[i].param2.x;
        param2[i * 3 + 1] = primitives[i].param2.y;
        param2[i * 3 + 2] = primitives[i].param2.z;
        colors[i * 4 + 0] = primitives[i].color.r;
        colors[i * 4 + 1] = primitives[i].color.g;
        colors[i * 4 + 2] = primitives[i].color.b;
        colors[i * 4 + 3] = primitives[i].color.a;
        reflectivities[i] = primitives[i].reflectivity;
    }

    shader->setUniform1iv("primitiveTypes", types.data(), this->_maxPrimitives);
    shader->setUniform3fv("primitivePositions", positions.data(), this->_maxPrimitives);
    shader->setUniform3fv("primitiveParam1", param1.data(), this->_maxPrimitives);
    shader->setUniform3fv("primitiveParam2", param2.data(), this->_maxPrimitives);
    shader->setUniform4fv("primitiveColors", colors.data(), this->_maxPrimitives);
    shader->setUniform1fv("primitiveReflectivities", reflectivities.data(), this->_maxPrimitives);
}

void RenderSystem::_setRaytracingUniforms(ofShader* shader) {
    shader->setUniform1i("raytracingEnabled", this->_raytracingEnabled ? 1 : 0);
}

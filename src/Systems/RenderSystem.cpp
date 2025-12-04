#include "RenderSystem.hpp"

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

    if (this->_raytracingEnabled) {
        this->_renderRaytracing();
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

    if (material->texture) shader->setUniformTexture("tex0", *material->texture, 0);
    else shader->setUniformTexture("tex0", this->_whiteTexture, 0);

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

    this->_skyboxSampler.loadFromFolder(folderPath);
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

void RenderSystem::_renderRaytracing()
{
    Camera* activeCamera = this->_cameraManager->getActiveCamera();
    EntityID activeCameraId = this->_cameraManager->getActiveCameraId();
    Transform* camTransform = this->_registry.getComponent<Transform>(activeCameraId);

    if (!activeCamera || !camTransform) return;

    this->_collectSceneLights();
    this->_raytracingCamera.lights = &this->_sceneLights;
    this->_raytracingCamera.skybox = &this->_skyboxSampler;

    HittableList world;
    this->_buildRaytracingScene(world);

    this->_raytracingCamera.aspectRatio = 16.0 / 9.0;
    this->_raytracingCamera.imageWidth = 400;
    this->_raytracingCamera.samplesPerPixel = 4;
    this->_raytracingCamera.maxDepth = 8;

    this->_raytracingCamera.lookFrom = point3(camTransform->position.x, camTransform->position.y, camTransform->position.z);

    glm::vec3 forward = -glm::normalize(glm::vec3(
        activeCamera->viewMatrix[0][2],
        activeCamera->viewMatrix[1][2],
        activeCamera->viewMatrix[2][2]
    ));

    glm::vec3 lookTarget = camTransform->position + forward * 10.0f;
    this->_raytracingCamera.lookAt = point3(lookTarget.x, lookTarget.y, lookTarget.z);

    glm::vec3 up = glm::normalize(glm::vec3(
        activeCamera->viewMatrix[0][1],
        activeCamera->viewMatrix[1][1],
        activeCamera->viewMatrix[2][1]
    ));
    this->_raytracingCamera.vup = Vec3(up.x, up.y, up.z);

    if (!activeCamera->isOrtho) this->_raytracingCamera.vfov = activeCamera->fov;
    else this->_raytracingCamera.vfov = 60.0;

    if (world.objects.empty()) {
        this->_raytracingCamera.render(world, this->_raytracingPixels);
    } else {
        Bvh bvh_tree(world);
        this->_raytracingCamera.render(bvh_tree, this->_raytracingPixels);
    }

    int width = this->_raytracingCamera.imageWidth;
    int height = int(width / this->_raytracingCamera.aspectRatio);

    if (!_raytracingTexture.isAllocated())
        this->_raytracingTexture.allocate(width, height, GL_RGB);

    this->_raytracingTexture.loadData(this->_raytracingPixels.data(), width, height, GL_RGB);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ofGetWidth(), 0, ofGetHeight(), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    this->_raytracingTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
}

void RenderSystem::_buildRaytracingScene(HittableList& world)
{
    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        Renderable* render = this->_registry.getComponent<Renderable>(id);

        if (!transform || !render || !render->visible) continue;

        std::shared_ptr<Materials> mat;

        if (render->material) {
            glm::vec3 diffuse = render->material->diffuseReflection;
            Color diffuseColor(diffuse.r, diffuse.g, diffuse.b);

            diffuseColor = Color(
                diffuseColor.x() * (render->color.r / 255.0),
                diffuseColor.y() * (render->color.g / 255.0),
                diffuseColor.z() * (render->color.b / 255.0)
            );

            glm::vec3 emissive = render->material->emissiveReflection;
            if (emissive.r > 0.01 || emissive.g > 0.01 || emissive.b > 0.01) {
                diffuseColor = Color(
                    std::min(1.0, diffuseColor.x() + emissive.r),
                    std::min(1.0, diffuseColor.y() + emissive.g),
                    std::min(1.0, diffuseColor.z() + emissive.b)
                );
            }

            bool hasHighReflectivity = render->material->reflectivity > 0.1;

            if (hasHighReflectivity) {
                glm::vec3 tint = render->material->reflectionTint;
                Color reflColor(tint.r * diffuseColor.x(), tint.g * diffuseColor.y(), tint.b * diffuseColor.z());
                double fuzz = 1.0 - render->material->reflectivity;
                mat = std::make_shared<Metal>(reflColor, fuzz);
            } else
                mat = std::make_shared<Lambertian>(diffuseColor);
        } else {
            Color albedo(render->color.r / 255.0, render->color.g / 255.0, render->color.b / 255.0);
            mat = std::make_shared<Lambertian>(albedo);
        }

        if (render->mesh.getNumVertices() > 0)
            this->_convertMeshToRaytracing(render->mesh, transform->matrix, mat, world);
    }

}

int RenderSystem::_convertMeshToRaytracing(
    const ofMesh& ofMesh,
    const glm::mat4& transform,
    std::shared_ptr<Materials> mat,
    HittableList& world)
{
    const auto& vertices = ofMesh.getVertices();
    const auto& indices = ofMesh.getIndices();

    if (vertices.size() == 0) return 0;

    auto RtMesh = std::make_shared<Mesh>(mat);
    int triangle_count = 0;

    if (indices.size() > 0) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            if (i + 2 >= indices.size()) break;

            glm::vec3 v0_local = vertices[indices[i]];
            glm::vec3 v1_local = vertices[indices[i + 1]];
            glm::vec3 v2_local = vertices[indices[i + 2]];

            glm::vec4 v0_world = transform * glm::vec4(v0_local, 1.0);
            glm::vec4 v1_world = transform * glm::vec4(v1_local, 1.0);
            glm::vec4 v2_world = transform * glm::vec4(v2_local, 1.0);

            point3 p0(v0_world.x, v0_world.y, v0_world.z);
            point3 p1(v1_world.x, v1_world.y, v1_world.z);
            point3 p2(v2_world.x, v2_world.y, v2_world.z);

            RtMesh->addTriangle(p0, p1, p2);
            triangle_count++;
        }
    } else if (vertices.size() >= 3) {
        for (size_t i = 0; i < vertices.size(); i += 3) {
            if (i + 2 >= vertices.size()) break;

            glm::vec3 v0_local = vertices[i];
            glm::vec3 v1_local = vertices[i + 1];
            glm::vec3 v2_local = vertices[i + 2];

            glm::vec4 v0_world = transform * glm::vec4(v0_local, 1.0);
            glm::vec4 v1_world = transform * glm::vec4(v1_local, 1.0);
            glm::vec4 v2_world = transform * glm::vec4(v2_local, 1.0);

            point3 p0(v0_world.x, v0_world.y, v0_world.z);
            point3 p1(v1_world.x, v1_world.y, v1_world.z);
            point3 p2(v2_world.x, v2_world.y, v2_world.z);

            RtMesh->addTriangle(p0, p1, p2);
            triangle_count++;
        }
    }

    if (triangle_count > 0) {
        RtMesh->buildBVH();
        world.add(RtMesh);
    }

    return triangle_count;
}

void RenderSystem::_collectSceneLights()
{
    this->_sceneLights.clear();

    int light_count = 0;

    for (EntityID id : this->_entityManager.getAllEntities()) {
        LightSource* light = this->_registry.getComponent<LightSource>(id);
        Transform* transform = this->_registry.getComponent<Transform>(id);

        if (light && light->enabled) {
            if (transform) {
                RtLight RtLight(*light, transform->matrix);
                this->_sceneLights.add(RtLight);
            } else {
                RtLight RtLight(*light);
                this->_sceneLights.add(RtLight);
            }

            light_count++;

            std::string type_str;
            switch(light->type) {
                case LightType::AMBIENT: type_str = "AMBIENT"; break;
                case LightType::DIRECTIONAL: type_str = "DIRECTIONAL"; break;
                case LightType::POINT: type_str = "POINT"; break;
                case LightType::SPOT: type_str = "SPOT"; break;
            }
        }
    }
}

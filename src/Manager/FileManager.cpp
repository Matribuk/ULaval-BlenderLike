#include "Manager/FileManager.hpp"
#include "Manager/CameraManager.hpp"
#include "Manager/ViewportManager.hpp"
#include "UI/Viewport.hpp"
#include "Systems/RaycastSystem.hpp"

FileManager::FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager)
    : _componentRegistry(componentRegistry), _entityManager(entityManager) {}

void FileManager::exportMesh(EntityID entity, const std::string& filename)
{
    if (!this->_componentRegistry.hasComponent<Renderable>(entity))
        return;

    Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(entity);
    if (!renderable)
        return;

    try {
        renderable->mesh.save(filename);
    }
    catch (const std::exception& e) {
        return;
    }
}


std::pair<EntityID, std::string> FileManager::_importMesh(const std::string& filename, ResourceManager& resourceManager)
{
    if (FileManager::isImageFile(filename))
        return {INVALID_ENTITY, ""};

    this->_model = std::make_unique<ofxAssimpModelLoader>();

    if (!this->_model->load(filename))
        return {INVALID_ENTITY, ""};

    int numMeshes = this->_model->getMeshCount();

    glm::vec3 min = this->_model->getSceneMin();
    glm::vec3 max = this->_model->getSceneMax();
    glm::vec3 size = max - min;
    glm::vec3 center = (min + max) * 0.5f;
    float maxDim = std::max({size.x, size.y, size.z});

    float targetSize = 5.0f;
    float scaleFactor = (maxDim != 0.f) ? targetSize / maxDim : 1.0f;

    std::filesystem::path path(filename);
    std::string fileName = path.stem().string();

    Entity entity = this->_entityManager.createEntity();
    if (numMeshes > 0) {
        ofMesh mesh = this->_model->getMesh(0);

        for (size_t i = 0; i < mesh.getNumVertices(); i++) {
            glm::vec3 vertex = mesh.getVertex(i);
            vertex = (vertex - center) * scaleFactor;
            mesh.setVertex(i, vertex);
        }

        this->_componentRegistry.registerComponent(entity.getId(), Transform(
            glm::vec3(0),
            glm::vec3(1.0f)
        ));

        glm::vec3 scaledSize = size * scaleFactor;
        this->_componentRegistry.registerComponent(entity.getId(), Box(scaledSize));

        Renderable renderable(mesh, ofColor::white);
        if (renderable.material) {
            renderable.material->illuminationShader = resourceManager.getDefaultIlluminationShader();
        }
        this->_componentRegistry.registerComponent(entity.getId(), renderable);
        this->_componentRegistry.registerComponent(entity.getId(), Selectable());
    }

    return {entity.getId(), fileName};
}

std::shared_ptr<ofTexture> FileManager::_importImageTexture(const std::string& filename)
{
    ofImage image;
    if (!image.load(filename))
        return nullptr;

    std::filesystem::path path(filename);
    std::string fileName = path.stem().string();

    std::shared_ptr<ofTexture> texture = std::make_shared<ofTexture>(image.getTexture());
    return texture;
}

EntityID FileManager::_createImagePlaneEntity(ofTexture& texture, const std::string& name, ResourceManager& resourceManager, const glm::vec3& position)
{
    if (!texture.isAllocated())
        return INVALID_ENTITY;

    float imgWidth = texture.getWidth();
    float imgHeight = texture.getHeight();
    float aspectRatio = imgWidth / imgHeight;
    float targetHeight = 3.0f;
    float targetWidth = targetHeight * aspectRatio;

    ofMesh planeMesh = this->_createImagePlane(targetWidth, targetHeight);
    Entity entity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(entity.getId(), Transform(
        position, glm::vec3(1.0f)
    ));

    Renderable renderable(planeMesh, ofColor::white, true, nullptr, &texture);
    if (renderable.material) {
        renderable.material->illuminationShader = resourceManager.getDefaultIlluminationShader();
    }
    this->_componentRegistry.registerComponent(entity.getId(), renderable);
    this->_componentRegistry.registerComponent(entity.getId(), Selectable());

    return entity.getId();
}

bool FileManager::isImageFile(const std::string& filename)
{
    std::filesystem::path path(filename);
    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return (ext == ".png" || ext == ".jpg");
}

bool FileManager::isModelFile(const std::string& filename)
{
    std::filesystem::path path(filename);
    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return (ext == ".obj" || ext == ".ply" || ext == ".stl");
}

std::pair<std::string, std::string> FileManager::_copyFileToDataFolder(const std::string& sourcePath)
{
    std::filesystem::path srcPath(sourcePath);
    std::string name = srcPath.stem().string();
    std::string ext = srcPath.extension().string();
    std::string filename = srcPath.filename().string();

    std::string dataPath = ofToDataPath("", true);
    if (!dataPath.empty() && dataPath.back() != '/') {
        dataPath += "/";
    }
    std::string destPath = dataPath + filename;

    if (std::filesystem::exists(destPath)) {
        int counter = 1;
        std::string baseName = name;
        name = baseName + "_" + std::to_string(counter);
        filename = name + ext;
        destPath = dataPath + filename;
        while (std::filesystem::exists(destPath)) {
            counter++;
            name = baseName + "_" + std::to_string(counter);
            filename = name + ext;
            destPath = dataPath + filename;
        }
    }

    std::filesystem::copy_file(srcPath, destPath);

    return {destPath, name};
}

bool FileManager::importAndAddAsset(const std::string& filePath, AssetsPanel& assetsPanel, EventLogPanel& eventLog)
{
    eventLog.addLog("File selected: " + filePath, ofColor::aqua);

    std::pair<std::string, std::string> copyResult;
    try {
        copyResult = this->_copyFileToDataFolder(filePath);
    } catch (const std::exception& e) {
        eventLog.addLog("Copy error: " + std::string(e.what()), ofColor::red);
        return false;
    }

    std::string destPath = copyResult.first;
    std::string name = copyResult.second;

    std::filesystem::path path(destPath);
    std::string filename = path.filename().string();
    eventLog.addLog("File copied to data/: " + filename, ofColor::cyan);

    if (FileManager::isImageFile(destPath)) {
        assetsPanel.addImageOrModelAsset(name, destPath, true);
        eventLog.addLog("Image loaded: " + name + " (drag & drop into scene)", ofColor::green);
        return true;
    } else if (FileManager::isModelFile(destPath)) {
        assetsPanel.addImageOrModelAsset(name, destPath, false);
        eventLog.addLog("3D model available: " + name + " (drag & drop into scene)", ofColor::green);
        return true;
    } else {
        std::filesystem::path ext = path.extension();
        eventLog.addLog("Unsupported file type: " + ext.string(), ofColor::red);
        return false;
    }

    return false;
}

void FileManager::handleAssetDrop(
    const AssetDropEvent& event,
    const AssetInfo* asset,
    SceneManager& sceneManager,
    ResourceManager& resourceManager,
    CameraManager& cameraManager,
    ViewportManager& viewportManager,
    EventLogPanel& eventLog
)
{
    if (!asset) return;

    glm::vec3 dropPosition3D = this->_calculate3DPositionFromScreenPos(
        event.dropPosition,
        event.viewportId,
        cameraManager,
        viewportManager
    );

    if (asset->isImage) {
        ofTexture& entityTexture = resourceManager.loadTexture(asset->filepath);
        EntityID entityId = this->_createImagePlaneEntity(
            entityTexture,
            asset->name,
            resourceManager,
            dropPosition3D
        );

        if (entityId != INVALID_ENTITY) {
            sceneManager.registerEntity(entityId, asset->name);
            eventLog.addLog("Plane created: " + asset->name, ofColor::lime);
        }
    } else if (!asset->isImage && !asset->filepath.empty()) {
        std::pair<EntityID, std::string> result = this->_importMesh(asset->filepath, resourceManager);
        if (result.first != INVALID_ENTITY) {
            Transform* transform = this->_componentRegistry.getComponent<Transform>(result.first);
            if (transform) {
                transform->position = dropPosition3D;
                transform->isDirty = true;
            }
            sceneManager.registerEntity(result.first, asset->name);
            eventLog.addLog("3D model created: " + asset->name, ofColor::lime);
        }
    }
}

ofMesh FileManager::_createImagePlane(float width, float height)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    float halfW = width / 2.0f;
    float halfH = height / 2.0f;

    mesh.addVertex(glm::vec3(-halfW, -halfH, 0));
    mesh.addVertex(glm::vec3(halfW, -halfH, 0));
    mesh.addVertex(glm::vec3(-halfW, halfH, 0));
    mesh.addVertex(glm::vec3(halfW, halfH, 0));

    mesh.addTexCoord(glm::vec2(0, 1));
    mesh.addTexCoord(glm::vec2(1, 1));
    mesh.addTexCoord(glm::vec2(0, 0));
    mesh.addTexCoord(glm::vec2(1, 0));

    for (int i = 0; i < 4; i++)
        mesh.addNormal(glm::vec3(0, 0, 1));

    return mesh;
}

glm::vec3 FileManager::_calculate3DPositionFromScreenPos(
    const glm::vec2& screenPos,
    ViewportID viewportId,
    CameraManager& cameraManager,
    ViewportManager& viewportManager
)
{
    Viewport* vp = nullptr;
    for (auto& viewport : viewportManager.getViewports()) {
        if (viewport->getId() == viewportId) {
            vp = viewport.get();
            break;
        }
    }
    if (!vp) vp = viewportManager.getActiveViewport();
    if (!vp) return glm::vec3(0, 0, 0);

    ofRectangle rect = vp->getRect();
    float localX = screenPos.x;
    float localY = screenPos.y;
    int vpWidth = static_cast<int>(rect.getWidth());
    int vpHeight = static_cast<int>(rect.getHeight());

    if (vpWidth <= 0 || vpHeight <= 0) return glm::vec3(0, 0, 0);

    EntityID camEntityId = vp->getCamera();
    if (camEntityId == INVALID_ENTITY) camEntityId = cameraManager.getActiveCameraId();
    if (camEntityId == INVALID_ENTITY) return glm::vec3(0, 0, 0);

    Camera* cam = this->_componentRegistry.getComponent<Camera>(camEntityId);
    Transform* camTransform = this->_componentRegistry.getComponent<Transform>(camEntityId);
    if (!cam || !camTransform) return glm::vec3(0, 0, 0);

    glm::mat4 proj;
    float viewportAspect = static_cast<float>(vpWidth) / static_cast<float>(vpHeight);
    if (cam->isOrtho) {
        float halfWidth = cam->orthoScale * viewportAspect;
        float halfHeight = cam->orthoScale;
        proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, cam->nearClip, cam->farClip);
    } else
        proj = glm::perspective(glm::radians(cam->fov), viewportAspect, cam->nearClip, cam->farClip);

    glm::vec3 camPos = camTransform->position;
    glm::vec3 forward = glm::normalize(cam->forward);
    glm::vec3 upVec = glm::normalize(-cam->up);
    glm::mat4 view = glm::lookAt(camPos, camPos + forward, upVec);

    glm::mat4 invVP = glm::inverse(proj * view);

    float ndcX = 1.0f - (localX / static_cast<float>(vpWidth)) * 2.0f;
    float ndcY = (localY / static_cast<float>(vpHeight)) * 2.0f - 1.0f;

    glm::vec4 clipNear(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 clipFar(ndcX, ndcY, 1.0f, 1.0f);

    glm::vec4 worldNear4 = invVP * clipNear;
    glm::vec4 worldFar4  = invVP * clipFar;

    glm::vec3 rayOrigin = glm::vec3(worldNear4) / worldNear4.w;
    glm::vec3 rayDir    = glm::normalize(glm::vec3(worldFar4) / worldFar4.w - rayOrigin);

    glm::vec3 planePoint(0, 0, 0);
    glm::vec3 planeNormal(0, 0, 1);

    std::optional<RaycastHit> hit = RaycastSystem::intersectRayPlane(
        rayOrigin,
        rayDir,
        planePoint,
        planeNormal
    );

    if (hit && hit->isValid()) return hit->point;

    return rayOrigin + rayDir * 10.0f;
}

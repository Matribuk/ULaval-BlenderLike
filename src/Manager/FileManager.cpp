#include "Manager/FileManager.hpp"

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


std::pair<EntityID, std::string> FileManager::_importMesh(const std::string& filename)
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
        this->_componentRegistry.registerComponent(entity.getId(), Renderable(mesh, ofColor::white));
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

EntityID FileManager::_createImagePlaneEntity(ofTexture& texture, const std::string& name, const glm::vec3& position)
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

void FileManager::handleAssetDrop(const AssetInfo* asset, SceneManager& sceneManager, ResourceManager& resourceManager, EventLogPanel& eventLog)
{
    if (!asset) return;

    if (asset->isImage) {
        ofTexture& entityTexture = resourceManager.loadTexture(asset->filepath);
        EntityID entityId = this->_createImagePlaneEntity(
            entityTexture,
            asset->name,
            glm::vec3(0, 0, 0)
        );

        if (entityId != INVALID_ENTITY) {
            sceneManager.registerEntity(entityId, asset->name);
            eventLog.addLog("Plane created: " + asset->name, ofColor::lime);
        }
    } else if (!asset->isImage && !asset->filepath.empty()) {
        std::pair<EntityID, std::string> result = this->_importMesh(asset->filepath);
        if (result.first != INVALID_ENTITY) {
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

    for (int i = 0; i < 4; i++) {
        mesh.addNormal(glm::vec3(0, 0, 1));
    }

    return mesh;
}

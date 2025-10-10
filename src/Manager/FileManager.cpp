#include "Manager/FileManager.hpp"

FileManager::FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager)
    : _componentRegistry(componentRegistry), _entityManager(entityManager) {}

void FileManager::exportMesh(EntityID entity, const std::string& filename)
{
    if (!this->_componentRegistry.hasComponent<Renderable>(entity)) {
        ofLogError("FileManager") << "Entity #" << entity << " has no Renderable component.";
        return;
    }

    Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(entity);
    if (!renderable) {
        ofLogError("FileManager") << "Renderable component is null for Entity #" << entity;
        return;
    }

    try {
        renderable->mesh.save(filename);
        ofLogNotice("FileManager") << "Exported mesh for Entity #" << entity << " to " << filename;
    }
    catch (const std::exception& e) {
        ofLogError("FileManager") << "Failed to export mesh: " << e.what();
    }
}


std::pair<EntityID, std::string> FileManager::importMesh(const std::string& filename)
{
    if (this->_isImageFile(filename)) {
        ofLogWarning("FileManager") << "Use importImageTexture() for images, not importMesh()";
        return {INVALID_ENTITY, ""};
    }

    this->_model = std::make_unique<ofxAssimpModelLoader>();

    if (!this->_model->load(filename)) {
        ofLogError("FileManager") << "Failed to load model: " << filename;
        return {INVALID_ENTITY, ""};
    }

    int numMeshes = this->_model->getMeshCount();

    glm::vec3 min = this->_model->getSceneMin();
    glm::vec3 max = this->_model->getSceneMax();
    glm::vec3 size = max - min;
    float maxDim = std::max({size.x, size.y, size.z});

    float targetSize = 5.0f;
    float scaleFactor = (maxDim != 0.f) ? targetSize / maxDim : 1.0f;

    std::filesystem::path path(filename);
    std::string fileName = path.stem().string();

    Entity entity = this->_entityManager.createEntity();
    if (numMeshes > 0) {
        ofMesh mesh = this->_model->getMesh(0);

        this->_componentRegistry.registerComponent(entity.getId(), Transform(
            glm::vec3(0),
            glm::vec3(scaleFactor)
        ));

        this->_componentRegistry.registerComponent(entity.getId(), Renderable(mesh, ofColor::white));
        this->_componentRegistry.registerComponent(entity.getId(), Selectable());
    }

    return {entity.getId(), fileName};
}

std::shared_ptr<ofTexture> FileManager::importImageTexture(const std::string& filename)
{
    ofImage image;
    if (!image.load(filename)) {
        ofLogError("FileManager") << "Failed to load image: " << filename;
        return nullptr;
    }

    std::filesystem::path path(filename);
    std::string fileName = path.stem().string();

    ofLogNotice("FileManager") << "Imported image texture: " << fileName
                               << " (" << image.getWidth() << "x" << image.getHeight() << ")";

    auto texture = std::make_shared<ofTexture>(image.getTexture());
    return texture;
}

EntityID FileManager::createImagePlaneEntity(std::shared_ptr<ofTexture> texture, const std::string& name, const glm::vec3& position)
{
    if (!texture) {
        ofLogError("FileManager") << "Cannot create plane with null texture";
        return INVALID_ENTITY;
    }

    float imgWidth = texture->getWidth();
    float imgHeight = texture->getHeight();
    float aspectRatio = imgWidth / imgHeight;
    float targetHeight = 3.0f;
    float targetWidth = targetHeight * aspectRatio;

    ofMesh planeMesh = this->_createImagePlane(targetWidth, targetHeight);

    Entity entity = this->_entityManager.createEntity();
    this->_componentRegistry.registerComponent(entity.getId(), Transform(
        position, glm::vec3(1.0f)
    ));

    Material* material = new Material();
    material->texture = new ofTexture(*texture);

    Renderable renderable(planeMesh, ofColor::white);
    renderable.material = material;
    this->_componentRegistry.registerComponent(entity.getId(), renderable);
    this->_componentRegistry.registerComponent(entity.getId(), Selectable());

    ofLogNotice("FileManager") << "Created image plane entity: " << name;

    return entity.getId();
}

bool FileManager::_isImageFile(const std::string& filename)
{
    std::filesystem::path path(filename);
    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return (ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
            ext == ".bmp" || ext == ".gif" || ext == ".tga");
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
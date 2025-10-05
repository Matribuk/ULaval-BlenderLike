#include "Manager/FileManager/FileManager.hpp"

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


EntityID FileManager::importMesh(const std::string& filename)
{
    this->_model = std::make_unique<ofxAssimpModelLoader>();

    if (!this->_model->load(filename)) {
        ofLogError("FileManager") << "Failed to load model: " << filename;
        return INVALID_ENTITY;
    }

    int numMeshes = this->_model->getMeshCount();

    glm::vec3 min = this->_model->getSceneMin();
    glm::vec3 max = this->_model->getSceneMax();
    glm::vec3 size = max - min;
    float maxDim = std::max({size.x, size.y, size.z});

    float targetSize = 5.0f;
    float scaleFactor = (maxDim != 0.f) ? targetSize / maxDim : 1.0f;

    Entity entity = this->_entityManager.createEntity();
    if (numMeshes > 0) {
        ofMesh mesh = this->_model->getMesh(0);

        this->_componentRegistry.registerComponent(entity.getId(), Transform(
            glm::vec3(0),
            glm::vec3(scaleFactor)
        ));

        this->_componentRegistry.registerComponent(entity.getId(), Renderable(mesh, ofColor::white));
    }

    return entity.getId();
}


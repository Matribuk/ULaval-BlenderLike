#pragma once

#include "EntityManager.hpp"
#include "ComponentRegistry.hpp"
#include "Transform.hpp"
#include "Renderable.hpp"
#include "ofxAssimpModelLoader.h"
#include <string>

class FileManager {
    public:
        FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager);
        ~FileManager() = default;

        void exportMesh(EntityID, const std::string& filename);
        EntityID importMesh(const std::string& filename);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::unique_ptr<ofxAssimpModelLoader> _model;
};

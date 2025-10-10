#pragma once

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Selectable.hpp"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "ofxAssimpModelLoader.h"

#include <string>

class FileManager {
    public:
        FileManager(ComponentRegistry& componentRegistry, EntityManager& entityManager);
        ~FileManager() = default;

        void exportMesh(EntityID, const std::string& filename);
        std::pair<EntityID, std::string> importMesh(const std::string& filename);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::unique_ptr<ofxAssimpModelLoader> _model;
};

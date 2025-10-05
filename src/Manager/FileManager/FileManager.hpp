#pragma once

#include <string>
#include "../../Core/EntityManager/EntityManager.hpp"
#include "../../Core/ComponentRegistry/ComponentRegistry.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/Renderable.hpp"
#include "ofxAssimpModelLoader.h"

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

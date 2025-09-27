#pragma once

#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"

#include "ofApp.h"

class RenderSystem {
    public:
        RenderSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~RenderSystem() = default;

        void render();

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;

        void drawMesh(const ofMesh& mesh, const glm::mat4& transform, const ofColor& color);
};

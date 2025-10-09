#include "UI/Viewport/Viewport.hpp"

Viewport::Viewport(CameraManager& cameraManager, RenderSystem& renderSystem, ViewportID id)
    : _cameraManager(cameraManager), _renderSystem(renderSystem), _id(id)
{
    this->_rect = ofRectangle((ofGetWindowWidth() - 1075) / 2 , (ofGetWindowHeight() - 605) / 2 , 1075, 605);
    this->_fboInitialized = false;
    this->_isDragging = false;
    this->_lastMousePos = glm::vec2(0);
    this->_dragDelta = glm::vec2(0);
}

Viewport::~Viewport()
{
    if (this->_fboInitialized)
        this->_fbo.clear();
}

void Viewport::renderScene()
{
    if (!this->_fboInitialized) return;

    this->_fbo.begin();
    ofClear(50, 50, 60, 255);

    EntityID previousCamera = this->_cameraManager.getActiveCameraId();

    if (this->_cameraId != INVALID_ENTITY)
        this->_cameraManager.setActiveCamera(this->_cameraId);

    this->_cameraManager.update(this->_rect.width, this->_rect.height);
    this->_renderSystem.render();

    if (previousCamera != INVALID_ENTITY)
        this->_cameraManager.setActiveCamera(previousCamera);

    this->_fbo.end();
}

bool Viewport::render()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse;

    std::string windowName = "Viewport " + std::to_string(_id);
    if (ImGui::Begin(windowName.c_str(), nullptr, flags))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x > 0 && viewportSize.y > 0 &&
            (!this->_fboInitialized || this->_fbo.getWidth() != (int)viewportSize.x || this->_fbo.getHeight() != (int)viewportSize.y))
            this->_initializeFbo((int)viewportSize.x, (int)viewportSize.y);

        renderScene();

        if (this->_fboInitialized && this->_fbo.isAllocated()) {
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();

            this->_rect.x = cursorPos.x;
            this->_rect.y = cursorPos.y;
            this->_rect.width = viewportSize.x;
            this->_rect.height = viewportSize.y;

            GLuint texID = this->_fbo.getTexture().getTextureData().textureID;
            ImGui::Image((ImTextureID)(uintptr_t)texID, viewportSize, ImVec2(0,1), ImVec2(1,0));
        }

        this->_handleMouseDrag();

        bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        ImGui::End();

        return hovered;
    }

    ImGui::End();
    return false;
}

ViewportID Viewport::getId() const
{
    return this->_id;
}

void Viewport::setRect(const ofRectangle& rect)
{
    this->_rect = rect;
}

ofRectangle Viewport::getRect() const
{
    return this->_rect;
}

void Viewport::setTexture(const ofTexture& texture)
{
    if (!this->_fboInitialized) return;

    this->_fbo.getTexture().clear();
    this->_fbo.getTexture() = texture;
}

ofTexture& Viewport::getTexture()
{
    return this->_fbo.getTexture();
}

void Viewport::setCamera(EntityID cameraId)
{
    this->_cameraId = cameraId;
}

EntityID Viewport::getCamera() const
{
    return this->_cameraId;
}

void Viewport::setActiveViewport(ViewportID id)
{
    this->_activeViewport = id;
}

ViewportID Viewport::getActiveViewport() const
{
    return this->_activeViewport;
}

bool Viewport::isMouseDragging() const
{
    return this->_isDragging;
}

glm::vec2 Viewport::getMouseDragDelta() const
{
    return this->_dragDelta;
}

void Viewport::_initializeFbo(int width, int height)
{
    if (width <= 0 || height <= 0) return;

    ofFboSettings settings;
    settings.width = width;
    settings.height = height;
    settings.internalformat = GL_RGBA;
    settings.numSamples = 4;
    settings.useDepth = true;
    settings.useStencil = false;
    settings.textureTarget = GL_TEXTURE_2D;

    this->_fbo.allocate(settings);
    this->_fboInitialized = true;
}

void Viewport::_handleMouseDrag()
{
    if (ImGui::IsItemHovered()) {
        ImVec2 mousePos = ImGui::GetMousePos();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
            ImGui::IsMouseClicked(ImGuiMouseButton_Middle) ||
            ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            this->_isDragging = true;
            this->_lastMousePos = glm::vec2(mousePos.x, mousePos.y);
            this->_dragDelta = glm::vec2(0, 0);
        }
    }

    if (_isDragging) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) ||
            ImGui::IsMouseDown(ImGuiMouseButton_Middle) ||
            ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            glm::vec2 currentPos = glm::vec2(mousePos.x, mousePos.y);

            this->_dragDelta = currentPos - this->_lastMousePos;
            this->_lastMousePos = currentPos;
        } else {
            this->_isDragging = false;
            this->_dragDelta = glm::vec2(0, 0);
        }
    }
}

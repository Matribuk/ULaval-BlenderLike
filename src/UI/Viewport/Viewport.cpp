#include "UI/Viewport/Viewport.hpp"

Viewport::Viewport(CameraManager& cameraManager, RenderSystem& renderSystem, ViewportID id)
    : _cameraManager(cameraManager), _renderSystem(renderSystem), _id(id)
{
    this->_rect = ofRectangle((ofGetWindowWidth() - 1075) / 2 , (ofGetWindowHeight() - 605) / 2 , 1075, 605);
}

Viewport::~Viewport()
{
    if (this->_fboInitialized)
        this->_fbo.clear();
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

void Viewport::renderScene()
{
    if (!this->_fboInitialized) return;

    this->_fbo.begin();

    ofClear(50, 50, 60, 255);
    this->_cameraManager.update(this->_rect.width, this->_rect.height);
    this->_renderSystem.render();

    this->_fbo.end();
}

bool Viewport::render()
{
    ImGui::SetNextWindowPos(ImVec2(_rect.x, _rect.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(_rect.width, _rect.height), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize;

    std::string windowName = "Viewport " + std::to_string(_id);
    if (ImGui::Begin(windowName.c_str(), nullptr, flags))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x > 0 && viewportSize.y > 0 &&
            (!_fboInitialized || _fbo.getWidth() != (int)viewportSize.x || _fbo.getHeight() != (int)viewportSize.y)) 
            _initializeFbo((int)viewportSize.x, (int)viewportSize.y);

        renderScene();

        if (_fboInitialized && _fbo.isAllocated()) {
            GLuint texID = _fbo.getTexture().getTextureData().textureID;
            ImGui::Image((ImTextureID)(uintptr_t)texID, viewportSize, ImVec2(0,1), ImVec2(1,0));
        }

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
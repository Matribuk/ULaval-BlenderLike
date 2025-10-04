#include "Viewport.hpp"

Viewport::Viewport(CameraSystem& cameraSystem, RenderSystem& renderSystem, ViewportID id)
    : _cameraSystem(cameraSystem), _renderSystem(renderSystem), _fboInitialized(false), _id(id)
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

    this->_cameraSystem.update(this->_rect.width, this->_rect.height);

    Camera* activeCam = nullptr;
    Transform* activeTf = nullptr;

    for (EntityID id : this->_cameraSystem.getEntityManager().getAllEntities()) {
        activeCam = this->_cameraSystem.getRegistry().getComponent<Camera>(id);
        activeTf = this->_cameraSystem.getRegistry().getComponent<Transform>(id);
        if (activeCam && activeTf) break;
    }

    if (activeCam) {
        ofEnableAlphaBlending();

        ofSetMatrixMode(OF_MATRIX_PROJECTION);
        ofLoadMatrix(activeCam->projMatrix);

        ofSetMatrixMode(OF_MATRIX_MODELVIEW);
        ofLoadMatrix(activeCam->viewMatrix);

        ofSetColor(255);
        ofDrawAxis(100);

        ofPushStyle();
        ofSetColor(100);
        for (int i = -10; i <= 10; i++) {
            ofDrawLine(-10, 0, i, 10, 0, i);
            ofDrawLine(i, 0, -10, i, 0, 10);
        }
        ofPopStyle();

        this->_renderSystem.render();
    }

    this->_fbo.end();
}

void Viewport::render()
{
    ImGui::SetNextWindowPos(ImVec2(this->_rect.x, this->_rect.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(this->_rect.width, this->_rect.height), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse;

    std::string windowName = "Viewport " + std::to_string(this->_id);
    if (ImGui::Begin(windowName.c_str(), nullptr, flags))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        if (!this->_fboInitialized || this->_fbo.getWidth() != (int)viewportSize.x || this->_fbo.getHeight() != (int)viewportSize.y)
            this->_initializeFbo((int)viewportSize.x, (int)viewportSize.y);

        renderScene();

        if (this->_fboInitialized) {
            GLuint texID = this->_fbo.getTexture().getTextureData().textureID;
            GLenum target = this->_fbo.getTexture().getTextureData().textureTarget;

            ImGui::Image(texID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }

    ImGui::End();
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


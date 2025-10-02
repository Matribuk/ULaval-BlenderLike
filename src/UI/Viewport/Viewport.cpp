#include "Viewport.hpp"

Viewport::Viewport(CameraSystem& cameraSystem, RenderSystem& renderSystem)
    : _cameraSystem(cameraSystem), _renderSystem(renderSystem), _fboInitialized(false)
{
    this->_size = ImVec2(1075, 605);
    this->_position = ImVec2((ofGetWindowWidth() - this->_size.x) / 2 , (ofGetWindowHeight() - this->_size.y) / 2 );
}

Viewport::~Viewport()
{
    if (_fboInitialized)
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
    if (!_fboInitialized) return;

    this->_fbo.begin();

    ofClear(50, 50, 60, 255);

    this->_cameraSystem.update(this->_size.x, this->_size.y);

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
    ImGui::SetNextWindowPos(this->_position, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(this->_size, ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoMove;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->_defaultTheme.colorWindowBg);

    if (ImGui::Begin("Viewport", nullptr, flags))
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
    ImGui::PopStyleColor();
}

#include "ofApp.h"

void ofApp::setup()
{
    this->_setupOpenGL();

    this->_bootstrapper = std::make_unique<ApplicationBootstrapper>(
        this->_entityManager,
        this->_componentRegistry,
        this->_eventManager
    );

    if (!this->_bootstrapper->bootstrap()) return;

    this->_runtime = std::make_unique<ApplicationRuntime>(
        this->_eventManager,
        this->_componentRegistry,
        this->_bootstrapper->getSystems(),
        this->_bootstrapper->getManagers(),
        this->_bootstrapper->getUI(),
        this->_bootstrapper->getTestEntities()
    );

    this->_runtime->initialize();
}

void ofApp::_setupOpenGL()
{
    ofEnableDepthTest();
    ofEnableLighting();
    ofEnableSeparateSpecularLight();

    this->_light.setup();
    this->_light.setPosition(0, 10, 0);
    this->_light.enable();

    ofSetWindowTitle("IFT3100");
    ofSetFrameRate(60);
    ofBackground(20);

    this->_gui.setup();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}

void ofApp::update()
{
    if (this->_runtime) this->_runtime->update(ofGetWidth(), ofGetHeight());
}

void ofApp::draw()
{
    ofEnableDepthTest();
    glDisable(GL_CULL_FACE);

    ofPushView();
    ofSetupScreen();

    this->_drawUI();

    ofPopView();
}

void ofApp::_drawUI()
{
    this->_gui.begin();

    if (this->_bootstrapper) this->_bootstrapper->getManagers().uiManager->render();

    this->_gui.end();
}

void ofApp::exit()
{
    if (this->_runtime) this->_runtime->shutdown();
}

void ofApp::keyPressed(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Pressed));
}

void ofApp::keyReleased(int key)
{
    this->_eventManager.emit(KeyEvent(key, KeyEventType::Released));
}

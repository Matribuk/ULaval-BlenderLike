#include "ofApp.h"

void ofApp::setup() {
    renderSystem = std::make_unique<RenderSystem>(registry, entityManager);

    cameraEntity = entityManager.createEntity().getId();
    Camera camera;
    registry.registerComponent<Camera>(cameraEntity, camera);
    renderSystem->setActiveCamera(cameraEntity);

    EntityID cubeEntity = entityManager.createEntity().getId();
    Transform t;
    t.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    registry.registerComponent<Transform>(cubeEntity, t);

    Renderable r;
    r.mesh = ofMesh::box(100, 100, 100);
    r.color = ofColor::red;
    r.visible = true;
    registry.registerComponent<Renderable>(cubeEntity, r);
}

void ofApp::update() {
    Camera* cam = renderSystem->getActiveCameraObject();
    if (cam) {
        cam->position.x = 500.0f * sin(ofGetElapsedTimef());
        cam->position.z = 500.0f * cos(ofGetElapsedTimef());
    }
}

void ofApp::draw() {
    ofBackground(50, 50, 50);
    renderSystem->render();
}

void ofApp::exit() {}

void ofApp::keyPressed(int key) {}

void ofApp::keyReleased(int key) {}

void ofApp::mouseMoved(int x, int y ) {}

void ofApp::mouseDragged(int x, int y, int button) {}

void ofApp::mousePressed(int x, int y, int button) {}

void ofApp::mouseReleased(int x, int y, int button) {}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {}

void ofApp::mouseEntered(int x, int y) {}

void ofApp::mouseExited(int x, int y) {}

void ofApp::windowResized(int w, int h) {}

void ofApp::gotMessage(ofMessage msg) {}

void ofApp::dragEvent(ofDragInfo dragInfo) {}

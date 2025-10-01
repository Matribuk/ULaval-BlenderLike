#include "ofApp.h"

void ofApp::setup() {
    renderSystem = std::make_unique<RenderSystem>(registry, entityManager);
    transformSystem = std::make_unique<TransformSystem>(registry, entityManager);

    cubeEntity = entityManager.createEntity().getId();

    Transform cubeTransform;
    cubeTransform.position = glm::vec3(0, 0, 0);
    cubeTransform.rotation = glm::vec3(0, 0, 0);
    cubeTransform.scale    = glm::vec3(1, 1, 1);
    cubeTransform.matrix   = glm::mat4(1.0f);
    registry.registerComponent<Transform>(cubeEntity, cubeTransform);

    Renderable r;
    r.mesh = ofMesh::box(100, 100, 100);
    r.color = ofColor::red;
    r.visible = true;
    registry.registerComponent<Renderable>(cubeEntity, r);

    cameraEntity = entityManager.createEntity().getId();

    Transform camTransform;
    camTransform.position = glm::vec3(0, 100, 500);
    camTransform.rotation = glm::vec3(0, 0, 0);
    camTransform.scale = glm::vec3(1, 1, 1);
    registry.registerComponent<Transform>(cameraEntity, camTransform);

    Camera camera;
    camera.target = cubeTransform.position;
    camera.focusMode = true;
    registry.registerComponent<Camera>(cameraEntity, camera);

    renderSystem->setActiveCamera(cameraEntity);
}

void ofApp::update() {
    Transform* cubeTransform = registry.getComponent<Transform>(cubeEntity);
    Transform* camTransform = registry.getComponent<Transform>(cameraEntity);
    if (camTransform && cubeTransform) {
        float time = ofGetElapsedTimef();
        float radius = 500.0f;
        float height = 200.0f;
        glm::vec3 cubePos = cubeTransform->position;

        camTransform->position.x = cubePos.x + radius * sin(time);
        camTransform->position.z = cubePos.z + radius * cos(time);
        camTransform->position.y = cubePos.y + height;
    }
    cubeTransform->rotation.x += 0.01f;

    transformSystem->update();
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

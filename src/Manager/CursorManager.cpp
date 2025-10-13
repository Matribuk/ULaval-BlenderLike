#include "CursorManager.hpp"
#include <GLFW/glfw3.h>

static int shapeToIndex(int shape) {
    switch (shape) {
        case GLFW_ARROW_CURSOR:     return 0;
        case GLFW_IBEAM_CURSOR:     return 1;
        case GLFW_CROSSHAIR_CURSOR: return 2;
        case GLFW_HAND_CURSOR:      return 3;
        case GLFW_HRESIZE_CURSOR:   return 4;
        case GLFW_VRESIZE_CURSOR:   return 5;
        default:                    return 0;
    }
}

void CursorManager::init(GLFWwindow* window) {
    this->_window = window;
    this->_cursors[0] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    this->_cursors[1] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    this->_cursors[2] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    this->_cursors[3] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    this->_cursors[4] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    this->_cursors[5] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
}
void CursorManager::requestCursor(CursorLayer layer, int shape) {
    this->_requested[(int)layer] = shape;
}

void CursorManager::resetCursor(CursorLayer layer) {
    this->_requested[(int)layer] = GLFW_ARROW_CURSOR;
}

void CursorManager::apply() {
    static const CursorLayer priority[] = {
        CursorLayer::Resize,
        CursorLayer::Drag,
        CursorLayer::TextInput,
        CursorLayer::Tool,
        CursorLayer::Default
    };

    int chosen = GLFW_ARROW_CURSOR;
    for (auto layer : priority) {
        int s = _requested[(int)layer];
        if (s != GLFW_ARROW_CURSOR) {
            chosen = s;
            break;
        }
    }

    if (chosen != this->_currentShape) {
        this->_currentShape = chosen;
        int idx = shapeToIndex(this->_currentShape);
        glfwSetCursor(this->_window, this->_cursors[idx]);
    }
}
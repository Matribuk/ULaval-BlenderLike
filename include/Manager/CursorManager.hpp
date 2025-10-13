#pragma once

#include "GLFW/glfw3.h"

enum class CursorLayer {
    Default = 0,
    TextInput,
    Tool,
    Drag,
    Resize,
    Count
};

class CursorManager {
public:
    CursorManager() = default;

    static CursorManager& get() {
        static CursorManager instance;
        return instance;
    }

    void init(GLFWwindow* window);
    void requestCursor(CursorLayer layer, int shape);
    void resetCursor(CursorLayer layer);
    void apply();

private:

    GLFWwindow* _window = nullptr;
    int _requested[(int)CursorLayer::Count] = {
        GLFW_ARROW_CURSOR, GLFW_ARROW_CURSOR, GLFW_ARROW_CURSOR,
        GLFW_ARROW_CURSOR, GLFW_ARROW_CURSOR
    };
    int _currentShape = GLFW_ARROW_CURSOR;
    GLFWcursor* _cursors[6] = {nullptr};
};


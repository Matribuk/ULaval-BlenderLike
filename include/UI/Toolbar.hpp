#pragma once

#include <ofMain.h>
#include "ofxImGui.h"
#include "GLFW/glfw3.h"
#include <vector>
#include <string>
#include "ofGraphics.h"

#include "Manager/CursorManager.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/MouseEvent.hpp"

enum class ToolMode {
    Select,
    Move
};

struct ToolButton {
    std::string name;
    std::vector<std::function<void(const std::any&)>> action;

    ToolButton(const std::string& n, std::vector<std::function<void(const std::any&)>> a)
        : name(n), action(a) {}
};

class Toolbar {
    public:
        Toolbar(CursorManager& cursorManager);

        void render();

        const ToolButton* getSelectedTool() const;
        void setSelectedTool(int tool);
        void addTool(ToolButton);

        void setToggleProjectionCallback(std::function<void()> callback);
        void setImportCallback(std::function<void()> callback);
        void setExportCallback(std::function<void()> callback);
        void setSelectCallback(std::function<void()> callback);
        void setMoveCallback(std::function<void()> callback);

        ToolMode getActiveToolMode() const;

    private:
        CursorManager& _cursorManager;

        void _renderToolButton(size_t index, const ImVec2& size);

        std::function<void()> _onToggleProjection;
        std::function<void()> _onSelect;
        std::function<void()> _onMove;
        std::function<void()> _onImport;
        std::function<void()> _onExport;

        int _selectedTool;
        float _buttonSpacing;
        int _currentCursor;

        std::vector<ToolButton> _tools;
};
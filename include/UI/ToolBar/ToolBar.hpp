#pragma once

#include <ofMain.h>
#include "ofxImGui.h"
#include <vector>
#include <string>
#include "ofGraphics.h"

struct ToolButton {
    std::string name;
    std::vector<std::function<void(const std::any&)>> action;

    ToolButton(const std::string& n, std::vector<std::function<void(const std::any&)>> a)
        : name(n), action(a) {}
};

class Toolbar {
    public:
        Toolbar();

        void render();

        const ToolButton* getSelectedTool() const;
        void setSelectedTool(int tool);
        void addTool(ToolButton);

        private:
        void _applyCursor();
        void renderToolButton(size_t index, const ImVec2& size);

        int _selectedTool;
        float _buttonSpacing;

        std::vector<ToolButton> _tools;
        ImGuiMouseCursor _currentCursor;
};
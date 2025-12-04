#include "UI/Toolbar.hpp"

Toolbar::Toolbar(CursorManager& cursorManager) : _cursorManager(cursorManager), _selectedTool(-1), _buttonSpacing(5.0)
{
    this->_tools.emplace_back(ToolButton{"Select", {
        [this](std::any){
            this->_cursorManager.requestCursor(CursorLayer::Tool, GLFW_ARROW_CURSOR);
            if (this->_onSelect) this->_onSelect();
        }
    }});

    this->_tools.emplace_back(ToolButton{"Move", {
        [this](std::any){
            this->_cursorManager.requestCursor(CursorLayer::Tool, GLFW_HAND_CURSOR);
            if (this->_onMove) this->_onMove();
        }
    }});

    this->_tools.emplace_back(ToolButton{"Ortho/Persp", {
        [this](std::any){
            if (this->_onToggleProjection)
                this->_onToggleProjection();
        }
    }});

    this->_tools.emplace_back(ToolButton{"Import", {
        [this](std::any){
            if (this->_onImport) this->_onImport();
        }
    }});

    this->_tools.emplace_back(ToolButton{"Export", {
        [this](std::any){
            if (this->_onExport) this->_onExport();
        }
    }});

    this->_tools.emplace_back(ToolButton{"Raytracing", {
        [this](std::any){
            if (this->_onRaytracing) this->_onRaytracing();
        }
    }});
}

void Toolbar::render()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                           | ImGuiWindowFlags_NoScrollbar
                           | ImGuiWindowFlags_NoCollapse
                           | ImGuiWindowFlags_NoMove
                           | ImGuiWindowFlags_NoResize;

    if (ImGui::Begin("Toolbar", nullptr, flags))
    {
        ImGui::SameLine();

        ImVec2 buttonSize(80, 20);
        for (size_t i = 0; i < this->_tools.size(); ++i) {
            if (i > 0) ImGui::SameLine();
            this->_renderToolButton(i, buttonSize);
        }
        this->_cursorManager.apply();
    }

    ImGui::End();
}

const ToolButton* Toolbar::getSelectedTool() const
{
    if (this->_selectedTool < 0 || this->_selectedTool >= static_cast<int>(this->_tools.size()))
        return nullptr;
    return &this->_tools[this->_selectedTool];
}

void Toolbar::setSelectedTool(int tool)
{
    if (tool >= -1 && tool < static_cast<int>(this->_tools.size()))
    this->_selectedTool = tool;
}

void Toolbar::addTool(ToolButton button)
{
    this->_tools.emplace_back(button);
}

void Toolbar::setToggleProjectionCallback(std::function<void()> callback)
{
    this->_onToggleProjection = callback;
}

void Toolbar::setImportCallback(std::function<void()> callback)
{
    this->_onImport = callback;
}

void Toolbar::setExportCallback(std::function<void()> callback)
{
    this->_onExport = callback;
}

void Toolbar::setSelectCallback(std::function<void()> callback)
{
    this->_onSelect = callback;
}

void Toolbar::setMoveCallback(std::function<void()> callback)
{
    this->_onMove = callback;
}

void Toolbar::setRaytracingCallback(std::function<void()> callback)
{
    this->_onRaytracing = callback;
}

ToolMode Toolbar::getActiveToolMode() const
{
    if (this->_selectedTool < 0 || this->_selectedTool >= static_cast<int>(this->_tools.size()))
        return ToolMode::Select;

    switch (this->_selectedTool) {
        case 0: return ToolMode::Select;
        case 1: return ToolMode::Move;
        default: return ToolMode::Select;
    }
}

void Toolbar::_renderToolButton(size_t i, const ImVec2& size)
{
    ImGui::PushID(static_cast<int>(i));

    bool clicked = ImGui::Button(this->_tools[i].name.c_str(), size);

    if (clicked) {
        this->_selectedTool = static_cast<int>(i);
        if (const ToolButton* tool = getSelectedTool()) {
            for (auto& callback : tool->action) {
                callback(std::any{});
            }
        }
    }

    ImGui::PopID();
}

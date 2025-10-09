#include "UI/Toolbar/Toolbar.hpp"

Toolbar::Toolbar() : _selectedTool(-1), _buttonSpacing(5.0), _currentCursor(ImGuiMouseCursor_Arrow)
{
    this->_tools.emplace_back(ToolButton{"Select", {
        [this](std::any){
            this->_currentCursor = ImGuiMouseCursor_Arrow;
        }
    }});

    this->_tools.emplace_back(ToolButton{"Move", {
        [this](std::any){
            this->_currentCursor = ImGuiMouseCursor_Hand;
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
    }

    ImGui::End();

    this->_applyCursor();
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

void Toolbar::_applyCursor()
{
    ImGui::SetMouseCursor(this->_currentCursor);
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

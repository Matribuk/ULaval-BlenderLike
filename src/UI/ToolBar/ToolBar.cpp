#include "UI/ToolBar/ToolBar.hpp"

Toolbar::Toolbar() : _selectedTool(-1), _buttonSpacing(5.0)
{
    this->_tools.emplace_back(ToolButton{"Select", {}});
    this->_tools.emplace_back(ToolButton{"Move", {}});
}

void Toolbar::renderToolButton(size_t i, const ImVec2& size)
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

void Toolbar::render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ofGetWindowWidth(), 20), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0);

    if (ImGui::Begin("Toolbar", nullptr, flags))
    {
        ImVec2 availableSize = ImGui::GetContentRegionAvail();

        float totalSpacing = this->_buttonSpacing * (this->_tools.size() - 1);
        float buttonWidth = (ofGetWindowWidth() - totalSpacing) / static_cast<float>(this->_tools.size()) * 0.1;
        float buttonHeight = availableSize.y;
        ImVec2 buttonSize(buttonWidth, buttonHeight);

        for (size_t i = 0; i < this->_tools.size(); ++i) {
            if (i > 0) ImGui::SameLine();
            renderToolButton(i, buttonSize);
        }
    }

    ImGui::End();

    ImGui::PopStyleVar(1);
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
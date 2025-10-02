#include "ToolBar.hpp"

Toolbar::Toolbar() : _selectedTool(-1), _buttonSpacing(5.0)
{
    setPosition(0, 0);
    setSize(ofGetWindowWidth(), 20);

    this->_tools.emplace_back(ToolButton{"Select", {}});
    this->_tools.emplace_back(ToolButton{"Move", {}});
}

void Toolbar::renderToolButton(size_t i, const ImVec2& size)
{
    ImGui::PushID(static_cast<int>(i));

    bool isSelected = (static_cast<int>(i) == this->_selectedTool);
    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Text, this->_selectedTheme.colorText);
        ImGui::PushStyleColor(ImGuiCol_Button, this->_selectedTheme.colorButton);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, this->_selectedTheme.colorButtonHover);
    }

    bool clicked = ImGui::Button(this->_tools[i].name.c_str(), size);

    if (clicked) {
        this->_selectedTool = static_cast<int>(i);
        if (const ToolButton* tool = getSelectedTool()) {
            for (auto& callback : tool->action) {
            callback(std::any{});
        }
        }
    }

    if (isSelected)
        ImGui::PopStyleColor(3);

    ImGui::PopID();
}

void Toolbar::render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ofGetWindowWidth(), 20.0), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0);

    ImGui::PushStyleColor(ImGuiCol_Text, this->_defaultTheme.colorText);

    ImGui::PushStyleColor(ImGuiCol_Button, this->_defaultTheme.colorButton);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, this->_defaultTheme.colorButtonHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, this->_defaultTheme.colorButtonActive);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->_defaultTheme.colorWindowBg);

    if (ImGui::Begin("Toolbar", nullptr, flags))
    {
        ImVec2 availableSize = ImGui::GetContentRegionAvail();

        float totalSpacing = _buttonSpacing * (this->_tools.size() - 1);
        float buttonWidth = (this->_size.x - totalSpacing) / static_cast<float>(this->_tools.size()) * 0.1;
        float buttonHeight = availableSize.y;
        ImVec2 buttonSize(buttonWidth, buttonHeight);

        for (size_t i = 0; i < this->_tools.size(); ++i) {
            if (i > 0) ImGui::SameLine();
            renderToolButton(i, buttonSize);
        }
    }

    ImGui::End();

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(1);
}

const ToolButton* Toolbar::getSelectedTool() const
{
    if (_selectedTool < 0 || _selectedTool >= static_cast<int>(_tools.size()))
        return nullptr;
    return &_tools[_selectedTool];
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
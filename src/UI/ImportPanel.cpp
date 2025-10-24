#include "UI/ImportPanel.hpp"

ImportPanel::ImportPanel(FileManager& fileManager, AssetsPanel& assetsPanel, EventLogPanel& eventLogPanel)
    : _fileManager(fileManager), _assetsPanel(assetsPanel), _eventLogPanel(eventLogPanel)
{
    std::strcpy(_filePathBuffer, "");
}

void ImportPanel::open()
{
    _isOpen = true;
}

void ImportPanel::close()
{
    _isOpen = false;
}

void ImportPanel::render()
{
    if (!_isOpen) return;

    if (!ImGui::IsPopupOpen("Import File")) {
        ImGui::OpenPopup("Import File");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Import File", &_isOpen, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Import Image or 3D Model");
        ImGui::Separator();

        ImGui::Text("File Path:");
        if (ImGui::InputText("##filepath", _filePathBuffer, sizeof(_filePathBuffer))) {
            _filePath = std::string(_filePathBuffer);
        }

        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            ofFileDialogResult result = ofSystemLoadDialog("Choose a file to import (image or 3D model)", false);
            if (result.bSuccess) {
                _filePath = result.getPath();
                std::strcpy(_filePathBuffer, _filePath.c_str());
            }
        }

        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "Supported: .jpg, .png, .obj, .ply, .stl");

        ImGui::Separator();

        bool canImport = !_filePath.empty();
        if (!canImport) ImGui::BeginDisabled();

        if (ImGui::Button("Import", ImVec2(120, 0))) {
            std::string fullPath = _filePath;
            if (_filePath[0] != '/' && _filePath[0] != '~') {
                fullPath = ofToDataPath(_filePath, true);
            }

            _fileManager.importAndAddAsset(fullPath, _assetsPanel, _eventLogPanel);

            std::strcpy(_filePathBuffer, "");
            _filePath = "";
            close();
        }

        if (!canImport) ImGui::EndDisabled();

        ImGui::EndPopup();
    }
}

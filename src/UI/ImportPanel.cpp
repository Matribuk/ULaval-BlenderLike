#include "UI/ImportPanel.hpp"

ImportPanel::ImportPanel(FileManager& fileManager, AssetsPanel& assetsPanel, EventLogPanel& eventLogPanel)
    : this->_fileManager(fileManager), this->_assetsPanel(assetsPanel), this->_eventLogPanel(eventLogPanel)
{
    std::strcpy(this->_filePathBuffer, "");
}

void ImportPanel::open()
{
    this->_isOpen = true;
}

void ImportPanel::close()
{
    this->_isOpen = false;
}

bool ImportPanel::isOpen() const
{
    return this->_isOpen;
}

void ImportPanel::render()
{
    if (!this->_isOpen) return;

    if (!ImGui::IsPopupOpen("Import File")) {
        ImGui::OpenPopup("Import File");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Import File", &this->_isOpen, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Import Image or 3D Model");
        ImGui::Separator();

        ImGui::Text("File Path:");
        if (ImGui::InputText("##filepath", this->_filePathBuffer, sizeof(this->_filePathBuffer))) {
            this->_filePath = std::string(this->_filePathBuffer);
        }

        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            ofFileDialogResult result = ofSystemLoadDialog("Choose a file to import (image or 3D model)", false);
            if (result.bSuccess) {
                this->_filePath = result.getPath();
                std::strcpy(this->_filePathBuffer, this->_filePath.c_str());
            }
        }

        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "Supported: .jpg, .png, .obj, .ply, .stl");

        ImGui::Separator();

        bool canImport = !this->_filePath.empty();
        if (!canImport) ImGui::BeginDisabled();

        if (ImGui::Button("Import", ImVec2(120, 0))) {
            std::string fullPath = this->_filePath;
            if (this->_filePath[0] != '/' && this->_filePath[0] != '~') {
                fullPath = ofToDataPath(this->_filePath, true);
            }

            this->_fileManager.importAndAddAsset(fullPath, this->_assetsPanel, this->_eventLogPanel);

            std::strcpy(this->_filePathBuffer, "");
            this->_filePath = "";
            close();
        }

        if (!canImport) ImGui::EndDisabled();

        ImGui::EndPopup();
    }
}

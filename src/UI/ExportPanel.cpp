#include "UI/ExportPanel.hpp"

ExportPanel::ExportPanel(ImageSequenceExporter& exporter, ViewportManager& viewportManager)
    : _exporter(exporter), _viewportManager(viewportManager)
{
    this->_exportFolder = ofToDataPath("exports", true);
    std::strcpy(this->_folderBuffer, this->_exportFolder.c_str());
}

void ExportPanel::open()
{
    this->_isOpen = true;
}

void ExportPanel::close()
{
    this->_isOpen = false;
}

bool ExportPanel::isOpen() const
{
    return this->_isOpen;
}

void ExportPanel::render()
{
    if (!this->_isOpen) return;

    if (!ImGui::IsPopupOpen("Export Frame Sequence")) {
        ImGui::OpenPopup("Export Frame Sequence");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Export Frame Sequence", &this->_isOpen, ImGuiWindowFlags_NoCollapse))
    {
        std::vector<std::unique_ptr<Viewport>>& viewports = this->_viewportManager.getViewports();

        if (viewports.empty()) {
            ImGui::TextDisabled("No viewports available");
            ImGui::EndPopup();
            return;
        }

        ImGui::Text("Export Settings");
        ImGui::Separator();

        std::vector<std::string> viewportNames;
        for (std::unique_ptr<Viewport>& vp : viewports) {
            viewportNames.push_back("Viewport " + std::to_string(vp->getId()));
        }

        if (ImGui::BeginCombo("Viewport", viewportNames[this->_selectedViewportIndex].c_str())) {
            for (size_t i = 0; i < viewportNames.size(); ++i) {
                bool isSelected = (this->_selectedViewportIndex == static_cast<int>(i));
                if (ImGui::Selectable(viewportNames[i].c_str(), isSelected)) {
                    this->_selectedViewportIndex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::InputInt("FPS", &this->_fps, 1, 10);
        if (this->_fps < 1) this->_fps = 1;
        if (this->_fps > 120) this->_fps = 120;

        ImGui::InputFloat("Duration (seconds)", &this->_durationSeconds, 0.1f, 1.0f, "%.3f");
        if (this->_durationSeconds < 0.001f) this->_durationSeconds = 0.001f;

        int totalFrames = static_cast<int>(this->_durationSeconds * this->_fps);
        ImGui::Text("Total frames: %d", totalFrames);

        ImGui::Separator();
        ImGui::Text("Export Folder:");
        if (ImGui::InputText("##folder", this->_folderBuffer, sizeof(this->_folderBuffer))) {
            this->_exportFolder = std::string(this->_folderBuffer);
        }

        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            ofFileDialogResult result = ofSystemLoadDialog("Choose export folder", true);
            if (result.bSuccess) {
                this->_exportFolder = result.getPath();
                std::strcpy(this->_folderBuffer, this->_exportFolder.c_str());
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Default")) {
            this->_exportFolder = ofToDataPath("exports", true);
            std::strcpy(this->_folderBuffer, this->_exportFolder.c_str());
        }

        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Current: %s", this->_exportFolder.c_str());

        ImGui::Separator();

        bool isRecording = this->_exporter.isRecording();

        if (!isRecording) {
            bool canStart = !this->_exportFolder.empty() && this->_selectedViewportIndex >= 0;
            if (!canStart) ImGui::BeginDisabled();

            if (ImGui::Button("Start Exporting", ImVec2(150, 30))) {
                ViewportID vpId = viewports[this->_selectedViewportIndex]->getId();
                this->_exporter.startRecording(vpId, this->_exportFolder, this->_fps, this->_durationSeconds);
            }

            if (!canStart) ImGui::EndDisabled();
        } else {
            if (ImGui::Button("Stop Recording", ImVec2(150, 30))) {
                this->_exporter.stopRecording();
            }
        }

        ImGui::Separator();

        if (isRecording) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Recording...");
            ImGui::Text("Frames captured: %d", this->_exporter.getFrameCount());
            ImGui::Text("Elapsed: %.3f / %.3f seconds", this->_exporter.getElapsedTime(), this->_exporter.getDuration());

            float progress = this->_exporter.getElapsedTime() / this->_exporter.getDuration();
            ImGui::ProgressBar(progress, ImVec2(-1, 0), "");

            ImGui::Text("Export folder: %s", this->_exporter.getExportFolder().c_str());
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Stopped");
        }

        ImGui::EndPopup();
    }
}

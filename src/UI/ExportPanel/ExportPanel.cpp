#include "UI/ExportPanel/ExportPanel.hpp"

ExportPanel::ExportPanel(ImageSequenceExporter& exporter, ViewportManager& viewportManager)
    : _exporter(exporter), _viewportManager(viewportManager)
{
    std::strcpy(_folderBuffer, "");
}

void ExportPanel::render()
{
    if (ImGui::Begin("Export", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        auto& viewports = _viewportManager.getViewports();

        if (viewports.empty()) {
            ImGui::TextDisabled("No viewports available");
            ImGui::End();
            return;
        }

        ImGui::Text("Export Settings");
        ImGui::Separator();

        std::vector<std::string> viewportNames;
        for (auto& vp : viewports) {
            viewportNames.push_back("Viewport " + std::to_string(vp->getId()));
        }

        if (ImGui::BeginCombo("Viewport", viewportNames[_selectedViewportIndex].c_str())) {
            for (size_t i = 0; i < viewportNames.size(); ++i) {
                bool isSelected = (_selectedViewportIndex == static_cast<int>(i));
                if (ImGui::Selectable(viewportNames[i].c_str(), isSelected)) {
                    _selectedViewportIndex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::InputInt("FPS", &_fps, 1, 10);
        if (_fps < 1) _fps = 1;
        if (_fps > 120) _fps = 120;

        ImGui::InputFloat("Duration (seconds)", &_durationSeconds, 0.1f, 1.0f, "%.3f");
        if (_durationSeconds < 0.001f) _durationSeconds = 0.001f;

        int totalFrames = static_cast<int>(_durationSeconds * _fps);
        ImGui::Text("Total frames: %d", totalFrames);

        ImGui::Separator();
        ImGui::Text("Export Folder:");
        ImGui::InputText("##folder", _folderBuffer, sizeof(_folderBuffer));
        ImGui::SameLine();

        if (ImGui::Button("Browse...")) {
            ofFileDialogResult result = ofSystemLoadDialog("Choose export folder", true);
            if (result.bSuccess) {
                _exportFolder = result.getPath();
                std::strcpy(_folderBuffer, _exportFolder.c_str());
            }
        }

        ImGui::Separator();

        bool isRecording = _exporter.isRecording();

        if (!isRecording) {
            bool canStart = !_exportFolder.empty() && _selectedViewportIndex >= 0;
            if (!canStart) ImGui::BeginDisabled();

            if (ImGui::Button("Start Exporting", ImVec2(150, 30))) {
                ViewportID vpId = viewports[_selectedViewportIndex]->getId();
                _exporter.startRecording(vpId, _exportFolder, _fps, _durationSeconds);
            }

            if (!canStart) ImGui::EndDisabled();
        } else {
            if (ImGui::Button("Stop Recording", ImVec2(150, 30))) {
                _exporter.stopRecording();
            }
        }

        ImGui::Separator();

        if (isRecording) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Recording...");
            ImGui::Text("Frames captured: %d", _exporter.getFrameCount());
            ImGui::Text("Elapsed: %.3f / %.3f seconds", _exporter.getElapsedTime(), _exporter.getDuration());

            float progress = _exporter.getElapsedTime() / _exporter.getDuration();
            ImGui::ProgressBar(progress, ImVec2(-1, 0), "");

            ImGui::Text("Export folder: %s", _exporter.getExportFolder().c_str());
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Stopped");
        }
    }
    ImGui::End();
}

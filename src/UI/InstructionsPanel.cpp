#include "UI/InstructionsPanel.hpp"

void InstructionsPanel::render()
{
    if (ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Controls Overview");
        ImGui::Separator();

        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "Mouse");
        ImGui::Spacing();

        ImGui::TextWrapped("  * Move mouse → ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Test MouseEvent");

        ImGui::TextWrapped("  * In MOVE mode:");
        ImGui::BulletText("Left click → Move camera");
        ImGui::BulletText("Middle click → Rotate camera");
        ImGui::BulletText("Right click → Move forward/backward");

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Keyboard");
        ImGui::Spacing();

        ImGui::TextWrapped("  * Any key → ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Test KeyEvent");

        ImGui::BulletText("P → Switch camera projection mode");
        ImGui::BulletText("CTRL+Z → (Reserved for Undo)");
        ImGui::BulletText("CTRL+Y → (Reserved for Redo)");

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f),
            "Tip:");
        ImGui::SameLine();
        ImGui::TextWrapped(
            "Use toolbar buttons to change modes "
            "("
        );
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f, 0.9f, 1.0f, 1.0f), "Select / Move / Rotate / Scale");
        ImGui::SameLine();
        ImGui::Text(")");

        ImGui::Spacing();
    }
    ImGui::End();
}
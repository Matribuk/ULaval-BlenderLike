#include "UI/EventLogPanel/EventLogPanel.hpp"

void EventLogPanel::addLog(const std::string& message, const ofColor& color)
{
    if (_logs.size() >= _maxLogs) _logs.erase(_logs.begin());

    _logs.push_back({message, color, std::chrono::steady_clock::now()});
}

void EventLogPanel::render()
{
    if (ImGui::Begin("Event Log", nullptr, ImGuiWindowFlags_NoCollapse)) {
        auto now = std::chrono::steady_clock::now();

        for (auto it = _logs.rbegin(); it != _logs.rend(); ++it) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->timestamp).count();

            ImGui::TextColored(
                ImVec4(it->color.r / 255.0f, it->color.g / 255.0f, it->color.b / 255.0f, 1.0f),
                "[%.2fs] %s", elapsed / 1000.0f, it->message.c_str()
            );
        }

        if (ImGui::Button("Clear")) clear();
    }
    ImGui::End();
}

void EventLogPanel::clear()
{
    _logs.clear();
}

#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include <chrono>
#include "ofColor.h"
struct LogEntry {
    std::string message;
    ofColor color;
    std::chrono::steady_clock::time_point timestamp;
};

class EventLogPanel {
public:
    EventLogPanel() = default;

    void addLog(const std::string& message, const ofColor& color = ofColor::white);
    void render();
    void clear();

private:
    std::vector<LogEntry> _logs;
    const size_t _maxLogs = 100;
};

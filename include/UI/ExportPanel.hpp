#pragma once

#include "Systems/ImageSequenceExporter.hpp"
#include "Manager/ViewportManager.hpp"
#include "ofxImGui.h"
#include <string>

class ExportPanel {
    public:
        ExportPanel(ImageSequenceExporter& exporter, ViewportManager& viewportManager);
        ~ExportPanel() = default;

        void render();
        void open();
        void close();
        bool isOpen() const { return _isOpen; }

    private:
        ImageSequenceExporter& _exporter;
        ViewportManager& _viewportManager;

        int _selectedViewportIndex = 0;
        int _fps = 30;
        float _durationSeconds = 5.0f;
        std::string _exportFolder = "";
        char _folderBuffer[512] = "";
        bool _isOpen = false;
};

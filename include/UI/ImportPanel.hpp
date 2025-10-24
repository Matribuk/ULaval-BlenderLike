#pragma once

#include "Manager/FileManager.hpp"
#include "UI/AssetsPanel.hpp"
#include "UI/EventLogPanel.hpp"
#include "ofxImGui.h"
#include <string>

class ImportPanel {
    public:
        ImportPanel(FileManager& fileManager, AssetsPanel& assetsPanel, EventLogPanel& eventLogPanel);
        ~ImportPanel() = default;

        void render();
        void open();
        void close();
        bool isOpen() const { return _isOpen; }

    private:
        FileManager& _fileManager;
        AssetsPanel& _assetsPanel;
        EventLogPanel& _eventLogPanel;

        char _filePathBuffer[512] = "";
        std::string _filePath = "";
        bool _isOpen = false;
};

#pragma once

#include "Manager/ViewportManager.hpp"
#include <ofMain.h>
#include <string>

class ImageSequenceExporter {
    public:
        ImageSequenceExporter(ViewportManager& viewportManager);
        ~ImageSequenceExporter() = default;

        void startRecording(ViewportID viewportId, const std::string& folder, int fps, float durationSeconds);
        void stopRecording();
        void update(float deltaTime);

        bool isRecording() const { return _isRecording; }
        int getFrameCount() const { return _frameCount; }
        float getElapsedTime() const { return _elapsedTime; }
        float getDuration() const { return _duration; }
        std::string getExportFolder() const { return _exportFolder; }

    private:
        ViewportManager& _viewportManager;

        bool _isRecording = false;
        ViewportID _viewportId = INVALID_VIEWPORT;
        std::string _exportFolder;
        int _fps = 30;
        float _duration = 0.0f;
        float _elapsedTime = 0.0f;
        float _timeSinceLastCapture = 0.0f;
        int _frameCount = 0;
        std::string _sessionTimestamp;
        std::vector<ofPixels> _capturedFrames;

        void _captureFrame();
        void _saveAllFrames();
        std::string _generateFilename(int frameIndex);
        std::string _getCurrentTimestamp();
};

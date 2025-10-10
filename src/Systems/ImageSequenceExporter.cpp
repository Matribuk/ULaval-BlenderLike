#include "Systems/ImageSequenceExporter.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

ImageSequenceExporter::ImageSequenceExporter(ViewportManager& viewportManager)
    : _viewportManager(viewportManager) {}

void ImageSequenceExporter::startRecording(ViewportID viewportId, const std::string& folder, int fps, float durationSeconds)
{
    if (_isRecording) {
        ofLogWarning("ImageSequenceExporter") << "Already recording, stop first";
        return;
    }

    _viewportId = viewportId;
    _exportFolder = folder;
    _fps = fps;
    _duration = durationSeconds;
    _elapsedTime = 0.0f;
    _frameCount = 0;
    _timeSinceLastCapture = 0.0f;
    _sessionTimestamp = _getCurrentTimestamp();

    _capturedFrames.clear();
    int estimatedFrames = static_cast<int>(fps * durationSeconds);
    _capturedFrames.reserve(estimatedFrames);

    _isRecording = true;

    ofLogNotice("ImageSequenceExporter") << "Started recording viewport " << viewportId
                                         << " to " << folder
                                         << " (fps: " << fps << ", duration: " << durationSeconds << "s)";
}

void ImageSequenceExporter::stopRecording()
{
    if (!_isRecording) {
        ofLogWarning("ImageSequenceExporter") << "Not currently recording";
        return;
    }

    _isRecording = false;
    ofLogNotice("ImageSequenceExporter") << "Stopped recording. Captured " << _frameCount << " frames. Saving to disk...";

    _saveAllFrames();

    ofLogNotice("ImageSequenceExporter") << "All frames saved successfully";
}

void ImageSequenceExporter::update(float deltaTime)
{
    if (!_isRecording) return;

    _elapsedTime += deltaTime;
    _timeSinceLastCapture += deltaTime;

    float captureInterval = 1.0f / _fps;

    if (_timeSinceLastCapture >= captureInterval) {
        _captureFrame();
        _timeSinceLastCapture = 0.0f;
    }

    if (_elapsedTime >= _duration) {
        stopRecording();
    }
}

void ImageSequenceExporter::_captureFrame()
{
    Viewport* viewport = nullptr;
    for (auto& vp : _viewportManager.getViewports()) {
        if (vp->getId() == _viewportId) {
            viewport = vp.get();
            break;
        }
    }

    if (!viewport) {
        ofLogError("ImageSequenceExporter") << "Viewport " << _viewportId << " not found";
        stopRecording();
        return;
    }

    ofTexture& texture = viewport->getTexture();
    ofPixels pixels;
    texture.readToPixels(pixels);
    pixels.mirror(true, false);

    _capturedFrames.push_back(pixels);
    _frameCount++;
}

void ImageSequenceExporter::_saveAllFrames()
{
    for (size_t i = 0; i < _capturedFrames.size(); ++i) {
        std::string filename = _generateFilename(i);
        std::string fullPath = _exportFolder + "/" + filename;

        if (!ofSaveImage(_capturedFrames[i], fullPath)) {
            ofLogError("ImageSequenceExporter") << "Failed to save frame " << i << ": " << fullPath;
        }
    }

    _capturedFrames.clear();
}

std::string ImageSequenceExporter::_generateFilename(int frameIndex)
{
    std::stringstream ss;
    ss << "capture_" << _sessionTimestamp << "_frame_"
       << std::setfill('0') << std::setw(4) << frameIndex
       << ".png";
    return ss.str();
}

std::string ImageSequenceExporter::_getCurrentTimestamp()
{
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return ss.str();
}

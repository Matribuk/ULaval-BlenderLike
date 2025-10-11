#include "Systems/ImageSequenceExporter.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

ImageSequenceExporter::ImageSequenceExporter(ViewportManager& viewportManager)
    : _viewportManager(viewportManager) {}

void ImageSequenceExporter::startRecording(ViewportID viewportId, const std::string& folder, int fps, float durationSeconds)
{
    if (this->_isRecording)
        return;

    this->_viewportId = viewportId;
    this->_exportFolder = folder;
    this->_fps = fps;
    this->_duration = durationSeconds;
    this->_elapsedTime = 0.0f;
    this->_frameCount = 0;
    this->_timeSinceLastCapture = 0.0f;
    this->_sessionTimestamp = this->_getCurrentTimestamp();

    this->_capturedFrames.clear();
    int estimatedFrames = static_cast<int>(fps * durationSeconds);
    this->_capturedFrames.reserve(estimatedFrames);

    this->_isRecording = true;
}

void ImageSequenceExporter::stopRecording()
{
    if (!this->_isRecording)
        return;

    this->_isRecording = false;
    this->_saveAllFrames();
}

void ImageSequenceExporter::update(float deltaTime)
{
    if (!this->_isRecording) return;

    this->_elapsedTime += deltaTime;
    this->_timeSinceLastCapture += deltaTime;

    float captureInterval = 1.0f / this->_fps;

    if (this->_timeSinceLastCapture >= captureInterval) {
        this->_captureFrame();
        this->_timeSinceLastCapture = 0.0f;
    }

    if (this->_elapsedTime >= this->_duration) {
        this->stopRecording();
    }
}

void ImageSequenceExporter::_captureFrame()
{
    Viewport* viewport = nullptr;
    for (std::unique_ptr<Viewport>& vp : this->_viewportManager.getViewports()) {
        if (vp->getId() == this->_viewportId) {
            viewport = vp.get();
            break;
        }
    }

    if (!viewport) {
        this->stopRecording();
        return;
    }

    ofTexture& texture = viewport->getTexture();
    ofPixels pixels;
    texture.readToPixels(pixels);
    pixels.mirror(true, false);

    this->_capturedFrames.push_back(pixels);
    this->_frameCount++;
}

void ImageSequenceExporter::_saveAllFrames()
{
    for (size_t i = 0; i < this->_capturedFrames.size(); ++i) {
        std::string filename = this->_generateFilename(i);
        std::string fullPath = this->_exportFolder + "/" + filename;
    }

    this->_capturedFrames.clear();
}

std::string ImageSequenceExporter::_generateFilename(int frameIndex)
{
    std::stringstream ss;
    ss << "capture_" << this->_sessionTimestamp << "_frame_"
       << std::setfill('0') << std::setw(4) << frameIndex
       << ".png";
    return ss.str();
}

std::string ImageSequenceExporter::_getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return ss.str();
}


bool ImageSequenceExporter::isRecording() const {
    return this->_isRecording;
}

int ImageSequenceExporter::getFrameCount() const {
    return this->_frameCount;
}

float ImageSequenceExporter::getElapsedTime() const {
    return this->_elapsedTime;
}

float ImageSequenceExporter::getDuration() const {
    return this->_duration;
}

std::string ImageSequenceExporter::getExportFolder() const {
    return this->_exportFolder;
}
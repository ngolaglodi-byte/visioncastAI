/// @file pipeline_manager.cpp
/// @brief PipelineManager implementation.

#include "visioncast/pipeline_manager.h"

#include <chrono>

#include "visioncast/capture_manager.h"
#include "visioncast/filter_chain.h"
#include "visioncast/output_manager.h"
#include "visioncast/overlay_renderer.h"

namespace visioncast {

PipelineManager::PipelineManager(CaptureManager& capture,
                                 FilterChain& filters,
                                 OverlayRenderer& overlays,
                                 OutputManager& output)
    : capture_(capture)
    , filters_(filters)
    , overlays_(overlays)
    , output_(output) {}

PipelineManager::~PipelineManager() {
    stop();
}

void PipelineManager::start() {
    if (running_.exchange(true)) {
        return; // Already running
    }
    pipelineThread_ = std::thread([this]() {
        const auto frameDuration =
            std::chrono::microseconds(static_cast<int64_t>(1000000.0 / targetFps_));

        while (running_) {
            auto start = std::chrono::steady_clock::now();
            processFrame();
            auto elapsed = std::chrono::steady_clock::now() - start;

            if (elapsed < frameDuration) {
                std::this_thread::sleep_for(frameDuration - elapsed);
            }
        }
    });
}

void PipelineManager::stop() {
    running_ = false;
    if (pipelineThread_.joinable()) {
        pipelineThread_.join();
    }
}

bool PipelineManager::isRunning() const {
    return running_;
}

void PipelineManager::processFrame() {
    // 1. Capture
    VideoFrame frame = capture_.captureActiveFrame();
    if (frame.data == nullptr) {
        return;
    }

    // 2. Apply filter chain
    frame = filters_.apply(frame);

    // 3. Overlay compositing (driven by metadata from Python AI)
    // TODO: Get metadata from MetadataReceiver and call overlays_.applyMetadata()

    // 4. Output to all destinations
    output_.sendFrame(frame);
}

void PipelineManager::setFrameRate(double fps) {
    targetFps_ = fps;
}

void PipelineManager::setResolution(int width, int height) {
    width_ = width;
    height_ = height;
}

} // namespace visioncast

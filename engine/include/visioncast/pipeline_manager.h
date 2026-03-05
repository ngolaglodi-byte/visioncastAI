#pragma once

/// @file pipeline_manager.h
/// @brief Orchestrates the full frame processing pipeline.

#include <atomic>
#include <thread>

namespace visioncast {

class CaptureManager;
class FilterChain;
class OverlayRenderer;
class OutputManager;

/// Orchestrates the full video processing pipeline: capture → filter → overlay → output.
class PipelineManager {
public:
    PipelineManager(CaptureManager& capture,
                    FilterChain& filters,
                    OverlayRenderer& overlays,
                    OutputManager& output);
    ~PipelineManager();

    /// Begin processing pipeline loop in background thread.
    void start();

    /// Stop processing.
    void stop();

    /// Check if the pipeline is currently running.
    bool isRunning() const;

    /// Process a single frame through the pipeline.
    void processFrame();

    /// Set target frame rate (e.g. 25.0, 29.97, 50.0, 59.94).
    void setFrameRate(double fps);

    /// Set output resolution (e.g. 3840x2160 for 4K).
    void setResolution(int width, int height);

private:
    CaptureManager& capture_;
    FilterChain& filters_;
    OverlayRenderer& overlays_;
    OutputManager& output_;

    std::atomic<bool> running_{false};
    std::thread pipelineThread_;
    double targetFps_ = 25.0;
    int width_ = 3840;
    int height_ = 2160;
};

} // namespace visioncast

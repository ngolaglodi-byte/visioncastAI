#pragma once

/// @file vision_engine.h
/// @brief Core vision engine: video capture, preview window, GPU pipeline,
///        GPU textures, and render loop.
///
/// Provides a unified high-level API that ties together live video capture,
/// an OpenGL (or Vulkan-ready) GPU rendering pipeline with texture upload,
/// a real-time preview window, and a frame-rate-governed render loop.
///
/// When built with HAS_OPENGL=1 (GLFW + OpenGL 3.3+), the engine uses a
/// native GPU path.  Otherwise it falls back to an OpenCV highgui preview.

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "visioncast/gpu_compositor.h"
#include "visioncast_sdk/video_device.h"

namespace visioncast {

// -----------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------

/// Start-up configuration for VisionEngine.
struct VisionEngineConfig {
    int captureDeviceIndex = 0;       ///< Camera/device index (used when captureUri is empty).
    std::string captureUri;           ///< File or stream URI; overrides captureDeviceIndex.
    int previewWidth  = 1280;         ///< Preview window width (pixels).
    int previewHeight = 720;          ///< Preview window height (pixels).
    std::string windowTitle = "VisionCast Preview"; ///< Preview window title.
    double targetFps  = 25.0;         ///< Target frame rate for the render loop.
    bool enableGpu    = true;         ///< Attempt GPU pipeline; fall back to CPU if unavailable.

    // OBS-like media-source behavior:
    // - true: file sources loop on EOF
    // - false: file sources stop the render loop on EOF (clean end)
    bool loopMedia = true;
};

// -----------------------------------------------------------------
// GPU texture handle
// -----------------------------------------------------------------

/// Lightweight handle to an OpenGL texture residing in VRAM.
struct GpuTexture {
    unsigned int id = 0;   ///< OpenGL texture name (0 = invalid).
    int width  = 0;
    int height = 0;
    bool valid = false;    ///< True after a successful upload.
};

// -----------------------------------------------------------------
// VisionEngine
// -----------------------------------------------------------------

/// Core vision engine combining capture, GPU pipeline, preview, and render loop.
///
/// Typical usage:
/// @code
///   VisionEngineConfig cfg;
///   cfg.captureDeviceIndex = 0;
///   cfg.targetFps = 30.0;
///
///   VisionEngine engine(cfg);
///   if (!engine.initialize()) { /* handle error */ }
///   engine.setFrameCallback([](VideoFrame& f) { /* per-frame processing */ });
///   engine.run();   // blocking – returns when stopped or window closed
///   engine.shutdown();
/// @endcode
class VisionEngine {
public:
    VisionEngine();
    explicit VisionEngine(const VisionEngineConfig& config);
    ~VisionEngine();

    // Non-copyable, non-movable.
    VisionEngine(const VisionEngine&) = delete;
    VisionEngine& operator=(const VisionEngine&) = delete;

    // ---- Lifecycle --------------------------------------------------

    /// Initialize capture + preview window + GPU pipeline in one call.
    bool initialize();

    /// Release all resources (capture, window, GPU).
    void shutdown();

    // ---- Video Capture ----------------------------------------------

    /// Open a capture device by index (e.g. webcam 0).
    bool openCapture(int deviceIndex);

    /// Open a capture source by URI (file path or stream URL).
    bool openCapture(const std::string& uri);

    /// Close the current capture source.
    void closeCapture();

    /// Check whether the capture source is open and delivering frames.
    bool isCaptureOpen() const;

    /// Grab the latest frame from the capture source into @p outFrame.
    bool grabFrame(VideoFrame& outFrame);

    // ---- Preview Window ---------------------------------------------

    /// Create the preview window with the given title and dimensions.
    bool createPreviewWindow(const std::string& title, int width, int height);

    /// Destroy the preview window.
    void destroyPreviewWindow();

    /// Check whether the preview window is still open.
    bool isPreviewOpen() const;

    // ---- GPU Pipeline (OpenGL / Vulkan-ready) -----------------------

    /// Initialize the GPU rendering pipeline (OpenGL context, shaders, VAO).
    bool initGpuPipeline();

    /// Shut down the GPU pipeline, releasing all GPU resources.
    void shutdownGpuPipeline();

    /// Check whether the GPU pipeline is active.
    bool isGpuReady() const;

    // ---- GPU Textures -----------------------------------------------

    /// Upload raw pixel data to a new GPU texture.
    /// @param data     Pointer to pixel data (BGRA8 or RGB8).
    /// @param width    Image width in pixels.
    /// @param height   Image height in pixels.
    /// @param channels Number of colour channels (3 = RGB, 4 = RGBA/BGRA).
    /// @return A GpuTexture handle (valid == true on success).
    GpuTexture uploadTexture(const uint8_t* data, int width, int height, int channels);

    /// Re-upload pixel data into an existing GPU texture (avoids re-allocation).
    bool updateTexture(GpuTexture& texture,
                       const uint8_t* data, int width, int height, int channels);

    /// Delete a GPU texture and free its VRAM.
    void deleteTexture(GpuTexture& texture);

    // ---- Render Loop ------------------------------------------------

    /// Start the render loop (blocking).  Returns when stop() is called or
    /// the preview window is closed.
    void run();

    /// Request the render loop to stop after the current frame.
    void stop();

    /// Check whether the render loop is currently running.
    bool isRunning() const;

    // ---- Configuration ----------------------------------------------

    /// Set the target frame rate for the render loop.
    void setFrameRate(double fps);

    /// Set the preview / output resolution.
    void setResolution(int width, int height);

    /// Per-frame processing callback (invoked each frame before rendering).
    using FrameCallback = std::function<void(VideoFrame&)>;
    void setFrameCallback(FrameCallback callback);

    // ---- GPU Compositor ---------------------------------------------

    /// Access the built-in GPU compositing pipeline (layers, animations,
    /// transitions, lower thirds, dynamic templates).
    GpuCompositor& compositor();
    const GpuCompositor& compositor() const;

private:
    /// Opaque implementation (PIMPL) — hides OpenCV / OpenGL / GLFW types.
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace visioncast
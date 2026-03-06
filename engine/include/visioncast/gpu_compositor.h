#pragma once

/// @file gpu_compositor.h
/// @brief GPU compositing pipeline for overlay layers, PNG alpha blending,
///        animations, transitions, lower thirds, and dynamic templates.
///
/// The GpuCompositor manages an ordered stack of compositing layers.  Each
/// layer holds an RGBA texture (e.g. a PNG with transparency) together with
/// transform, opacity, and animation state.  Every frame the compositor
/// evaluates pending animations / transitions, uploads any dirty textures,
/// and renders the layer stack with alpha blending on the GPU.
///
/// When built *without* HAS_OPENGL the compositor falls back to CPU-based
/// OpenCV alpha compositing so the API surface remains identical.

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "visioncast_sdk/video_device.h"

namespace visioncast {

// -----------------------------------------------------------------
// Easing & Animation
// -----------------------------------------------------------------

/// Easing functions used by animations and transitions.
enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut
};

/// Supported animation types for layers.
enum class AnimationType {
    None,
    FadeIn,
    FadeOut,
    SlideLeft,
    SlideRight,
    SlideUp,
    SlideDown,
    ScaleIn,
    ScaleOut
};

/// Supported transition types between scenes or states.
enum class TransitionType {
    None,
    Cut,
    Fade,
    WipeLeft,
    WipeRight,
    WipeUp,
    WipeDown,
    Dissolve
};

/// Describes a single animation applied to a compositing layer.
struct AnimationDesc {
    AnimationType type       = AnimationType::None;
    EasingType    easing     = EasingType::EaseInOut;
    double        durationMs = 500.0;  ///< Duration in milliseconds.
    double        delayMs    = 0.0;    ///< Delay before the animation starts.
};

/// Describes a transition between two states.
struct TransitionDesc {
    TransitionType type       = TransitionType::None;
    EasingType     easing     = EasingType::EaseInOut;
    double         durationMs = 500.0;
};

// -----------------------------------------------------------------
// Lower-Third Configuration
// -----------------------------------------------------------------

/// Visual configuration for a lower-third overlay.
struct LowerThirdDesc {
    std::string title;                       ///< Main title text.
    std::string subtitle;                    ///< Subtitle / role text.
    float backgroundOpacity  = 0.7f;
    uint8_t bgColor[4]       = {0, 0, 0, 180};    ///< BGRA.
    uint8_t textColor[4]     = {255, 255, 255, 255};
    uint8_t subtitleColor[4] = {200, 200, 200, 255};
    float   fontScale        = 1.0f;
    int     thickness        = 2;
    float   bandHeightRatio  = 0.15f;       ///< Fraction of frame height.
    AnimationDesc entryAnim;
    AnimationDesc exitAnim;
};

// -----------------------------------------------------------------
// Compositing Layer
// -----------------------------------------------------------------

/// A single compositing layer in the GPU stack.
struct CompositingLayer {
    std::string id;              ///< Unique identifier for the layer.
    int         zOrder  = 0;     ///< Draw order (lower = behind).
    float       opacity = 1.0f;  ///< Layer-global opacity [0, 1].

    /// Position in normalised coordinates [0, 1] relative to the output frame.
    float posX = 0.0f;
    float posY = 0.0f;

    /// Size in normalised coordinates (1.0 = full width/height).
    float width  = 1.0f;
    float height = 1.0f;

    bool visible = true;

    /// Raw BGRA pixel data (CPU side, uploaded to GPU when dirty).
    std::vector<uint8_t> pixels;
    int texWidth  = 0;
    int texHeight = 0;
    int channels  = 4;

    /// Animation state.
    AnimationDesc animation;
    double        animElapsedMs = 0.0;
    bool          animActive    = false;
};

// -----------------------------------------------------------------
// Dynamic Template
// -----------------------------------------------------------------

/// A dynamic overlay template loaded from JSON.
struct DynamicTemplate {
    std::string name;
    std::string description;

    /// Lower-third style pulled from the template.
    LowerThirdDesc lowerThird;

    /// Entry / exit animation descriptors.
    AnimationDesc entryAnimation;
    AnimationDesc exitAnimation;

    /// Default transition when switching templates.
    TransitionDesc transition;

    /// Path to an optional logo PNG.
    std::string logoPath;

    /// Arbitrary key-value pairs for user-defined fields.
    std::vector<std::pair<std::string, std::string>> fields;
};

// -----------------------------------------------------------------
// GpuCompositor
// -----------------------------------------------------------------

/// GPU-accelerated compositing pipeline.
///
/// Typical lifecycle:
/// @code
///   GpuCompositor comp;
///   comp.initialize(1920, 1080);
///   comp.loadTemplate("overlays/templates/default.json");
///   comp.addPngLayer("logo", "assets/logo.png", 0.9f, 0.05f, 0.1f, 0.06f, 10);
///   comp.showLowerThird("Alice", "Host", 5000.0);
///   // In the render loop:
///   comp.update(deltaMs);
///   comp.composite(frame);
///   // Cleanup:
///   comp.shutdown();
/// @endcode
class GpuCompositor {
public:
    GpuCompositor();
    ~GpuCompositor();

    // Non-copyable.
    GpuCompositor(const GpuCompositor&) = delete;
    GpuCompositor& operator=(const GpuCompositor&) = delete;

    // ---- Lifecycle --------------------------------------------------

    /// Initialise internal state for the given output resolution.
    bool initialize(int width, int height);

    /// Release all resources.
    void shutdown();

    /// @return true when the compositor is ready to composite.
    bool isReady() const;

    // ---- Layer Management -------------------------------------------

    /// Add a layer with raw BGRA pixel data.
    /// @return true on success.
    bool addLayer(const std::string& id,
                  const uint8_t* bgraData, int texW, int texH,
                  float posX, float posY, float w, float h,
                  int zOrder = 0);

    /// Convenience: load a PNG file (with alpha) as a layer.
    bool addPngLayer(const std::string& id,
                     const std::string& pngPath,
                     float posX, float posY, float w, float h,
                     int zOrder = 0);

    /// Remove a layer by id.
    void removeLayer(const std::string& id);

    /// Update a layer's pixel data (e.g. animated sprite sheet).
    bool updateLayerPixels(const std::string& id,
                           const uint8_t* bgraData, int texW, int texH);

    /// Set layer visibility.
    void setLayerVisible(const std::string& id, bool visible);

    /// Set layer opacity.
    void setLayerOpacity(const std::string& id, float opacity);

    // ---- Animations -------------------------------------------------

    /// Start an animation on a layer.
    void animateLayer(const std::string& id, const AnimationDesc& anim);

    // ---- Transitions ------------------------------------------------

    /// Begin a transition between the current and next state.
    void startTransition(const TransitionDesc& transition);

    /// @return true while a transition is in progress.
    bool isTransitioning() const;

    // ---- Lower Thirds -----------------------------------------------

    /// Show a lower-third overlay for @p durationMs milliseconds.
    /// Uses the current template's style or the supplied @p desc.
    void showLowerThird(const std::string& title,
                        const std::string& subtitle,
                        double durationMs = 5000.0,
                        const LowerThirdDesc* desc = nullptr);

    /// Immediately hide any active lower-third.
    void hideLowerThird();

    // ---- Dynamic Templates ------------------------------------------

    /// Load a template from a JSON file and store it by name.
    bool loadTemplate(const std::string& jsonPath);

    /// Activate a previously loaded template by name.
    bool activateTemplate(const std::string& name);

    /// @return pointer to the active template, or nullptr if none.
    const DynamicTemplate* activeTemplate() const;

    /// @return list of all loaded template names.
    std::vector<std::string> templateNames() const;

    // ---- Per-Frame --------------------------------------------------

    /// Advance animation / transition clocks by @p deltaMs milliseconds.
    void update(double deltaMs);

    /// Composite all visible layers onto @p frame (CPU fallback path).
    void composite(VideoFrame& frame);

#ifdef HAS_OPENGL
    /// Composite all visible layers using GPU (OpenGL).
    /// Call between glClear / swap-buffers in the render loop.
    void compositeGpu(unsigned int baseTextureId, int frameW, int frameH);
#endif

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace visioncast

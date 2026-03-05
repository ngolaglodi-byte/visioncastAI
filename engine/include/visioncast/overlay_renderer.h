#pragma once

/// @file overlay_renderer.h
/// @brief Composites overlay graphics (lower-thirds, logos) onto video frames.

#include <map>
#include <string>

#include "visioncast_sdk/video_device.h"

namespace visioncast {

/// Style configuration for overlay rendering.
struct OverlayStyle {
    float backgroundOpacity = 0.6f;
    int backgroundColor[3] = {0, 0, 0};    // BGR
    int textColor[3] = {255, 255, 255};     // BGR
    int subtitleColor[3] = {200, 200, 200}; // BGR
    float fontScale = 1.0f;
    int thickness = 2;
    float bandHeightRatio = 0.18f;          // Fraction of frame height
};

/// Template definition loaded from JSON overlay files.
struct OverlayTemplate {
    std::string name;
    OverlayStyle style;
    std::string logoPath;
};

/// Position for logo placement.
enum class Position {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

/// Recognition metadata received from the Python AI module.
struct RecognitionMetadata {
    struct Face {
        int top, right, bottom, left;
        std::string talentId;
        std::string talentName;
        std::string talentRole;
        std::string overlayTemplate;
        float confidence;
    };
    int64_t timestampMs = 0;
    int frameId = 0;
    std::vector<Face> faces;
};

/// Composites overlay graphics onto video frames based on AI metadata.
class OverlayRenderer {
public:
    OverlayRenderer();
    ~OverlayRenderer();

    /// Load an overlay template from a JSON file.
    bool loadTemplate(const std::string& templatePath);

    /// Render a lower-third overlay onto the frame.
    void renderLowerThird(VideoFrame& frame,
                          const std::string& name,
                          const std::string& role,
                          const OverlayStyle& style);

    /// Render a logo at a specified position.
    void renderLogo(VideoFrame& frame,
                    const std::string& logoPath,
                    Position position);

    /// Apply overlays based on metadata from the Python AI module.
    void applyMetadata(VideoFrame& frame,
                       const RecognitionMetadata& metadata);

private:
    std::map<std::string, OverlayTemplate> templates_;
};

} // namespace visioncast

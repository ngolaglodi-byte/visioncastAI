/// @file overlay_renderer.cpp
/// @brief OverlayRenderer implementation.

#include "visioncast/overlay_renderer.h"

#include <iostream>

namespace visioncast {

OverlayRenderer::OverlayRenderer() = default;
OverlayRenderer::~OverlayRenderer() = default;

bool OverlayRenderer::loadTemplate(const std::string& templatePath) {
    // TODO: Parse JSON template file and populate templates_ map
    std::cout << "[OverlayRenderer] Loading template: " << templatePath << std::endl;
    return true;
}

void OverlayRenderer::renderLowerThird(VideoFrame& frame,
                                        const std::string& name,
                                        const std::string& role,
                                        const OverlayStyle& style) {
    if (frame.data == nullptr) {
        return;
    }
    // TODO: Implement GPU-accelerated lower-third rendering
    // Similar logic to python/main.py draw_lower_third but in C++/OpenCV
}

void OverlayRenderer::renderLogo(VideoFrame& frame,
                                  const std::string& logoPath,
                                  Position position) {
    if (frame.data == nullptr) {
        return;
    }
    // TODO: Load logo image and composite at specified position
}

void OverlayRenderer::applyMetadata(VideoFrame& frame,
                                     const RecognitionMetadata& metadata) {
    OverlayStyle defaultStyle;
    for (const auto& face : metadata.faces) {
        if (!face.talentName.empty()) {
            renderLowerThird(frame, face.talentName, face.talentRole, defaultStyle);
        }
    }
}

} // namespace visioncast

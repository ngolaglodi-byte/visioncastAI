/// @file main.cpp
/// @brief VisionCast Engine entry point.

#include <iostream>

#include "visioncast/capture_manager.h"
#include "visioncast/filter_chain.h"
#include "visioncast/output_manager.h"
#include "visioncast/overlay_renderer.h"
#include "visioncast/pipeline_manager.h"
#include "visioncast/metadata_receiver.h"

int main(int argc, char* argv[]) {
    std::cout << "[VisionCast Engine] Starting..." << std::endl;

    visioncast::CaptureManager capture;
    visioncast::FilterChain filters;
    visioncast::OverlayRenderer overlays;
    visioncast::OutputManager output;

    visioncast::PipelineManager pipeline(capture, filters, overlays, output);
    pipeline.setResolution(3840, 2160);
    pipeline.setFrameRate(25.0);

    // TODO: Initialize SDK devices, metadata receiver, and start pipeline
    std::cout << "[VisionCast Engine] Initialized (4K @ 25fps)." << std::endl;
    std::cout << "[VisionCast Engine] Waiting for implementation..." << std::endl;

    return 0;
}

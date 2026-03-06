/// @file main.cpp
/// @brief VisionCast Engine entry point.

#include <iostream>
#include <string>

#include "visioncast/capture_manager.h"
#include "visioncast/filter_chain.h"
#include "visioncast/output_manager.h"
#include "visioncast/overlay_renderer.h"
#include "visioncast/pipeline_manager.h"
#include "visioncast/metadata_receiver.h"
#include "visioncast/vision_engine.h"

int main(int argc, char* argv[]) {
    std::cout << "[VisionCast Engine] Starting..." << std::endl;

    // --- Classic pipeline (SDK devices, filters, overlays, output) ---
    visioncast::CaptureManager capture;
    visioncast::FilterChain filters;
    visioncast::OverlayRenderer overlays;
    visioncast::OutputManager output;

    visioncast::PipelineManager pipeline(capture, filters, overlays, output);
    pipeline.setResolution(3840, 2160);
    pipeline.setFrameRate(25.0);

    // --- VisionEngine: capture + GPU preview + render loop -----------
    visioncast::VisionEngineConfig cfg;
    cfg.captureDeviceIndex = 0;
    cfg.previewWidth       = 1280;
    cfg.previewHeight      = 720;
    cfg.targetFps          = 25.0;
    cfg.windowTitle        = "VisionCast Preview";

    // Allow overriding capture source via command-line argument.
    if (argc > 1) {
        cfg.captureUri = argv[1];
    }

    visioncast::VisionEngine engine(cfg);

    if (!engine.initialize()) {
        std::cerr << "[VisionCast Engine] VisionEngine initialization failed." << std::endl;
        std::cout << "[VisionCast Engine] Continuing without VisionEngine preview."
                  << std::endl;
    } else {
        // Optional: per-frame callback for custom processing.
        engine.setFrameCallback([](VideoFrame& frame) {
            // Placeholder: additional per-frame processing can be added here.
            (void)frame;
        });

        // Blocking render loop — runs until window is closed or ESC pressed.
        engine.run();
        engine.shutdown();
    }

    std::cout << "[VisionCast Engine] Done." << std::endl;
    return 0;
}

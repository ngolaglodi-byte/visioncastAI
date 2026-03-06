#pragma once

/// @file video_filters.h
/// @brief Global video filters: Cinema LUT, Sharpen, Noise Reduction,
///        HDR Tonemapping, and Dynamic Contrast.
///
/// Each filter implements the IVideoFilter interface and can be inserted
/// into a FilterChain.  All filters operate on BGRA8 VideoFrames using
/// OpenCV and maintain an internal buffer to avoid per-frame allocation.

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "visioncast/filter_chain.h"

namespace visioncast {

// ======================================================================
// LutFilter — Cinema 3D LUT color grading
// ======================================================================

/// Applies a 3D colour Look-Up Table for cinema-grade colour grading.
///
/// An external .cube LUT file can be loaded, or a built-in cinematic grade
/// (warm highlights, teal shadows, lifted blacks) is applied by default.
class LutFilter : public IVideoFilter {
public:
    /// @param lutPath    Optional path to a .cube 3D-LUT file.
    /// @param intensity  Blend between original (0) and graded (1).
    explicit LutFilter(const std::string& lutPath = "", float intensity = 1.0f);
    ~LutFilter() override;

    std::string name() const override;
    VideoFrame process(const VideoFrame& input) override;
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;

    /// Load a .cube 3D-LUT file.  Returns false on parse error.
    bool loadCubeLut(const std::string& path);

    void setIntensity(float intensity);
    float intensity() const;

private:
    void buildDefaultLut();
    cv::Mat applyLut3D(const cv::Mat& bgr) const;

    bool enabled_ = true;
    float intensity_ = 1.0f;
    int lutSize_ = 0;
    std::vector<cv::Vec3f> lut3d_;
    cv::Mat buffer_;
};

// ======================================================================
// SharpenFilter — Unsharp-mask sharpening
// ======================================================================

/// Applies unsharp-mask sharpening to increase perceived detail.
class SharpenFilter : public IVideoFilter {
public:
    /// @param strength    Sharpening amount [0.0, 5.0].
    /// @param kernelSize  Gaussian kernel size (odd, >= 1).
    explicit SharpenFilter(float strength = 1.0f, int kernelSize = 3);
    ~SharpenFilter() override;

    std::string name() const override;
    VideoFrame process(const VideoFrame& input) override;
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;

    void setStrength(float strength);
    float strength() const;
    void setKernelSize(int size);
    int kernelSize() const;

private:
    bool enabled_ = true;
    float strength_ = 1.0f;
    int kernelSize_ = 3;
    cv::Mat buffer_;
};

// ======================================================================
// NoiseReductionFilter — Edge-preserving bilateral filtering
// ======================================================================

/// Reduces noise while preserving edges using a bilateral filter.
class NoiseReductionFilter : public IVideoFilter {
public:
    /// @param strength    Filter diameter (pixels, 1–50).
    /// @param sigmaColor  Colour-space sigma (1–300).
    /// @param sigmaSpace  Spatial sigma (1–300).
    explicit NoiseReductionFilter(int strength = 7,
                                  double sigmaColor = 75.0,
                                  double sigmaSpace = 75.0);
    ~NoiseReductionFilter() override;

    std::string name() const override;
    VideoFrame process(const VideoFrame& input) override;
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;

    void setStrength(int strength);
    int strength() const;
    void setSigmaColor(double sigma);
    double sigmaColor() const;
    void setSigmaSpace(double sigma);
    double sigmaSpace() const;

private:
    bool enabled_ = true;
    int strength_ = 7;
    double sigmaColor_ = 75.0;
    double sigmaSpace_ = 75.0;
    cv::Mat buffer_;
};

// ======================================================================
// HdrTonemapFilter — HDR-to-SDR tonemapping
// ======================================================================

/// Maps HDR-range frames into displayable SDR range using a
/// Reinhard-inspired tonemapping curve with gamma and saturation control.
class HdrTonemapFilter : public IVideoFilter {
public:
    /// @param gamma       Display gamma [0.1, 5.0].
    /// @param saturation  Colour saturation multiplier [0.0, 3.0].
    explicit HdrTonemapFilter(float gamma = 1.0f, float saturation = 1.0f);
    ~HdrTonemapFilter() override;

    std::string name() const override;
    VideoFrame process(const VideoFrame& input) override;
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;

    void setGamma(float gamma);
    float gamma() const;
    void setSaturation(float saturation);
    float saturation() const;

private:
    bool enabled_ = true;
    float gamma_ = 1.0f;
    float saturation_ = 1.0f;
    cv::Mat buffer_;
};

// ======================================================================
// DynamicContrastFilter — CLAHE adaptive contrast
// ======================================================================

/// Enhances local contrast via Contrast Limited Adaptive Histogram
/// Equalization (CLAHE) on the luminance channel.
class DynamicContrastFilter : public IVideoFilter {
public:
    /// @param clipLimit     Contrast clip limit [1.0, 40.0].
    /// @param tileGridSize  Number of tiles per dimension [2, 32].
    explicit DynamicContrastFilter(double clipLimit = 2.0,
                                   int tileGridSize = 8);
    ~DynamicContrastFilter() override;

    std::string name() const override;
    VideoFrame process(const VideoFrame& input) override;
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;

    void setClipLimit(double limit);
    double clipLimit() const;
    void setTileGridSize(int size);
    int tileGridSize() const;

private:
    bool enabled_ = true;
    double clipLimit_ = 2.0;
    int tileGridSize_ = 8;
    cv::Mat buffer_;
};

} // namespace visioncast

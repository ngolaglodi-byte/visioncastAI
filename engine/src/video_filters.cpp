/// @file video_filters.cpp
/// @brief Implementation of global video filters.
///
/// All filters wrap the incoming VideoFrame in a cv::Mat (zero-copy for
/// BGRA8), process through OpenCV, and return a VideoFrame that points to
/// the filter's internal buffer.

#include "visioncast/video_filters.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#include <opencv2/imgproc.hpp>

namespace visioncast {

// ======================================================================
// Helpers
// ======================================================================

namespace {

/// Map PixelFormat to OpenCV type constant.
int cvTypeFor(PixelFormat fmt) {
    switch (fmt) {
        case PixelFormat::BGRA8: return CV_8UC4;
        case PixelFormat::NV12:  return CV_8UC1; // luma plane only
        default:                 return CV_8UC4;
    }
}

/// Wrap a VideoFrame in a cv::Mat (zero-copy).
cv::Mat wrapFrame(const VideoFrame& f) {
    return cv::Mat(f.height, f.width, cvTypeFor(f.format),
                   const_cast<uint8_t*>(f.data),
                   static_cast<size_t>(f.stride));
}

/// Build an output VideoFrame that points to the given cv::Mat buffer.
VideoFrame makeOutput(const VideoFrame& ref, cv::Mat& buf) {
    VideoFrame out = ref;
    out.data   = buf.data;
    out.stride = static_cast<int>(buf.step);
    return out;
}

} // anonymous namespace

// ======================================================================
// LutFilter
// ======================================================================

LutFilter::LutFilter(const std::string& lutPath, float intensity)
    : intensity_(std::clamp(intensity, 0.0f, 1.0f)) {
    if (!lutPath.empty()) {
        loadCubeLut(lutPath);
    }
    if (lut3d_.empty()) {
        buildDefaultLut();
    }
}

LutFilter::~LutFilter() = default;

std::string LutFilter::name() const { return "cinema_lut"; }

void LutFilter::setEnabled(bool e) { enabled_ = e; }
bool LutFilter::isEnabled() const   { return enabled_; }

void LutFilter::setIntensity(float v) { intensity_ = std::clamp(v, 0.0f, 1.0f); }
float LutFilter::intensity() const     { return intensity_; }

bool LutFilter::loadCubeLut(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;

    std::string line;
    int size = 0;
    std::vector<cv::Vec3f> data;

    while (std::getline(ifs, line)) {
        // Strip comments / metadata.
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("LUT_3D_SIZE", 0) == 0) {
            std::istringstream ss(line.substr(11));
            ss >> size;
            data.reserve(static_cast<size_t>(size * size * size));
            continue;
        }

        // Skip other keywords.
        if (line.rfind("TITLE", 0) == 0 ||
            line.rfind("DOMAIN_MIN", 0) == 0 ||
            line.rfind("DOMAIN_MAX", 0) == 0) {
            continue;
        }

        // Parse R G B triplet.
        float r, g, b;
        std::istringstream ss(line);
        if (ss >> r >> g >> b) {
            data.emplace_back(r, g, b);
        }
    }

    if (size < 2 || data.size() != static_cast<size_t>(size * size * size)) {
        return false;
    }

    lutSize_ = size;
    lut3d_   = std::move(data);
    return true;
}

void LutFilter::buildDefaultLut() {
    // Built-in cinematic grade: warm highlights, teal shadows, lifted blacks.
    const int N = 17; // 17^3 LUT — good balance of precision vs. memory.
    lutSize_ = N;
    lut3d_.resize(static_cast<size_t>(N * N * N));

    for (int ib = 0; ib < N; ++ib) {
        for (int ig = 0; ig < N; ++ig) {
            for (int ir = 0; ir < N; ++ir) {
                float r = static_cast<float>(ir) / (N - 1);
                float g = static_cast<float>(ig) / (N - 1);
                float b = static_cast<float>(ib) / (N - 1);

                // Lift blacks slightly.
                const float lift = 0.03f;
                r = lift + r * (1.0f - lift);
                g = lift + g * (1.0f - lift);
                b = lift + b * (1.0f - lift);

                // Warm highlights (push red/green up in bright areas).
                float lum = 0.2126f * r + 0.7152f * g + 0.0722f * b;
                r += 0.04f * lum;
                g += 0.02f * lum;

                // Teal shadows (push blue/green up in dark areas).
                float shadow = 1.0f - lum;
                g += 0.02f * shadow;
                b += 0.05f * shadow;

                // Mild S-curve contrast.
                auto sCurve = [](float v) {
                    v = std::clamp(v, 0.0f, 1.0f);
                    return v * v * (3.0f - 2.0f * v); // smoothstep
                };
                r = sCurve(r);
                g = sCurve(g);
                b = sCurve(b);

                size_t idx = static_cast<size_t>(
                    ib * N * N + ig * N + ir);
                lut3d_[idx] = cv::Vec3f(r, g, b);
            }
        }
    }
}

cv::Mat LutFilter::applyLut3D(const cv::Mat& bgr) const {
    cv::Mat out(bgr.size(), bgr.type());
    const int N = lutSize_;
    const float scale = static_cast<float>(N - 1);

    for (int y = 0; y < bgr.rows; ++y) {
        const auto* srcRow = bgr.ptr<cv::Vec3b>(y);
        auto*       dstRow = out.ptr<cv::Vec3b>(y);
        for (int x = 0; x < bgr.cols; ++x) {
            // OpenCV BGR → LUT RGB index.
            float rf = srcRow[x][2] / 255.0f * scale;
            float gf = srcRow[x][1] / 255.0f * scale;
            float bf = srcRow[x][0] / 255.0f * scale;

            int ri = std::min(static_cast<int>(rf), N - 2);
            int gi = std::min(static_cast<int>(gf), N - 2);
            int bi = std::min(static_cast<int>(bf), N - 2);
            float rd = rf - ri, gd = gf - gi, bd = bf - bi;

            // Trilinear interpolation.
            auto idx = [&](int ir, int ig, int ib) -> size_t {
                return static_cast<size_t>(ib * N * N + ig * N + ir);
            };
            cv::Vec3f c000 = lut3d_[idx(ri,     gi,     bi    )];
            cv::Vec3f c100 = lut3d_[idx(ri + 1, gi,     bi    )];
            cv::Vec3f c010 = lut3d_[idx(ri,     gi + 1, bi    )];
            cv::Vec3f c110 = lut3d_[idx(ri + 1, gi + 1, bi    )];
            cv::Vec3f c001 = lut3d_[idx(ri,     gi,     bi + 1)];
            cv::Vec3f c101 = lut3d_[idx(ri + 1, gi,     bi + 1)];
            cv::Vec3f c011 = lut3d_[idx(ri,     gi + 1, bi + 1)];
            cv::Vec3f c111 = lut3d_[idx(ri + 1, gi + 1, bi + 1)];

            cv::Vec3f c00 = c000 * (1 - rd) + c100 * rd;
            cv::Vec3f c01 = c001 * (1 - rd) + c101 * rd;
            cv::Vec3f c10 = c010 * (1 - rd) + c110 * rd;
            cv::Vec3f c11 = c011 * (1 - rd) + c111 * rd;

            cv::Vec3f c0 = c00 * (1 - gd) + c10 * gd;
            cv::Vec3f c1 = c01 * (1 - gd) + c11 * gd;

            cv::Vec3f c = c0 * (1 - bd) + c1 * bd;

            // LUT RGB → OpenCV BGR, back to 8-bit.
            dstRow[x][0] = static_cast<uint8_t>(std::clamp(c[2] * 255.0f, 0.0f, 255.0f));
            dstRow[x][1] = static_cast<uint8_t>(std::clamp(c[1] * 255.0f, 0.0f, 255.0f));
            dstRow[x][2] = static_cast<uint8_t>(std::clamp(c[0] * 255.0f, 0.0f, 255.0f));
        }
    }
    return out;
}

VideoFrame LutFilter::process(const VideoFrame& input) {
    if (!enabled_ || input.data == nullptr) return input;

    cv::Mat src = wrapFrame(input);
    cv::Mat bgr;

    // Convert to BGR for LUT processing.
    if (input.format == PixelFormat::BGRA8) {
        cv::cvtColor(src, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = src.clone();
    }

    cv::Mat graded = applyLut3D(bgr);

    // Blend original and graded based on intensity.
    if (intensity_ < 1.0f) {
        cv::addWeighted(bgr, 1.0 - intensity_, graded, intensity_, 0.0, graded);
    }

    // Convert back to original format.
    if (input.format == PixelFormat::BGRA8) {
        cv::cvtColor(graded, buffer_, cv::COLOR_BGR2BGRA);
    } else {
        buffer_ = std::move(graded);
    }

    return makeOutput(input, buffer_);
}

// ======================================================================
// SharpenFilter
// ======================================================================

SharpenFilter::SharpenFilter(float strength, int kernelSize)
    : strength_(std::clamp(strength, 0.0f, 5.0f))
    , kernelSize_(kernelSize | 1) {} // Ensure odd.

SharpenFilter::~SharpenFilter() = default;

std::string SharpenFilter::name() const { return "sharpen"; }

void SharpenFilter::setEnabled(bool e) { enabled_ = e; }
bool SharpenFilter::isEnabled() const   { return enabled_; }

void SharpenFilter::setStrength(float v) { strength_ = std::clamp(v, 0.0f, 5.0f); }
float SharpenFilter::strength() const     { return strength_; }

void SharpenFilter::setKernelSize(int s) { kernelSize_ = std::max(1, s) | 1; }
int SharpenFilter::kernelSize() const     { return kernelSize_; }

VideoFrame SharpenFilter::process(const VideoFrame& input) {
    if (!enabled_ || input.data == nullptr) return input;

    cv::Mat src = wrapFrame(input);

    // Unsharp mask: sharpened = src + strength * (src - blurred).
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred,
                     cv::Size(kernelSize_, kernelSize_), 0);
    cv::addWeighted(src, 1.0 + strength_, blurred, -strength_, 0, buffer_);

    return makeOutput(input, buffer_);
}

// ======================================================================
// NoiseReductionFilter
// ======================================================================

NoiseReductionFilter::NoiseReductionFilter(int strength,
                                           double sigmaColor,
                                           double sigmaSpace)
    : strength_(std::clamp(strength, 1, 50))
    , sigmaColor_(std::clamp(sigmaColor, 1.0, 300.0))
    , sigmaSpace_(std::clamp(sigmaSpace, 1.0, 300.0)) {}

NoiseReductionFilter::~NoiseReductionFilter() = default;

std::string NoiseReductionFilter::name() const { return "noise_reduction"; }

void NoiseReductionFilter::setEnabled(bool e) { enabled_ = e; }
bool NoiseReductionFilter::isEnabled() const   { return enabled_; }

void NoiseReductionFilter::setStrength(int v) { strength_ = std::clamp(v, 1, 50); }
int NoiseReductionFilter::strength() const     { return strength_; }

void NoiseReductionFilter::setSigmaColor(double v) { sigmaColor_ = std::clamp(v, 1.0, 300.0); }
double NoiseReductionFilter::sigmaColor() const     { return sigmaColor_; }

void NoiseReductionFilter::setSigmaSpace(double v) { sigmaSpace_ = std::clamp(v, 1.0, 300.0); }
double NoiseReductionFilter::sigmaSpace() const     { return sigmaSpace_; }

VideoFrame NoiseReductionFilter::process(const VideoFrame& input) {
    if (!enabled_ || input.data == nullptr) return input;

    cv::Mat src = wrapFrame(input);

    // Bilateral filter preserves edges while smoothing noise.
    cv::bilateralFilter(src, buffer_, strength_, sigmaColor_, sigmaSpace_);

    return makeOutput(input, buffer_);
}

// ======================================================================
// HdrTonemapFilter
// ======================================================================

HdrTonemapFilter::HdrTonemapFilter(float gamma, float saturation)
    : gamma_(std::clamp(gamma, 0.1f, 5.0f))
    , saturation_(std::clamp(saturation, 0.0f, 3.0f)) {}

HdrTonemapFilter::~HdrTonemapFilter() = default;

std::string HdrTonemapFilter::name() const { return "hdr_tonemap"; }

void HdrTonemapFilter::setEnabled(bool e) { enabled_ = e; }
bool HdrTonemapFilter::isEnabled() const   { return enabled_; }

void HdrTonemapFilter::setGamma(float v) { gamma_ = std::clamp(v, 0.1f, 5.0f); }
float HdrTonemapFilter::gamma() const     { return gamma_; }

void HdrTonemapFilter::setSaturation(float v) { saturation_ = std::clamp(v, 0.0f, 3.0f); }
float HdrTonemapFilter::saturation() const     { return saturation_; }

VideoFrame HdrTonemapFilter::process(const VideoFrame& input) {
    if (!enabled_ || input.data == nullptr) return input;

    cv::Mat src = wrapFrame(input);

    // Convert to 32-bit float normalized [0,1].
    cv::Mat floatImg;
    src.convertTo(floatImg, CV_32F, 1.0 / 255.0);

    // Split channels.
    std::vector<cv::Mat> channels;
    cv::split(floatImg, channels);

    // Compute luminance (BT.709 coefficients, in BGR order).
    cv::Mat lum = 0.0722f * channels[0]   // B
               + 0.7152f * channels[1]    // G
               + 0.2126f * channels[2];   // R

    // Reinhard global tonemapping: L_mapped = L / (1 + L).
    cv::Mat lumMapped;
    cv::divide(lum, lum + 1.0f, lumMapped);

    // Apply gamma correction.
    float invGamma = 1.0f / gamma_;
    cv::pow(lumMapped, invGamma, lumMapped);

    // Rebuild color channels with saturation control.
    // ratio = lumMapped / (lum + epsilon)
    cv::Mat ratio;
    cv::divide(lumMapped, lum + 1e-6f, ratio);

    int numChannels = static_cast<int>(channels.size());
    for (int i = 0; i < numChannels; ++i) {
        // Desaturated = lumMapped, fully saturated = channels[i] * ratio
        cv::Mat saturated = channels[i].mul(ratio);
        cv::addWeighted(lumMapped, 1.0 - saturation_,
                        saturated, saturation_, 0.0, channels[i]);
    }

    // Preserve original alpha if BGRA.
    if (input.format == PixelFormat::BGRA8 && numChannels == 4) {
        // Alpha was divided by 255 — restore it.
        channels[3] *= 255.0f;
    }

    cv::merge(channels, floatImg);

    // Convert back to 8-bit.
    floatImg.convertTo(buffer_, src.type(), 255.0);

    return makeOutput(input, buffer_);
}

// ======================================================================
// DynamicContrastFilter
// ======================================================================

DynamicContrastFilter::DynamicContrastFilter(double clipLimit, int tileGridSize)
    : clipLimit_(std::clamp(clipLimit, 1.0, 40.0))
    , tileGridSize_(std::clamp(tileGridSize, 2, 32)) {}

DynamicContrastFilter::~DynamicContrastFilter() = default;

std::string DynamicContrastFilter::name() const { return "dynamic_contrast"; }

void DynamicContrastFilter::setEnabled(bool e) { enabled_ = e; }
bool DynamicContrastFilter::isEnabled() const   { return enabled_; }

void DynamicContrastFilter::setClipLimit(double v) { clipLimit_ = std::clamp(v, 1.0, 40.0); }
double DynamicContrastFilter::clipLimit() const     { return clipLimit_; }

void DynamicContrastFilter::setTileGridSize(int v) { tileGridSize_ = std::clamp(v, 2, 32); }
int DynamicContrastFilter::tileGridSize() const     { return tileGridSize_; }

VideoFrame DynamicContrastFilter::process(const VideoFrame& input) {
    if (!enabled_ || input.data == nullptr) return input;

    cv::Mat src = wrapFrame(input);
    cv::Mat bgr;

    // CLAHE operates on a single channel — apply to L channel in Lab space.
    if (input.format == PixelFormat::BGRA8) {
        cv::cvtColor(src, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = src.clone();
    }

    cv::Mat lab;
    cv::cvtColor(bgr, lab, cv::COLOR_BGR2Lab);

    std::vector<cv::Mat> labChannels;
    cv::split(lab, labChannels);

    // Apply CLAHE to the L (lightness) channel.
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(
        clipLimit_,
        cv::Size(tileGridSize_, tileGridSize_));
    clahe->apply(labChannels[0], labChannels[0]);

    cv::merge(labChannels, lab);
    cv::cvtColor(lab, bgr, cv::COLOR_Lab2BGR);

    // Convert back to original format.
    if (input.format == PixelFormat::BGRA8) {
        // Restore alpha from original frame.
        cv::Mat alpha;
        cv::extractChannel(src, alpha, 3);

        cv::Mat bgra;
        cv::cvtColor(bgr, bgra, cv::COLOR_BGR2BGRA);

        // Write original alpha back.
        cv::insertChannel(alpha, bgra, 3);
        buffer_ = std::move(bgra);
    } else {
        buffer_ = std::move(bgr);
    }

    return makeOutput(input, buffer_);
}

} // namespace visioncast

#pragma once

/// @file decklink_helpers.h
/// @brief Internal DeckLink SDK helper utilities (VideoMode ↔ BMD enums, BSTR conversion).
///
/// This header is private to the SDK library and must NOT be installed.
/// It is included only by decklink_device.cpp, decklink_input.cpp and
/// decklink_output.cpp when HAS_DECKLINK is defined.

#ifdef HAS_DECKLINK

#include <DeckLinkAPI.h>
#include <windows.h>

#include <string>

#include "visioncast_sdk/video_device.h"

/// Map a VisionCast VideoMode to the closest BMDDisplayMode.
inline BMDDisplayMode resolveBMDMode(const VideoMode& mode) {
    if (mode.width == 3840 && mode.height == 2160) {
        if (mode.frameRate <= 25.01)  return bmdMode4K2160p25;
        if (mode.frameRate <= 29.98)  return bmdMode4K2160p2997;
        if (mode.frameRate <= 50.01)  return bmdMode4K2160p50;
        return bmdMode4K2160p5994;
    }
    // 1080 interlaced
    if (mode.interlaced) {
        if (mode.frameRate <= 25.01)  return bmdModeHD1080i50;
        return bmdModeHD1080i5994;
    }
    // 1080 progressive
    if (mode.frameRate <= 24.01)  return bmdModeHD1080p24;
    if (mode.frameRate <= 25.01)  return bmdModeHD1080p25;
    if (mode.frameRate <= 29.98)  return bmdModeHD1080p2997;
    if (mode.frameRate <= 50.01)  return bmdModeHD1080p50;
    return bmdModeHD1080p5994;
}

/// Map a BMDDisplayMode back to a VisionCast VideoMode (best-effort).
inline VideoMode bmdModeToVideoMode(BMDDisplayMode mode, bool /*interlaced*/ = false) {
    switch (mode) {
        case bmdModeHD1080p24:   return {1920, 1080, 24.0,  PixelFormat::UYVY, false};
        case bmdModeHD1080p25:   return {1920, 1080, 25.0,  PixelFormat::UYVY, false};
        case bmdModeHD1080p2997: return {1920, 1080, 29.97, PixelFormat::UYVY, false};
        case bmdModeHD1080p50:   return {1920, 1080, 50.0,  PixelFormat::UYVY, false};
        case bmdModeHD1080p5994: return {1920, 1080, 59.94, PixelFormat::UYVY, false};
        case bmdModeHD1080i50:   return {1920, 1080, 25.0,  PixelFormat::UYVY, true};
        case bmdModeHD1080i5994: return {1920, 1080, 29.97, PixelFormat::UYVY, true};
        case bmdMode4K2160p25:   return {3840, 2160, 25.0,  PixelFormat::V210, false};
        case bmdMode4K2160p2997: return {3840, 2160, 29.97, PixelFormat::V210, false};
        case bmdMode4K2160p50:   return {3840, 2160, 50.0,  PixelFormat::V210, false};
        case bmdMode4K2160p5994: return {3840, 2160, 59.94, PixelFormat::V210, false};
        default:                 return {};
    }
}

/// Map a VisionCast PixelFormat to BMDPixelFormat.
inline BMDPixelFormat resolveBMDPixelFormat(PixelFormat fmt) {
    switch (fmt) {
        case PixelFormat::UYVY:  return bmdFormat8BitYUV;
        case PixelFormat::V210:  return bmdFormat10BitYUV;
        case PixelFormat::BGRA8: return bmdFormat8BitBGRA;
        case PixelFormat::NV12:  return bmdFormat8BitYUV; // nearest supported
        default:                 return bmdFormat8BitYUV;
    }
}

/// Convert a Windows BSTR to a UTF-8 std::string and free the BSTR.
inline std::string bstrToString(BSTR bstr) {
    if (!bstr) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, bstr, -1,
                                  nullptr, 0, nullptr, nullptr);
    if (len <= 1) return {};
    std::string result(static_cast<size_t>(len - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, bstr, -1,
                        &result[0], len, nullptr, nullptr);
    return result;
}

/// Return the frame duration and time-scale for a given frame rate.
/// These values are suitable for IDeckLinkOutput::StartScheduledPlayback
/// and ScheduleVideoFrame.
inline void getFrameTiming(double frameRate,
                           BMDTimeValue& duration,
                           BMDTimeScale&  scale) {
    if      (frameRate <= 24.01) { duration = 1000; scale = 24000; }
    else if (frameRate <= 25.01) { duration = 1000; scale = 25000; }
    else if (frameRate <= 29.98) { duration = 1001; scale = 30000; }
    else if (frameRate <= 50.01) { duration = 1000; scale = 50000; }
    else                         { duration = 1001; scale = 60000; }
}

#endif // HAS_DECKLINK

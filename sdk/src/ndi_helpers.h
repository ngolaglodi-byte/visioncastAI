#pragma once

/// @file ndi_helpers.h
/// @brief Internal NDI SDK helper utilities (timecode conversions).
///
/// This header is private to the SDK library and must NOT be installed.
/// It is included only by ndi_input.cpp and ndi_output.cpp when HAS_NDI
/// is defined.

#ifdef HAS_NDI

#include <Processing.NDI.Lib.h>

#include <cstdint>
#include <cstdio>
#include <string>

/// Convert an NDI timecode (100-nanosecond intervals from midnight) to a
/// SMPTE "HH:MM:SS:FF" string at the given frame rate.
inline std::string parseNDITimecode(int64_t timecode, double fps = 25.0) {
    if (timecode == NDIlib_send_timecode_synthesize) return {};

    // NDI timecode is in 100 ns units
    const int64_t totalSec    = timecode / 10000000LL;
    const int64_t remain100ns = timecode % 10000000LL;

    const int hh = static_cast<int>(totalSec / 3600);
    const int mm = static_cast<int>((totalSec % 3600) / 60);
    const int ss = static_cast<int>(totalSec % 60);
    const int ff = static_cast<int>(static_cast<double>(remain100ns) * fps / 10000000.0);

    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d:%02d", hh, mm, ss, ff);
    return buf;
}

/// Convert a SMPTE "HH:MM:SS:FF" timecode string to an NDI timecode
/// (100-nanosecond intervals from midnight) at the given frame rate.
inline int64_t parseTimecodeToNDI(const std::string& tc, double fps = 25.0) {
    int hh = 0, mm = 0, ss = 0, ff = 0;
    std::sscanf(tc.c_str(), "%d:%d:%d:%d", &hh, &mm, &ss, &ff);

    const int64_t totalSec        = static_cast<int64_t>(hh) * 3600 + mm * 60 + ss;
    const int64_t frameDuration   = (fps > 0.0)
                                    ? static_cast<int64_t>(10000000.0 / fps)
                                    : 400000LL; // fallback: 25 fps
    return totalSec * 10000000LL + ff * frameDuration;
}

#endif // HAS_NDI

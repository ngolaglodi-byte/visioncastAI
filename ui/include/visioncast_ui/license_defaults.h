#pragma once
/// @file license_defaults.h
/// @brief Compile-time defaults for the licensing API.
///
/// These values are embedded in the binary at build time so that
/// end-users never have to configure environment variables or
/// edit JSON files.  Developers can still override them via
/// LICENSE_API_URL / LICENSE_API_KEY environment variables or
/// CMake definitions (-DVISIONCAST_LICENSE_API_URL=...).
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include <QString>

namespace visioncast_ui {

/// Production Supabase Edge Function URL.
/// Override at build time: cmake -DVISIONCAST_LICENSE_API_URL="https://..."
#ifndef VISIONCAST_LICENSE_API_URL
#define VISIONCAST_LICENSE_API_URL "https://your-project.supabase.co/functions/v1/license-api"
#endif

/// Production Supabase anon/public key.
/// Override at build time: cmake -DVISIONCAST_LICENSE_API_KEY="..."
#ifndef VISIONCAST_LICENSE_API_KEY
#define VISIONCAST_LICENSE_API_KEY "your-supabase-anon-key"
#endif

inline QString defaultApiUrl() {
    return QStringLiteral(VISIONCAST_LICENSE_API_URL);
}

inline QString defaultApiKey() {
    return QStringLiteral(VISIONCAST_LICENSE_API_KEY);
}

} // namespace visioncast_ui

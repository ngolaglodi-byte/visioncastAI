#pragma once

/// @file zmq_endpoint.h
/// @brief Shared ZeroMQ endpoint resolution utility for the VisionCast engine.
///
/// Resolution order (highest to lowest priority):
///   1. Explicit argument (non-empty string).
///   2. @c ZMQ_PUB_ENDPOINT environment variable.
///   3. @c ai.zmq_pub_endpoint key in @c config/system.json (searched in the
///      current working directory and up to two parent directories).
///   4. Built-in default @c tcp://127.0.0.1:5557.

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace visioncast {
namespace detail {

/// Built-in default ZMQ PUB endpoint — single source of truth for C++.
constexpr const char* kZmqDefaultPubEndpoint = "tcp://127.0.0.1:5557";

/// Minimal JSON string extractor: returns value for @c "key": "value".
inline std::string zmqJsonString(const std::string& json, const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return {};
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return {};
    pos = json.find('"', pos + 1);
    if (pos == std::string::npos) return {};
    auto end = json.find('"', pos + 1);
    if (end == std::string::npos) return {};
    return json.substr(pos + 1, end - pos - 1);
}

/// Resolve the ZMQ PUB endpoint using the priority order described above.
///
/// @param explicit_ep  When non-empty, returned immediately without further
///                     resolution steps.
/// @return The resolved endpoint string.
inline std::string resolveZmqPubEndpoint(const std::string& explicit_ep) {
    if (!explicit_ep.empty()) return explicit_ep;

    const char* env = std::getenv("ZMQ_PUB_ENDPOINT");
    if (env && env[0] != '\0') return env;

    // Search for config/system.json relative to cwd and parent directories.
    static const char* kPaths[] = {
        "config/system.json",
        "../config/system.json",
        "../../config/system.json",
    };
    for (const char* p : kPaths) {
        std::ifstream ifs(p);
        if (!ifs.is_open()) continue;
        std::ostringstream ss;
        ss << ifs.rdbuf();
        const std::string text = ss.str();
        // Find the "ai" block.
        const std::string aiNeedle = "\"ai\"";
        auto aiPos = text.find(aiNeedle);
        if (aiPos == std::string::npos) continue;
        auto bracePos = text.find('{', aiPos + aiNeedle.size());
        if (bracePos == std::string::npos) continue;
        int depth = 1;
        size_t cur = bracePos + 1;
        while (cur < text.size() && depth > 0) {
            if (text[cur] == '{') ++depth;
            else if (text[cur] == '}') --depth;
            ++cur;
        }
        const std::string aiBlock = text.substr(bracePos, cur - bracePos);
        const std::string ep = zmqJsonString(aiBlock, "zmq_pub_endpoint");
        if (!ep.empty()) return ep;
    }

    return kZmqDefaultPubEndpoint;
}

} // namespace detail
} // namespace visioncast

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
///
/// Security:
///   All endpoints are validated to use loopback addresses (127.0.0.1 or localhost)
///   to prevent accidental network exposure. Non-loopback endpoints cause
///   the function to fall back to the default loopback endpoint and log a warning.

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace visioncast {
namespace detail {

/// Built-in default ZMQ PUB endpoint — single source of truth for C++.
constexpr const char* kZmqDefaultPubEndpoint = "tcp://127.0.0.1:5557";

/// IPC prefix for local communication
constexpr const char* kIpcPrefix = "ipc://";
constexpr size_t kIpcPrefixLen = 6;

/// Validate that an endpoint uses a loopback address.
///
/// @param endpoint  The ZMQ endpoint string (e.g., "tcp://127.0.0.1:5557").
/// @return True if the endpoint is a loopback address, false otherwise.
inline bool isLoopbackEndpoint(const std::string& endpoint) {
    // Allow IPC endpoints (always local)
    if (endpoint.compare(0, kIpcPrefixLen, kIpcPrefix) == 0) {
        return true;
    }
    
    // Match tcp://host:port pattern
    static const std::regex tcpPattern(R"(^tcp://([^:]+):(\d+)$)");
    std::smatch match;
    if (!std::regex_match(endpoint, match, tcpPattern)) {
        return false;
    }
    
    std::string host = match[1].str();
    // Convert to lowercase for comparison.
    // The double cast (char -> unsigned char -> tolower result -> char) is needed
    // because std::tolower has undefined behavior for negative char values.
    for (auto& c : host) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    
    // Check against allowed loopback addresses
    return host == "127.0.0.1" ||
           host == "localhost" ||
           host == "::1" ||
           host == "[::1]";
}

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
///                     resolution steps (if it passes loopback validation).
/// @return The resolved endpoint string (always a loopback address).
inline std::string resolveZmqPubEndpoint(const std::string& explicit_ep) {
    std::string candidate;
    
    if (!explicit_ep.empty()) {
        candidate = explicit_ep;
    } else {
        const char* env = std::getenv("ZMQ_PUB_ENDPOINT");
        if (env && env[0] != '\0') {
            candidate = env;
        } else {
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
                if (!ep.empty()) {
                    candidate = ep;
                    break;
                }
            }
        }
    }
    
    // Security: Validate loopback-only binding
    if (!candidate.empty()) {
        if (isLoopbackEndpoint(candidate)) {
            return candidate;
        }
        std::cerr << "[ZMQ Security] Non-loopback endpoint rejected: " << candidate
                  << ". Falling back to default loopback endpoint. "
                  << "See SECURITY.md for details." << std::endl;
    }

    return kZmqDefaultPubEndpoint;
}

} // namespace detail
} // namespace visioncast

/// @file sdk_logger.cpp
/// @brief SDKLogger implementation — levelled, thread-safe logging for the SDK layer.

#include "visioncast_sdk/sdk_logger.h"

#include <iostream>
#include <mutex>

// ---------------------------------------------------------------------------
// Internal state
// ---------------------------------------------------------------------------

namespace {

std::mutex  g_mutex;
LogLevel    g_level = LogLevel::INFO;
LogSink     g_sink  = nullptr;

const char* levelStr(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERR:   return "ERROR";
    }
    return "?????";
}

void defaultSink(LogLevel lvl, const std::string& component, const std::string& message) {
    std::cerr << "[" << levelStr(lvl) << "] [" << component << "] " << message << std::endl;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void SDKLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_level = level;
}

LogLevel SDKLogger::level() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_level;
}

void SDKLogger::setSink(LogSink sink) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_sink = std::move(sink);
}

void SDKLogger::log(LogLevel lvl, const std::string& component, const std::string& message) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (static_cast<int>(lvl) < static_cast<int>(g_level)) return;
    if (g_sink) {
        g_sink(lvl, component, message);
    } else {
        defaultSink(lvl, component, message);
    }
}

void SDKLogger::debug(const std::string& component, const std::string& message) {
    log(LogLevel::DEBUG, component, message);
}

void SDKLogger::info(const std::string& component, const std::string& message) {
    log(LogLevel::INFO, component, message);
}

void SDKLogger::warn(const std::string& component, const std::string& message) {
    log(LogLevel::WARN, component, message);
}

void SDKLogger::error(const std::string& component, const std::string& message) {
    log(LogLevel::ERR, component, message);
}

#pragma once

/// @file sdk_logger.h
/// @brief Logging utility for the VisionCast SDK abstraction layer.
///
/// Provides levelled logging (DEBUG, INFO, WARN, ERROR) with configurable
/// output. Thread-safe via a simple mutex guard.

#include <functional>
#include <string>

/// Log severity levels.
enum class LogLevel {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    ERR   = 3  // "ERROR" may conflict with Win32 macros
};

/// Custom log sink signature: (level, component, message).
using LogSink = std::function<void(LogLevel, const std::string&, const std::string&)>;

/// Central SDK logger.
///
/// By default, messages are printed to stderr with a `[component] message` format.
/// A custom sink can be installed to redirect logs (e.g. to a file or UI).
///
/// Usage:
///   SDKLogger::setLevel(LogLevel::DEBUG);
///   SDKLogger::info("DeckLinkInput", "Opened device 0");
///   SDKLogger::error("NDIOutput", "Failed to send frame");
class SDKLogger {
public:
    /// Set the minimum log level. Messages below this level are discarded.
    static void setLevel(LogLevel level);

    /// Return the current minimum log level.
    static LogLevel level();

    /// Install a custom log sink. Pass nullptr to restore the default stderr sink.
    static void setSink(LogSink sink);

    /// Log at DEBUG level.
    static void debug(const std::string& component, const std::string& message);

    /// Log at INFO level.
    static void info(const std::string& component, const std::string& message);

    /// Log at WARN level.
    static void warn(const std::string& component, const std::string& message);

    /// Log at ERROR level.
    static void error(const std::string& component, const std::string& message);

    /// Generic log with explicit level.
    static void log(LogLevel level, const std::string& component, const std::string& message);
};

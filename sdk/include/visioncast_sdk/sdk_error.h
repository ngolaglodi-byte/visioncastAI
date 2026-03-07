#pragma once

/// @file sdk_error.h
/// @brief Error classes for the VisionCast SDK abstraction layer.
///
/// Provides a hierarchy of exceptions for hardware-specific error reporting:
/// - SDKError: base class for all SDK errors
/// - DeckLinkError: Blackmagic DeckLink specific errors
/// - NDIError: NDI specific errors
/// - DeviceNotFoundError: device enumeration failures
/// - SignalLostError: signal detection failures

#include <stdexcept>
#include <string>

/// Base exception for all VisionCast SDK errors.
class SDKError : public std::runtime_error {
public:
    explicit SDKError(const std::string& message)
        : std::runtime_error("[SDKError] " + message) {}

    /// Return the error code (0 = generic).
    virtual int errorCode() const noexcept { return 0; }
};

/// Exception for Blackmagic DeckLink errors.
class DeckLinkError : public SDKError {
public:
    explicit DeckLinkError(const std::string& message, int code = 0)
        : SDKError("[DeckLink] " + message), code_(code) {}

    int errorCode() const noexcept override { return code_; }

private:
    int code_;
};

/// Exception for NDI errors.
class NDIError : public SDKError {
public:
    explicit NDIError(const std::string& message, int code = 0)
        : SDKError("[NDI] " + message), code_(code) {}

    int errorCode() const noexcept override { return code_; }

private:
    int code_;
};

/// Exception raised when a requested device cannot be found.
class DeviceNotFoundError : public SDKError {
public:
    explicit DeviceNotFoundError(const std::string& deviceDescription)
        : SDKError("Device not found: " + deviceDescription) {}
};

/// Exception raised when the input signal is lost.
class SignalLostError : public SDKError {
public:
    explicit SignalLostError(const std::string& deviceName)
        : SDKError("Signal lost on device: " + deviceName) {}
};

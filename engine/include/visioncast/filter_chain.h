#pragma once

/// @file filter_chain.h
/// @brief Sequential video filter pipeline (color correction, LUTs, etc.).

#include <memory>
#include <string>
#include <vector>

#include "visioncast_sdk/video_device.h"

namespace visioncast {

/// Abstract base class for video filters.
class IVideoFilter {
public:
    virtual ~IVideoFilter() = default;

    /// Filter display name.
    virtual std::string name() const = 0;

    /// Process a single frame.
    virtual VideoFrame process(const VideoFrame& input) = 0;

    /// Enable or disable this filter.
    virtual void setEnabled(bool enabled) = 0;

    /// Check if this filter is enabled.
    virtual bool isEnabled() const = 0;
};

/// Applies a chain of video filters sequentially to each frame.
class FilterChain {
public:
    FilterChain();
    ~FilterChain();

    /// Add a filter to the end of the chain.
    void addFilter(std::unique_ptr<IVideoFilter> filter);

    /// Remove a filter by name.
    void removeFilter(const std::string& filterName);

    /// Reorder filters by name list.
    void reorderFilters(const std::vector<std::string>& order);

    /// Apply all enabled filters to a frame.
    VideoFrame apply(const VideoFrame& input);

    /// Enable or disable a specific filter by name.
    void setEnabled(const std::string& filterName, bool enabled);

private:
    std::vector<std::unique_ptr<IVideoFilter>> filters_;
};

} // namespace visioncast

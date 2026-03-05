/// @file filter_chain.cpp
/// @brief FilterChain implementation.

#include "visioncast/filter_chain.h"

#include <algorithm>

namespace visioncast {

FilterChain::FilterChain() = default;
FilterChain::~FilterChain() = default;

void FilterChain::addFilter(std::unique_ptr<IVideoFilter> filter) {
    filters_.push_back(std::move(filter));
}

void FilterChain::removeFilter(const std::string& filterName) {
    filters_.erase(
        std::remove_if(filters_.begin(), filters_.end(),
                       [&](const std::unique_ptr<IVideoFilter>& f) {
                           return f->name() == filterName;
                       }),
        filters_.end());
}

void FilterChain::reorderFilters(const std::vector<std::string>& order) {
    std::vector<std::unique_ptr<IVideoFilter>> reordered;
    reordered.reserve(filters_.size());
    for (const auto& name : order) {
        for (auto& f : filters_) {
            if (f && f->name() == name) {
                reordered.push_back(std::move(f));
                break;
            }
        }
    }
    // Append any filters not in the order list
    for (auto& f : filters_) {
        if (f) {
            reordered.push_back(std::move(f));
        }
    }
    filters_ = std::move(reordered);
}

VideoFrame FilterChain::apply(const VideoFrame& input) {
    VideoFrame frame = input;
    for (auto& filter : filters_) {
        if (filter && filter->isEnabled()) {
            frame = filter->process(frame);
        }
    }
    return frame;
}

void FilterChain::setEnabled(const std::string& filterName, bool enabled) {
    for (auto& filter : filters_) {
        if (filter->name() == filterName) {
            filter->setEnabled(enabled);
            return;
        }
    }
}

} // namespace visioncast

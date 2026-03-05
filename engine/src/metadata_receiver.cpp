/// @file metadata_receiver.cpp
/// @brief MetadataReceiver implementation (ZeroMQ subscriber).

#include "visioncast/metadata_receiver.h"

#include <iostream>

namespace visioncast {

MetadataReceiver::MetadataReceiver(const std::string& endpoint)
    : endpoint_(endpoint) {}

MetadataReceiver::~MetadataReceiver() {
    stop();
}

void MetadataReceiver::start() {
    if (running_.exchange(true)) {
        return;
    }
    listenerThread_ = std::thread(&MetadataReceiver::listenerLoop, this);
}

void MetadataReceiver::stop() {
    running_ = false;
    if (listenerThread_.joinable()) {
        listenerThread_.join();
    }
}

RecognitionMetadata MetadataReceiver::getLatestMetadata() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return latestMetadata_;
}

bool MetadataReceiver::hasNewMetadata() const {
    return hasNew_.load();
}

void MetadataReceiver::listenerLoop() {
    // TODO: Initialize ZeroMQ SUB socket, subscribe to "face.result" topic
    // In a loop: receive messages, parse JSON, update latestMetadata_
    std::cout << "[MetadataReceiver] Listener started on " << endpoint_ << std::endl;

#ifdef HAS_ZMQ
    // ZeroMQ subscriber implementation would go here
    // zmq::context_t context(1);
    // zmq::socket_t socket(context, zmq::socket_type::sub);
    // socket.connect(endpoint_);
    // socket.set(zmq::sockopt::subscribe, "face.result");
    // while (running_) { ... }
#else
    std::cout << "[MetadataReceiver] ZeroMQ not available, listener inactive." << std::endl;
#endif
}

RecognitionMetadata MetadataReceiver::parseJson(const std::string& json) {
    RecognitionMetadata metadata;
    // TODO: Parse JSON using nlohmann/json
    return metadata;
}

} // namespace visioncast

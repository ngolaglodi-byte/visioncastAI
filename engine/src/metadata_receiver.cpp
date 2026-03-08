/// @file metadata_receiver.cpp
/// @brief MetadataReceiver implementation (ZeroMQ subscriber for face.result).
///
/// MetadataReceiver subscribes to the "face.result" topic published by the
/// Python AI module (MetadataSender).  The endpoint defaults to the same
/// value resolved by ZmqReceiver (ZMQ_PUB_ENDPOINT env var, config/system.json
/// ai.zmq_pub_endpoint, then tcp://127.0.0.1:5557).

#include "visioncast/metadata_receiver.h"
#include "visioncast/zmq_endpoint.h"

#include <chrono>
#include <iostream>
#include <thread>

#ifdef HAS_ZMQ
#include <zmq.hpp>
#endif

namespace visioncast {

// ---------------------------------------------------------------------------
// MetadataReceiver
// ---------------------------------------------------------------------------

MetadataReceiver::MetadataReceiver(const std::string& endpoint)
    : endpoint_(detail::resolveZmqPubEndpoint(endpoint)) {}

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
    std::cout << "[MetadataReceiver] Listener started on " << endpoint_
              << " (topic: face.result)" << std::endl;

#ifdef HAS_ZMQ
    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::sub);
        socket.connect(endpoint_);
        socket.set(zmq::sockopt::subscribe, std::string("face.result"));
        socket.set(zmq::sockopt::rcvtimeo, 100);

        while (running_) {
            zmq::message_t topicMsg;
            zmq::message_t payloadMsg;

            auto res = socket.recv(topicMsg, zmq::recv_flags::none);
            if (!res) continue;
            if (!topicMsg.more()) continue;

            res = socket.recv(payloadMsg, zmq::recv_flags::none);
            if (!res) continue;

            std::string payload(static_cast<char*>(payloadMsg.data()),
                                payloadMsg.size());
            RecognitionMetadata md = parseJson(payload);
            {
                std::lock_guard<std::mutex> lock(dataMutex_);
                latestMetadata_ = md;
                hasNew_ = true;
            }
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "[MetadataReceiver] ZeroMQ error: " << e.what() << std::endl;
    }
#else
    std::cout << "[MetadataReceiver] ZeroMQ not available, listener inactive."
              << std::endl;
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
#endif

    running_ = false;
    std::cout << "[MetadataReceiver] Listener stopped." << std::endl;
}

RecognitionMetadata MetadataReceiver::parseJson(const std::string& json) {
    RecognitionMetadata metadata;
    // Forward to ZmqReceiver's parser which handles both recognition_result
    // and talent_overlay message types.
    metadata = ZmqReceiver::parseJson(json);
    return metadata;
}

} // namespace visioncast

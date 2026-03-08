/// @file metadata_receiver.cpp
/// @brief MetadataReceiver implementation (ZeroMQ subscriber for face.result).
///
/// MetadataReceiver subscribes to the "face.result" topic published by the
/// Python AI module (MetadataSender).  The endpoint defaults to the same
/// value resolved by ZmqReceiver (ZMQ_PUB_ENDPOINT env var, config/system.json
/// ai.zmq_pub_endpoint, then tcp://127.0.0.1:5557).

#include "visioncast/metadata_receiver.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef HAS_ZMQ
#include <zmq.hpp>
#endif

namespace visioncast {

// ---------------------------------------------------------------------------
// Endpoint resolution (mirrors zmq_receiver.cpp)
// ---------------------------------------------------------------------------

namespace {

constexpr const char* kDefaultEndpoint = "tcp://127.0.0.1:5557";

std::string simpleJsonStr(const std::string& json, const std::string& key) {
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

std::string resolveEndpoint(const std::string& explicit_ep) {
    if (!explicit_ep.empty()) return explicit_ep;
    const char* env = std::getenv("ZMQ_PUB_ENDPOINT");
    if (env && env[0] != '\0') return env;
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
        const std::string ep = simpleJsonStr(aiBlock, "zmq_pub_endpoint");
        if (!ep.empty()) return ep;
    }
    return kDefaultEndpoint;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// MetadataReceiver
// ---------------------------------------------------------------------------

MetadataReceiver::MetadataReceiver(const std::string& endpoint)
    : endpoint_(resolveEndpoint(endpoint)) {}

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

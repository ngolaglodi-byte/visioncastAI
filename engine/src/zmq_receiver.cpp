/// @file zmq_receiver.cpp
/// @brief ZmqReceiver implementation – ZeroMQ subscriber with thread-safe
///        MetadataStore that receives Python AI metadata.

#include "visioncast/zmq_receiver.h"

#include <chrono>
#include <iostream>
#include <sstream>

#ifdef HAS_ZMQ
#include <zmq.hpp>
#endif

namespace visioncast {

// ---------------------------------------------------------------------------
// MetadataStore
// ---------------------------------------------------------------------------

void MetadataStore::update(const RecognitionMetadata& metadata) {
    std::lock_guard<std::mutex> lock(mutex_);
    latest_ = metadata;
    hasNew_ = true;
}

RecognitionMetadata MetadataStore::get() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_;
}

bool MetadataStore::hasNew() const {
    return hasNew_.load();
}

bool MetadataStore::consumeNew() {
    return hasNew_.exchange(false);
}

// ---------------------------------------------------------------------------
// Minimal JSON helpers (no external dependency required)
// ---------------------------------------------------------------------------

namespace {

/// Return the string value for a given key: "key": "value"
std::string jsonString(const std::string& json, const std::string& key) {
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

/// Return a numeric value for a given key: "key": 123.4
double jsonNumber(const std::string& json, const std::string& key,
                  double defaultVal = 0.0) {
    const std::string needle = "\"" + key + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return defaultVal;
    ++pos;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        ++pos;
    try {
        return std::stod(json.substr(pos));
    } catch (...) {
        return defaultVal;
    }
}

/// Extract a balanced JSON sub-object or sub-array for the given key.
std::string jsonBlock(const std::string& json, const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return {};
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return {};
    ++pos;
    while (pos < json.size() &&
           (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n'))
        ++pos;
    if (pos >= json.size()) return {};

    char open = json[pos];
    char close;
    if (open == '{')
        close = '}';
    else if (open == '[')
        close = ']';
    else
        return {};

    int depth = 1;
    size_t start = pos;
    ++pos;
    bool inStr = false;
    while (pos < json.size() && depth > 0) {
        char c = json[pos];
        if (c == '"' && (pos == 0 || json[pos - 1] != '\\'))
            inStr = !inStr;
        if (!inStr) {
            if (c == open) ++depth;
            else if (c == close) --depth;
        }
        ++pos;
    }
    return json.substr(start, pos - start);
}

/// Split a JSON array "[{…},{…}]" into its top-level element strings.
std::vector<std::string> jsonArrayElements(const std::string& arr) {
    std::vector<std::string> elements;
    if (arr.size() < 2 || arr.front() != '[') return elements;

    size_t pos = 1;
    while (pos < arr.size()) {
        while (pos < arr.size() &&
               (arr[pos] == ' ' || arr[pos] == ',' || arr[pos] == '\n'))
            ++pos;
        if (pos >= arr.size() || arr[pos] == ']') break;

        char open = arr[pos];
        char close;
        if (open == '{')
            close = '}';
        else if (open == '[')
            close = ']';
        else
            break;

        int depth = 1;
        size_t start = pos;
        ++pos;
        bool inStr = false;
        while (pos < arr.size() && depth > 0) {
            char c = arr[pos];
            if (c == '"' && (pos == 0 || arr[pos - 1] != '\\'))
                inStr = !inStr;
            if (!inStr) {
                if (c == open) ++depth;
                else if (c == close) --depth;
            }
            ++pos;
        }
        elements.push_back(arr.substr(start, pos - start));
    }
    return elements;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// ZmqReceiver
// ---------------------------------------------------------------------------

ZmqReceiver::ZmqReceiver(const std::string& endpoint, const std::string& topic)
    : endpoint_(endpoint), topic_(topic) {}

ZmqReceiver::~ZmqReceiver() { stop(); }

void ZmqReceiver::start() {
    if (running_.exchange(true)) return; // already running
    listenerThread_ = std::thread(&ZmqReceiver::listenerLoop, this);
}

void ZmqReceiver::stop() {
    running_ = false;
    if (listenerThread_.joinable()) listenerThread_.join();
}

bool ZmqReceiver::isRunning() const { return running_.load(); }

RecognitionMetadata ZmqReceiver::getLatestMetadata() const {
    return store_.get();
}

bool ZmqReceiver::hasNewMetadata() const { return store_.hasNew(); }

bool ZmqReceiver::consumeNew() { return store_.consumeNew(); }

// ---------------------------------------------------------------------------
// Listener thread
// ---------------------------------------------------------------------------

void ZmqReceiver::listenerLoop() {
    std::cout << "[ZmqReceiver] Listener started on " << endpoint_
              << " (topic: " << topic_ << ")" << std::endl;

#ifdef HAS_ZMQ
    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::sub);
        socket.connect(endpoint_);
        socket.set(zmq::sockopt::subscribe, topic_);

        // Use a receive timeout so we can periodically check the running_ flag.
        socket.set(zmq::sockopt::rcvtimeo, 100);

        while (running_) {
            zmq::message_t topicMsg;
            zmq::message_t payloadMsg;

            // Receive the topic frame.
            auto res = socket.recv(topicMsg, zmq::recv_flags::none);
            if (!res) continue; // timeout – re-check running_

            // Receive the payload frame.
            res = socket.recv(payloadMsg, zmq::recv_flags::none);
            if (!res) continue;

            std::string payload(static_cast<char*>(payloadMsg.data()),
                                payloadMsg.size());

            RecognitionMetadata metadata = parseJson(payload);
            store_.update(metadata);
        }

        socket.close();
        context.close();
    } catch (const zmq::error_t& e) {
        std::cerr << "[ZmqReceiver] ZeroMQ error: " << e.what() << std::endl;
    }
#else
    std::cout << "[ZmqReceiver] ZeroMQ not available, listener inactive."
              << std::endl;
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
#endif

    running_ = false;
    std::cout << "[ZmqReceiver] Listener stopped." << std::endl;
}

// ---------------------------------------------------------------------------
// JSON parsing
// ---------------------------------------------------------------------------

RecognitionMetadata ZmqReceiver::parseJson(const std::string& json) {
    RecognitionMetadata metadata;
    const std::string msgType = jsonString(json, "type");

    if (msgType == "recognition_result") {
        // ---- RecognitionResult (see python/ipc/protocol.py) ----
        metadata.timestampMs =
            static_cast<int64_t>(jsonNumber(json, "timestamp_ms"));
        metadata.frameId =
            static_cast<int>(jsonNumber(json, "frame_id"));

        std::string facesArr = jsonBlock(json, "faces");
        for (const auto& faceJson : jsonArrayElements(facesArr)) {
            RecognitionMetadata::Face face{};

            std::string locJson = jsonBlock(faceJson, "location");
            if (!locJson.empty()) {
                face.top    = static_cast<int>(jsonNumber(locJson, "top"));
                face.right  = static_cast<int>(jsonNumber(locJson, "right"));
                face.bottom = static_cast<int>(jsonNumber(locJson, "bottom"));
                face.left   = static_cast<int>(jsonNumber(locJson, "left"));
            }

            face.confidence =
                static_cast<float>(jsonNumber(faceJson, "confidence"));

            std::string talentJson = jsonBlock(faceJson, "talent");
            if (!talentJson.empty() && talentJson != "null") {
                face.talentId        = jsonString(talentJson, "id");
                face.talentName      = jsonString(talentJson, "name");
                face.talentRole      = jsonString(talentJson, "role");
                face.overlayTemplate = jsonString(talentJson, "overlay_template");
            }

            metadata.faces.push_back(face);
        }
    } else if (msgType == "talent_overlay") {
        // ---- TalentOverlayMessage (see python/ipc/protocol.py) ----
        // Convert to a single-face RecognitionMetadata so the rest of the
        // engine pipeline can consume it uniformly.
        RecognitionMetadata::Face face{};
        face.talentId        = jsonString(json, "talent_id");
        face.talentName      = jsonString(json, "name");
        face.talentRole      = jsonString(json, "role");
        face.overlayTemplate = jsonString(json, "overlay");
        face.confidence      =
            static_cast<float>(jsonNumber(json, "confidence"));
        metadata.faces.push_back(face);
    }

    return metadata;
}

} // namespace visioncast

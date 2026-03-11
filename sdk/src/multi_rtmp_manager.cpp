/// @file multi_rtmp_manager.cpp
/// @brief Multi-platform RTMP streaming manager implementation.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast_sdk/multi_rtmp_manager.h"
#include "visioncast_sdk/sdk_logger.h"

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

static const char* TAG = "MultiRtmpManager";

// ── Status helper ──────────────────────────────────────────────────────────

const char* rtmpStreamStatusToString(RtmpStreamStatus s)
{
    switch (s) {
        case RtmpStreamStatus::Idle:       return "idle";
        case RtmpStreamStatus::Connecting: return "connecting";
        case RtmpStreamStatus::Live:       return "live";
        case RtmpStreamStatus::Error:      return "error";
    }
    return "unknown";
}

// ── Internal stream record ─────────────────────────────────────────────────

struct StreamRecord {
    RtmpStreamEntry  entry;
    RTMPOutput       output;
    std::thread      worker;
    std::atomic<bool> stopRequested{false};
    mutable std::mutex mutex;   ///< Protects entry fields.

    StreamRecord() = default;

    // Non-copyable because of mutex and thread
    StreamRecord(const StreamRecord&)            = delete;
    StreamRecord& operator=(const StreamRecord&) = delete;
};
// ── ID generation ──────────────────────────────────────────────────────────

static std::string generateId()
{
    // Simple monotonic counter — sufficient for a runtime-only ID.
    static std::atomic<uint64_t> counter{1};
    std::ostringstream oss;
    oss << "rtmp-" << counter.fetch_add(1);
    return oss.str();
}

// ── Impl ───────────────────────────────────────────────────────────────────

struct MultiRtmpManager::Impl {
    mutable std::mutex listMutex;   ///< Protects streams vector.
    std::vector<std::shared_ptr<StreamRecord>> streams;
    MultiRtmpManager::StatusCallback statusCb;

    // Find a record by ID (caller must hold listMutex).
    std::shared_ptr<StreamRecord> findLocked(const std::string& id) const
    {
        for (const auto& rec : streams) {
            if (rec->entry.id == id) return rec;
        }
        return nullptr;
    }

    void pushLog(StreamRecord& rec, const std::string& line)
    {
        std::lock_guard<std::mutex> lk(rec.mutex);
        rec.entry.logLines.push_back(line);
        if (static_cast<int>(rec.entry.logLines.size()) > RtmpStreamEntry::kMaxLogLines)
            rec.entry.logLines.erase(rec.entry.logLines.begin());
    }

    void setStatus(StreamRecord& rec, RtmpStreamStatus st, const std::string& msg)
    {
        {
            std::lock_guard<std::mutex> lk(rec.mutex);
            rec.entry.status        = st;
            rec.entry.statusMessage = msg;
        }
        if (statusCb) statusCb(rec.entry.id, st, msg);
    }
};

// ── Construction / Destruction ─────────────────────────────────────────────

MultiRtmpManager::MultiRtmpManager()
    : impl_(std::make_unique<Impl>())
{}

MultiRtmpManager::~MultiRtmpManager()
{
    stopAll();
}

// ── Callback ───────────────────────────────────────────────────────────────

void MultiRtmpManager::setStatusCallback(StatusCallback cb)
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    impl_->statusCb = std::move(cb);
}

// ── addStream ──────────────────────────────────────────────────────────────

std::string MultiRtmpManager::addStream(const std::string& name,
                                        const std::string& platform,
                                        const std::string& serverUrl,
                                        const std::string& streamKey)
{
    auto rec = std::make_shared<StreamRecord>();
    rec->entry.id        = generateId();
    rec->entry.name      = name;
    rec->entry.platform  = platform;
    rec->entry.serverUrl = serverUrl;
    rec->entry.streamKey = streamKey;
    rec->entry.status    = RtmpStreamStatus::Idle;

    const std::string id = rec->entry.id;
    SDKLogger::info(TAG, "Added stream: " + name + " [" + id + "] → " + serverUrl);

    std::lock_guard<std::mutex> lk(impl_->listMutex);
    impl_->streams.push_back(std::move(rec));
    return id;
}

// ── removeStream ───────────────────────────────────────────────────────────

bool MultiRtmpManager::removeStream(const std::string& id)
{
    // Stop first (outside list lock to avoid deadlock with worker thread).
    stopStream(id);

    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto it = std::find_if(impl_->streams.begin(), impl_->streams.end(),
                           [&id](const std::unique_ptr<StreamRecord>& r) {
                               return r->entry.id == id;
                           });
    if (it == impl_->streams.end()) return false;
    impl_->streams.erase(it);
    SDKLogger::info(TAG, "Removed stream: " + id);
    return true;
}

// ── updateStream ───────────────────────────────────────────────────────────

bool MultiRtmpManager::updateStream(const std::string& id,
                                    const std::string& name,
                                    const std::string& serverUrl,
                                    const std::string& streamKey)
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto rec = impl_->findLocked(id);
    if (!rec) return false;

    std::lock_guard<std::mutex> rlk(rec->mutex);
    if (rec->entry.status == RtmpStreamStatus::Live ||
        rec->entry.status == RtmpStreamStatus::Connecting)
    {
        SDKLogger::warn(TAG, "Cannot update active stream: " + id);
        return false;
    }
    rec->entry.name      = name;
    rec->entry.serverUrl = serverUrl;
    rec->entry.streamKey = streamKey;
    SDKLogger::info(TAG, "Updated stream: " + name + " [" + id + "]");
    return true;
}

// ── startStream ────────────────────────────────────────────────────────────

bool MultiRtmpManager::startStream(const std::string& id)
{
    std::shared_ptr<StreamRecord> rec;
    {
        std::lock_guard<std::mutex> lk(impl_->listMutex);
        rec = impl_->findLocked(id);
    }
    if (!rec) {
        SDKLogger::warn(TAG, "startStream: unknown id " + id);
        return false;
    }

    {
        std::lock_guard<std::mutex> rlk(rec->mutex);
        if (rec->entry.status == RtmpStreamStatus::Live ||
            rec->entry.status == RtmpStreamStatus::Connecting)
        {
            SDKLogger::warn(TAG, "Stream already active: " + id);
            return false;
        }
    }

    // Reset stop flag and mark Connecting.
    rec->stopRequested.store(false);
    impl_->setStatus(*rec, RtmpStreamStatus::Connecting, "Connecting…");
    impl_->pushLog(*rec, "[INFO] Starting stream: " + rec->entry.name);

    const std::string url   = rec->entry.serverUrl;
    const std::string key   = rec->entry.streamKey;
    const std::string recId = id;

    // Capture shared_ptr so the StreamRecord outlives the worker thread,
    // even if removeStream() is called concurrently.
    rec->worker = std::thread([this, rec, url, key, recId]() {
        SDKLogger::info(TAG, "Worker thread started for: " + recId);
        impl_->pushLog(*rec, "[INFO] Connecting to " + url);

        rec->output.setServerUrl(url);
        rec->output.setStreamKey(key);

        DeviceConfig cfg;
        cfg.name = rec->entry.name;

        const bool opened = rec->output.open(cfg);
        if (!opened) {
            const std::string errMsg = "Failed to open RTMP output for " + rec->entry.name;
            SDKLogger::error(TAG, errMsg);
            impl_->pushLog(*rec, "[ERROR] " + errMsg);
            impl_->setStatus(*rec, RtmpStreamStatus::Error, errMsg);
            return;
        }

        const VideoMode mode{1920, 1080, 25.0, PixelFormat::NV12, false};
        const bool started = rec->output.startPlayout(mode);
        if (!started) {
            const std::string errMsg = "startPlayout failed for " + rec->entry.name;
            SDKLogger::error(TAG, errMsg);
            impl_->pushLog(*rec, "[ERROR] " + errMsg);
            rec->output.close();
            impl_->setStatus(*rec, RtmpStreamStatus::Error, errMsg);
            return;
        }

        impl_->pushLog(*rec, "[OK] Stream live: " + rec->entry.name);
        impl_->setStatus(*rec, RtmpStreamStatus::Live, "Streaming live");
        SDKLogger::info(TAG, "Stream live: " + recId);

        // Keep the stream alive until stopRequested.
        while (!rec->stopRequested.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Graceful shutdown.
        rec->output.stopPlayout();
        rec->output.close();
        impl_->pushLog(*rec, "[INFO] Stream stopped: " + rec->entry.name);
        impl_->setStatus(*rec, RtmpStreamStatus::Idle, "Stopped");
        SDKLogger::info(TAG, "Stream stopped: " + recId);
    });

    return true;
}

// ── stopStream ─────────────────────────────────────────────────────────────

bool MultiRtmpManager::stopStream(const std::string& id)
{
    std::shared_ptr<StreamRecord> rec;
    {
        std::lock_guard<std::mutex> lk(impl_->listMutex);
        rec = impl_->findLocked(id);
        if (!rec) return false;
    }

    // Signal stop and join worker (without holding listMutex to avoid deadlock).
    rec->stopRequested.store(true);
    if (rec->worker.joinable()) {
        rec->worker.join();
    }
    return true;
}

// ── stopAll ────────────────────────────────────────────────────────────────

void MultiRtmpManager::stopAll()
{
    // Collect shared_ptrs while holding the lock, then release the lock before joining.
    std::vector<std::shared_ptr<StreamRecord>> toStop;
    {
        std::lock_guard<std::mutex> lk(impl_->listMutex);
        for (auto& rec : impl_->streams) {
            rec->stopRequested.store(true);
            toStop.push_back(rec);
        }
    }
    for (auto& rec : toStop) {
        if (rec->worker.joinable()) rec->worker.join();
    }
    SDKLogger::info(TAG, "All streams stopped");
}

// ── Queries ────────────────────────────────────────────────────────────────

std::vector<RtmpStreamEntry> MultiRtmpManager::streams() const
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    std::vector<RtmpStreamEntry> result;
    result.reserve(impl_->streams.size());
    for (const auto& rec : impl_->streams) {
        std::lock_guard<std::mutex> rlk(rec->mutex);
        result.push_back(rec->entry);
    }
    return result;
}

bool MultiRtmpManager::getStream(const std::string& id, RtmpStreamEntry& out) const
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto rec = impl_->findLocked(id);
    if (!rec) return false;
    std::lock_guard<std::mutex> rlk(rec->mutex);
    out = rec->entry;
    return true;
}

std::size_t MultiRtmpManager::streamCount() const
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    return impl_->streams.size();
}

/// @file multi_ffmpeg_rtmp_manager.cpp
/// @brief Multi-platform RTMP streaming manager implementation using FFmpeg.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast/multi_ffmpeg_rtmp_manager.h"

#include <algorithm>
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace visioncast {

static const char* TAG = "MultiFFmpegRtmpManager";

// ── Log helper ─────────────────────────────────────────────────────────────

namespace {
    void logInfo(const char* tag, const std::string& msg) {
        std::cerr << "[" << tag << "] INFO: " << msg << std::endl;
    }
    void logWarn(const char* tag, const std::string& msg) {
        std::cerr << "[" << tag << "] WARN: " << msg << std::endl;
    }
    void logError(const char* tag, const std::string& msg) {
        std::cerr << "[" << tag << "] ERROR: " << msg << std::endl;
    }
}

// ── Internal stream record ─────────────────────────────────────────────────

struct StreamRecord {
    FFmpegRtmpStreamEntry entry;
    FFmpegRtmpOutput      output;
    std::thread           worker;
    std::atomic<bool>     stopRequested{false};
    mutable std::mutex    mutex;   ///< Protects entry fields.

    StreamRecord() = default;

    // Non-copyable because of mutex and thread
    StreamRecord(const StreamRecord&)            = delete;
    StreamRecord& operator=(const StreamRecord&) = delete;
};

// ── ID generation ──────────────────────────────────────────────────────────

static std::string generateId()
{
    static std::atomic<uint64_t> counter{1};
    std::ostringstream oss;
    oss << "ffmpeg-rtmp-" << counter.fetch_add(1);
    return oss.str();
}

// ── Impl ───────────────────────────────────────────────────────────────────

struct MultiFFmpegRtmpManager::Impl {
    mutable std::mutex listMutex;   ///< Protects streams vector.
    std::vector<std::shared_ptr<StreamRecord>> streams;
    MultiFFmpegRtmpManager::StatusCallback statusCb;

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
        if (static_cast<int>(rec.entry.logLines.size()) > FFmpegRtmpStreamEntry::kMaxLogLines)
            rec.entry.logLines.erase(rec.entry.logLines.begin());
    }

    void setStatus(StreamRecord& rec, RtmpStatus st, const std::string& msg)
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

MultiFFmpegRtmpManager::MultiFFmpegRtmpManager()
    : impl_(std::make_unique<Impl>())
{}

MultiFFmpegRtmpManager::~MultiFFmpegRtmpManager()
{
    stopAll();
}

// ── Callback ───────────────────────────────────────────────────────────────

void MultiFFmpegRtmpManager::setStatusCallback(StatusCallback cb)
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    impl_->statusCb = std::move(cb);
}

// ── addStream ──────────────────────────────────────────────────────────────

std::string MultiFFmpegRtmpManager::addStream(const std::string& name,
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
    rec->entry.status    = RtmpStatus::Idle;

    const std::string id = rec->entry.id;
    logInfo(TAG, "Added stream: " + name + " [" + id + "] → " + serverUrl);

    std::lock_guard<std::mutex> lk(impl_->listMutex);
    impl_->streams.push_back(std::move(rec));
    return id;
}

// ── removeStream ───────────────────────────────────────────────────────────

bool MultiFFmpegRtmpManager::removeStream(const std::string& id)
{
    // Stop first (outside list lock to avoid deadlock with worker thread).
    stopStream(id);

    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto it = std::find_if(impl_->streams.begin(), impl_->streams.end(),
                           [&id](const std::shared_ptr<StreamRecord>& r) {
                               return r->entry.id == id;
                           });
    if (it == impl_->streams.end()) return false;
    impl_->streams.erase(it);
    logInfo(TAG, "Removed stream: " + id);
    return true;
}

// ── updateStream ───────────────────────────────────────────────────────────

bool MultiFFmpegRtmpManager::updateStream(const std::string& id,
                                          const std::string& name,
                                          const std::string& serverUrl,
                                          const std::string& streamKey)
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto rec = impl_->findLocked(id);
    if (!rec) return false;

    std::lock_guard<std::mutex> rlk(rec->mutex);
    if (rec->entry.status == RtmpStatus::Live ||
        rec->entry.status == RtmpStatus::Connecting)
    {
        logWarn(TAG, "Cannot update active stream: " + id);
        return false;
    }
    rec->entry.name      = name;
    rec->entry.serverUrl = serverUrl;
    rec->entry.streamKey = streamKey;
    logInfo(TAG, "Updated stream: " + name + " [" + id + "]");
    return true;
}

// ── startStream ────────────────────────────────────────────────────────────

bool MultiFFmpegRtmpManager::startStream(const std::string& id)
{
    std::shared_ptr<StreamRecord> rec;
    {
        std::lock_guard<std::mutex> lk(impl_->listMutex);
        rec = impl_->findLocked(id);
    }
    if (!rec) {
        logWarn(TAG, "startStream: unknown id " + id);
        return false;
    }

    {
        std::lock_guard<std::mutex> rlk(rec->mutex);
        if (rec->entry.status == RtmpStatus::Live ||
            rec->entry.status == RtmpStatus::Connecting)
        {
            logWarn(TAG, "Stream already active: " + id);
            return false;
        }
    }

    // Reset stop flag and mark Connecting.
    rec->stopRequested.store(false);
    impl_->setStatus(*rec, RtmpStatus::Connecting, "Connecting…");
    impl_->pushLog(*rec, "[INFO] Starting stream: " + rec->entry.name);

    const std::string url   = rec->entry.serverUrl;
    const std::string key   = rec->entry.streamKey;
    const std::string recId = id;

    // Capture shared_ptr so the StreamRecord outlives the worker thread.
    rec->worker = std::thread([this, rec, url, key, recId]() {
        logInfo(TAG, "Worker thread started for: " + recId);
        impl_->pushLog(*rec, "[INFO] Connecting to " + url);

        // Configure and open the FFmpeg RTMP output
        RtmpStreamConfig cfg;
        cfg.serverUrl = url;
        cfg.streamKey = key;
        cfg.width     = 1920;
        cfg.height    = 1080;
        cfg.frameRate = 25.0;

        rec->output.setConfig(cfg);

        const bool opened = rec->output.open();
        if (!opened) {
            const std::string errMsg = "Failed to open RTMP output for " + rec->entry.name;
            logError(TAG, errMsg);
            impl_->pushLog(*rec, "[ERROR] " + errMsg);
            impl_->setStatus(*rec, RtmpStatus::Error, errMsg);
            return;
        }

        const bool started = rec->output.start();
        if (!started) {
            const std::string errMsg = "Failed to start stream for " + rec->entry.name;
            logError(TAG, errMsg);
            impl_->pushLog(*rec, "[ERROR] " + errMsg);
            rec->output.close();
            impl_->setStatus(*rec, RtmpStatus::Error, errMsg);
            return;
        }

        impl_->pushLog(*rec, "[OK] Stream live: " + rec->entry.name);
        impl_->setStatus(*rec, RtmpStatus::Live, "Streaming live");
        logInfo(TAG, "Stream live: " + recId);

        // Keep the stream alive until stopRequested.
        while (!rec->stopRequested.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Graceful shutdown.
        rec->output.stop();
        rec->output.close();
        impl_->pushLog(*rec, "[INFO] Stream stopped: " + rec->entry.name);
        impl_->setStatus(*rec, RtmpStatus::Idle, "Stopped");
        logInfo(TAG, "Stream stopped: " + recId);
    });

    return true;
}

// ── stopStream ─────────────────────────────────────────────────────────────

bool MultiFFmpegRtmpManager::stopStream(const std::string& id)
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

void MultiFFmpegRtmpManager::stopAll()
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
    logInfo(TAG, "All streams stopped");
}

// ── Frame sending ──────────────────────────────────────────────────────────

void MultiFFmpegRtmpManager::sendFrame(const RtmpFrame& frame)
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    for (auto& rec : impl_->streams) {
        if (rec->output.isStreaming()) {
            rec->output.sendFrame(frame);
        }
    }
}

bool MultiFFmpegRtmpManager::sendFrame(const std::string& id, const RtmpFrame& frame)
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto rec = impl_->findLocked(id);
    if (!rec || !rec->output.isStreaming()) {
        return false;
    }
    return rec->output.sendFrame(frame);
}

// ── Queries ────────────────────────────────────────────────────────────────

std::vector<FFmpegRtmpStreamEntry> MultiFFmpegRtmpManager::streams() const
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    std::vector<FFmpegRtmpStreamEntry> result;
    result.reserve(impl_->streams.size());
    for (const auto& rec : impl_->streams) {
        std::lock_guard<std::mutex> rlk(rec->mutex);
        result.push_back(rec->entry);
    }
    return result;
}

bool MultiFFmpegRtmpManager::getStream(const std::string& id, FFmpegRtmpStreamEntry& out) const
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    auto rec = impl_->findLocked(id);
    if (!rec) return false;
    std::lock_guard<std::mutex> rlk(rec->mutex);
    out = rec->entry;
    return true;
}

std::size_t MultiFFmpegRtmpManager::streamCount() const
{
    std::lock_guard<std::mutex> lk(impl_->listMutex);
    return impl_->streams.size();
}

} // namespace visioncast

/// @file ffmpeg_rtmp.cpp
/// @brief FFmpeg-based RTMP streaming implementation.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast/ffmpeg_rtmp.h"

#ifdef HAS_FFMPEG
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#endif

#include <cstring>
#include <iostream>
#include <mutex>

namespace visioncast {

// ── Status helper ──────────────────────────────────────────────────────────

const char* rtmpStatusToString(RtmpStatus status) {
    switch (status) {
        case RtmpStatus::Idle:       return "Idle";
        case RtmpStatus::Connecting: return "Connecting";
        case RtmpStatus::Live:       return "Live";
        case RtmpStatus::Error:      return "Error";
        default:                     return "Unknown";
    }
}

// ── Log helper (uses std::cerr for engine, not SDK logger) ─────────────────

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

static const char* TAG = "FFmpegRtmpOutput";

// ── Implementation ─────────────────────────────────────────────────────────

struct FFmpegRtmpOutput::Impl {
    RtmpStreamConfig config;
    RtmpStatus status = RtmpStatus::Idle;
    RtmpStatusCallback statusCallback;
    std::string lastError;
    std::mutex mutex;

#ifdef HAS_FFMPEG
    AVFormatContext* fmtCtx   = nullptr;
    AVCodecContext*  codecCtx = nullptr;
    AVStream*        stream   = nullptr;
    AVPacket*        pkt      = nullptr;
    int64_t          frameIdx = 0;
#endif

    bool isOpen = false;
    bool isStreaming = false;

    void setStatus(RtmpStatus newStatus, const std::string& message = "") {
        status = newStatus;
        if (newStatus == RtmpStatus::Error) {
            lastError = message;
        }
        if (statusCallback) {
            statusCallback(newStatus, message);
        }
    }
};

FFmpegRtmpOutput::FFmpegRtmpOutput()
    : impl_(std::make_unique<Impl>()) {}

FFmpegRtmpOutput::~FFmpegRtmpOutput() {
    close();
}

FFmpegRtmpOutput::FFmpegRtmpOutput(FFmpegRtmpOutput&&) noexcept = default;
FFmpegRtmpOutput& FFmpegRtmpOutput::operator=(FFmpegRtmpOutput&&) noexcept = default;

void FFmpegRtmpOutput::setConfig(const RtmpStreamConfig& config) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->config = config;
}

RtmpStreamConfig FFmpegRtmpOutput::config() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->config;
}

std::string FFmpegRtmpOutput::fullUrl() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->config.serverUrl + "/" + impl_->config.streamKey;
}

bool FFmpegRtmpOutput::open() {
#ifdef HAS_FFMPEG
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (impl_->isOpen) {
        logWarn(TAG, "Already open");
        return true;
    }

    impl_->setStatus(RtmpStatus::Connecting, "Opening RTMP connection");
    const std::string url = impl_->config.serverUrl + "/" + impl_->config.streamKey;
    logInfo(TAG, "Opening RTMP output to " + url);

    int ret = avformat_alloc_output_context2(
                  &impl_->fmtCtx, nullptr, "flv", url.c_str());
    if (ret < 0 || !impl_->fmtCtx) {
        char errbuf[128];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::string errMsg = std::string("avformat_alloc_output_context2 failed: ") + errbuf;
        logError(TAG, errMsg);
        impl_->setStatus(RtmpStatus::Error, errMsg);
        return false;
    }

    impl_->isOpen = true;
    logInfo(TAG, "RTMP context created for " + url);
    return true;
#else
    logWarn(TAG, "FFmpeg not available — RTMP output will not open");
    impl_->setStatus(RtmpStatus::Error, "FFmpeg not available");
    return false;
#endif
}

bool FFmpegRtmpOutput::open(const RtmpStreamConfig& config) {
    setConfig(config);
    return open();
}

void FFmpegRtmpOutput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->isOpen) return;
    
    logInfo(TAG, "Closing RTMP output");
    impl_->isStreaming = false;

#ifdef HAS_FFMPEG
    if (impl_->fmtCtx) {
        if (impl_->fmtCtx->pb) {
            av_write_trailer(impl_->fmtCtx);
            avio_closep(&impl_->fmtCtx->pb);
        }
        avformat_free_context(impl_->fmtCtx);
        impl_->fmtCtx = nullptr;
    }
    if (impl_->codecCtx) {
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
    }
    if (impl_->pkt) {
        av_packet_free(&impl_->pkt);
        impl_->pkt = nullptr;
    }
    impl_->stream   = nullptr;
    impl_->frameIdx = 0;
#endif

    impl_->isOpen = false;
    impl_->setStatus(RtmpStatus::Idle, "Connection closed");
    logInfo(TAG, "RTMP output closed");
}

bool FFmpegRtmpOutput::isOpen() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->isOpen;
}

bool FFmpegRtmpOutput::start() {
#ifdef HAS_FFMPEG
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->isOpen || !impl_->fmtCtx) {
        logError(TAG, "start() called on closed connection");
        impl_->setStatus(RtmpStatus::Error, "Connection not open");
        return false;
    }
    
    if (impl_->isStreaming) {
        logWarn(TAG, "Already streaming");
        return true;
    }

    const auto& cfg = impl_->config;
    logInfo(TAG, "Starting RTMP stream " +
                 std::to_string(cfg.width) + "x" +
                 std::to_string(cfg.height) + "@" +
                 std::to_string(cfg.frameRate));

    // Create H.264 encoder
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        logError(TAG, "H.264 encoder not found");
        impl_->setStatus(RtmpStatus::Error, "H.264 encoder not found");
        return false;
    }

    impl_->codecCtx = avcodec_alloc_context3(codec);
    impl_->codecCtx->bit_rate = cfg.bitrateBps;
    impl_->codecCtx->width    = cfg.width;
    impl_->codecCtx->height   = cfg.height;
    impl_->codecCtx->time_base = {1000, static_cast<int>(cfg.frameRate * 1000)};
    impl_->codecCtx->framerate = {static_cast<int>(cfg.frameRate * 1000), 1000};
    impl_->codecCtx->gop_size = 12;
    impl_->codecCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
    
    if (impl_->fmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        impl_->codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    
    av_opt_set(impl_->codecCtx->priv_data, "preset", cfg.preset.c_str(), 0);
    av_opt_set(impl_->codecCtx->priv_data, "tune", cfg.tune.c_str(), 0);

    if (avcodec_open2(impl_->codecCtx, codec, nullptr) < 0) {
        logError(TAG, "avcodec_open2 failed");
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
        impl_->setStatus(RtmpStatus::Error, "Failed to open H.264 encoder");
        return false;
    }

    // Create output stream
    impl_->stream = avformat_new_stream(impl_->fmtCtx, nullptr);
    if (!impl_->stream) {
        logError(TAG, "avformat_new_stream failed");
        impl_->setStatus(RtmpStatus::Error, "Failed to create output stream");
        return false;
    }
    avcodec_parameters_from_context(impl_->stream->codecpar, impl_->codecCtx);
    impl_->stream->time_base = impl_->codecCtx->time_base;

    // Open I/O and write header
    const std::string url = impl_->config.serverUrl + "/" + impl_->config.streamKey;
    if (avio_open(&impl_->fmtCtx->pb, url.c_str(), AVIO_FLAG_WRITE) < 0) {
        logError(TAG, "avio_open failed for " + url);
        impl_->setStatus(RtmpStatus::Error, "Failed to open RTMP connection");
        return false;
    }
    
    if (avformat_write_header(impl_->fmtCtx, nullptr) < 0) {
        logError(TAG, "avformat_write_header failed");
        impl_->setStatus(RtmpStatus::Error, "Failed to write stream header");
        return false;
    }

    impl_->pkt      = av_packet_alloc();
    impl_->frameIdx = 0;
    impl_->isStreaming = true;
    impl_->setStatus(RtmpStatus::Live, "Stream started");
    logInfo(TAG, "RTMP stream started");
    return true;
#else
    logWarn(TAG, "FFmpeg not available");
    return false;
#endif
}

bool FFmpegRtmpOutput::stop() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->isStreaming) return true;
    
    logInfo(TAG, "Stopping RTMP stream");
    
#ifdef HAS_FFMPEG
    if (impl_->fmtCtx && impl_->fmtCtx->pb) {
        av_write_trailer(impl_->fmtCtx);
    }
    if (impl_->codecCtx) {
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
    }
    if (impl_->pkt) {
        av_packet_free(&impl_->pkt);
        impl_->pkt = nullptr;
    }
    impl_->stream   = nullptr;
    impl_->frameIdx = 0;
#endif

    impl_->isStreaming = false;
    impl_->setStatus(RtmpStatus::Idle, "Stream stopped");
    return true;
}

bool FFmpegRtmpOutput::isStreaming() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->isStreaming;
}

bool FFmpegRtmpOutput::sendFrame(const RtmpFrame& frame) {
#ifdef HAS_FFMPEG
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (!impl_->isStreaming || !impl_->codecCtx || !impl_->pkt) {
        return false;
    }

    AVFrame* avFrame = av_frame_alloc();
    if (!avFrame) return false;
    
    avFrame->format = AV_PIX_FMT_YUV420P;
    avFrame->width  = frame.width;
    avFrame->height = frame.height;
    avFrame->pts    = impl_->frameIdx++;
    
    if (av_frame_get_buffer(avFrame, 0) < 0) {
        av_frame_free(&avFrame);
        return false;
    }

    // Convert frame data to YUV420P for the encoder
    if (frame.data) {
        const size_t lumaSize = static_cast<size_t>(frame.stride) * frame.height;
        
        if (frame.isNV12) {
            // Convert NV12 (semi-planar) to YUV420P (planar)
            // Copy Y plane directly
            std::memcpy(avFrame->data[0], frame.data, lumaSize);
            
            // De-interleave NV12 UV into separate U and V planes
            const uint8_t* uv = frame.data + lumaSize;
            const int chromaPixels = (frame.width / 2) * (frame.height / 2);
            uint8_t* uPlane = avFrame->data[1];
            uint8_t* vPlane = avFrame->data[2];
            for (int i = 0; i < chromaPixels; ++i) {
                uPlane[i] = uv[2 * i];
                vPlane[i] = uv[2 * i + 1];
            }
        } else {
            // Assume YUV420P input — copy planes directly
            std::memcpy(avFrame->data[0], frame.data, lumaSize);
            const size_t chromaSize = lumaSize / 4;
            std::memcpy(avFrame->data[1], frame.data + lumaSize, chromaSize);
            std::memcpy(avFrame->data[2], frame.data + lumaSize + chromaSize, chromaSize);
        }
    }

    bool sent = false;
    if (avcodec_send_frame(impl_->codecCtx, avFrame) >= 0) {
        while (avcodec_receive_packet(impl_->codecCtx, impl_->pkt) == 0) {
            av_packet_rescale_ts(impl_->pkt,
                                 impl_->codecCtx->time_base,
                                 impl_->stream->time_base);
            impl_->pkt->stream_index = impl_->stream->index;
            
            int ret = av_interleaved_write_frame(impl_->fmtCtx, impl_->pkt);
            if (ret < 0) {
                char errbuf[128];
                av_strerror(ret, errbuf, sizeof(errbuf));
                logWarn(TAG, std::string("av_interleaved_write_frame: ") + errbuf);
            }
            av_packet_unref(impl_->pkt);
            sent = true;
        }
    }
    
    av_frame_free(&avFrame);
    return sent;
#else
    (void)frame;
    return false;
#endif
}

RtmpStatus FFmpegRtmpOutput::status() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->status;
}

void FFmpegRtmpOutput::setStatusCallback(RtmpStatusCallback callback) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->statusCallback = std::move(callback);
}

std::string FFmpegRtmpOutput::lastError() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->lastError;
}

std::vector<std::pair<int, int>> FFmpegRtmpOutput::supportedResolutions() {
    return {
        {1280, 720},   // 720p
        {1920, 1080},  // 1080p
        {2560, 1440},  // 1440p
        {3840, 2160},  // 4K UHD
    };
}

std::vector<double> FFmpegRtmpOutput::supportedFrameRates() {
    return {
        24.0,
        25.0,
        29.97,
        30.0,
        50.0,
        59.94,
        60.0,
    };
}

} // namespace visioncast

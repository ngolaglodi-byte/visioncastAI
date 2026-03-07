/// @file srt_output.cpp
/// @brief SRT send-only implementation.

#include "visioncast_sdk/srt_output.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_SRT
#include <srt/srt.h>
#include <winsock2.h>
#include <ws2tcpip.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#endif

#include <mutex>

static const char* TAG = "SRTOutput";

struct SRTOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "SRT Output";
    std::string destination = "srt://localhost:9000";
    int latencyMs = 120;
    std::mutex mutex;

#ifdef HAS_SRT
    SRTSOCKET    socket    = SRT_INVALID_SOCK;
    AVCodecContext* codecCtx = nullptr;
    AVPacket*       pkt      = nullptr;
#endif
};

SRTOutput::SRTOutput() : impl_(std::make_unique<Impl>()) {}
SRTOutput::~SRTOutput() { close(); }

bool SRTOutput::open(const DeviceConfig& config) {
#ifdef HAS_SRT
    std::lock_guard<std::mutex> lock(impl_->mutex);
    SDKLogger::info(TAG, "Opening SRT output to " + impl_->destination);
    impl_->name = config.name.empty() ? "SRT Output" : config.name;

    // Parse "srt://host:port" URL
    std::string url = impl_->destination;
    // Strip scheme
    const std::string scheme = "srt://";
    if (url.substr(0, scheme.size()) == scheme) url = url.substr(scheme.size());
    std::string host;
    int port = 9000;
    auto colonPos = url.rfind(':');
    if (colonPos != std::string::npos) {
        host = url.substr(0, colonPos);
        port = std::stoi(url.substr(colonPos + 1));
    } else {
        host = url;
    }

    srt_startup();
    impl_->socket = srt_create_socket();
    if (impl_->socket == SRT_INVALID_SOCK) {
        SDKLogger::error(TAG, "srt_create_socket failed: " +
                              std::string(srt_getlasterror_str()));
        return false;
    }

    // Set latency option
    srt_setsockopt(impl_->socket, 0, SRTO_LATENCY,
                   &impl_->latencyMs, sizeof(impl_->latencyMs));
    int liveMode = 1;
    srt_setsockopt(impl_->socket, 0, SRTO_SENDER, &liveMode, sizeof(liveMode));

    struct sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_port   = htons(static_cast<uint16_t>(port));
    inet_pton(AF_INET, host.c_str(), &sa.sin_addr);

    if (srt_connect(impl_->socket,
                    reinterpret_cast<const sockaddr*>(&sa),
                    sizeof(sa)) == SRT_ERROR) {
        SDKLogger::error(TAG, "srt_connect failed: " +
                              std::string(srt_getlasterror_str()));
        srt_close(impl_->socket);
        impl_->socket = SRT_INVALID_SOCK;
        return false;
    }

    impl_->isOpen = true;
    SDKLogger::info(TAG, "SRT connected to " + impl_->destination);
    return true;
#else
    SDKLogger::warn(TAG, "SRT library not available — output will not open");
    return false;
#endif
}

void SRTOutput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing SRT output: " + impl_->name);
    impl_->playing = false;
#ifdef HAS_SRT
    if (impl_->codecCtx) {
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
    }
    if (impl_->pkt) {
        av_packet_free(&impl_->pkt);
        impl_->pkt = nullptr;
    }
    if (impl_->socket != SRT_INVALID_SOCK) {
        srt_close(impl_->socket);
        impl_->socket = SRT_INVALID_SOCK;
    }
    srt_cleanup();
#endif
    impl_->isOpen = false;
    SDKLogger::info(TAG, "SRT output closed");
}

bool SRTOutput::isOpen() const { return impl_->isOpen; }
std::string SRTOutput::deviceName() const { return impl_->name; }
DeviceType SRTOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> SRTOutput::supportedModes() const {
    return {
        {1920, 1080, 25.0,   PixelFormat::NV12, false},
        {1920, 1080, 29.97,  PixelFormat::NV12, false},
        {1920, 1080, 50.0,   PixelFormat::NV12, false},
        {1920, 1080, 59.94,  PixelFormat::NV12, false},
        {3840, 2160, 25.0,   PixelFormat::NV12, false},
        {3840, 2160, 29.97,  PixelFormat::NV12, false},
        {3840, 2160, 50.0,   PixelFormat::NV12, false},
        {3840, 2160, 59.94,  PixelFormat::NV12, false},
    };
}

// Capture no-ops
bool SRTOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool SRTOutput::stopCapture() { return false; }
VideoFrame SRTOutput::captureFrame() { return VideoFrame{}; }

bool SRTOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_SRT
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) {
        SDKLogger::error(TAG, "startPlayout() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting SRT playout " +
                         std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));

    // Set up H.264 encoder via FFmpeg for the chosen mode.
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        SDKLogger::error(TAG, "H.264 encoder not found");
        return false;
    }
    impl_->codecCtx = avcodec_alloc_context3(codec);
    impl_->codecCtx->bit_rate = 6000000;
    impl_->codecCtx->width    = mode.width;
    impl_->codecCtx->height   = mode.height;
    impl_->codecCtx->time_base = {1, static_cast<int>(mode.frameRate * 1000)};
    impl_->codecCtx->framerate = {static_cast<int>(mode.frameRate * 1000), 1000};
    impl_->codecCtx->gop_size = 12;
    impl_->codecCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
    av_opt_set(impl_->codecCtx->priv_data, "preset", "veryfast", 0);
    av_opt_set(impl_->codecCtx->priv_data, "tune",   "zerolatency", 0);

    if (avcodec_open2(impl_->codecCtx, codec, nullptr) < 0) {
        SDKLogger::error(TAG, "avcodec_open2 failed");
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
        return false;
    }
    impl_->pkt = av_packet_alloc();
    impl_->currentMode = mode;
    impl_->playing = true;
    SDKLogger::info(TAG, "SRT playout started");
    return true;
#else
    return false;
#endif
}

bool SRTOutput::stopPlayout() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->playing) return true;
    SDKLogger::info(TAG, "Stopping SRT playout");
#ifdef HAS_SRT
    if (impl_->codecCtx) {
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
    }
    if (impl_->pkt) {
        av_packet_free(&impl_->pkt);
        impl_->pkt = nullptr;
    }
#endif
    impl_->playing = false;
    return true;
}

bool SRTOutput::sendFrame(const VideoFrame& frame) {
#ifdef HAS_SRT
    if (!impl_->playing || !impl_->codecCtx || !impl_->pkt) return false;

    AVFrame* avFrame = av_frame_alloc();
    if (!avFrame) return false;
    avFrame->format = AV_PIX_FMT_YUV420P;
    avFrame->width  = frame.width;
    avFrame->height = frame.height;
    if (av_frame_get_buffer(avFrame, 0) < 0) {
        av_frame_free(&avFrame);
        return false;
    }
    // Convert NV12 (semi-planar) to YUV420P (planar) for the encoder
    if (frame.data) {
        const size_t lumaSize = static_cast<size_t>(frame.stride) * frame.height;
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
    }

    bool sent = false;
    if (avcodec_send_frame(impl_->codecCtx, avFrame) >= 0) {
        while (avcodec_receive_packet(impl_->codecCtx, impl_->pkt) == 0) {
            int rc = srt_sendmsg(impl_->socket,
                                 reinterpret_cast<const char*>(impl_->pkt->data),
                                 impl_->pkt->size,
                                 -1 /*infinite TTL*/,
                                 0  /*not in-order*/);
            if (rc == SRT_ERROR)
                SDKLogger::warn(TAG, "srt_sendmsg error: " +
                                     std::string(srt_getlasterror_str()));
            av_packet_unref(impl_->pkt);
            sent = true;
        }
    }
    av_frame_free(&avFrame);
    return sent;
#endif
    (void)frame;
    return impl_->playing;
}

void SRTOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode SRTOutput::currentMode() const { return impl_->currentMode; }

// SRTOutput-specific
void SRTOutput::setDestination(const std::string& url) {
    impl_->destination = url;
}

std::string SRTOutput::destination() const {
    return impl_->destination;
}

void SRTOutput::setLatency(int ms) {
    impl_->latencyMs = ms;
}

int SRTOutput::latency() const {
    return impl_->latencyMs;
}

bool SRTOutput::isOutputActive() const {
    return impl_->playing;
}


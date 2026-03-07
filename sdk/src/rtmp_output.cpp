/// @file rtmp_output.cpp
/// @brief RTMP send-only implementation.

#include "visioncast_sdk/rtmp_output.h"
#include "visioncast_sdk/sdk_error.h"
#include "visioncast_sdk/sdk_logger.h"

#ifdef HAS_RTMP
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#endif

#include <cstring>
#include <mutex>

static const char* TAG = "RTMPOutput";

struct RTMPOutput::Impl {
    bool isOpen = false;
    bool playing = false;
    VideoMode currentMode;
    std::string name = "RTMP Output";
    std::string serverUrl;
    std::string streamKey;
    std::mutex mutex;

#ifdef HAS_RTMP
    AVFormatContext* fmtCtx   = nullptr;
    AVCodecContext*  codecCtx = nullptr;
    AVStream*        stream   = nullptr;
    AVPacket*        pkt      = nullptr;
    int64_t          frameIdx = 0;
#endif
};

RTMPOutput::RTMPOutput() : impl_(std::make_unique<Impl>()) {}
RTMPOutput::~RTMPOutput() { close(); }

bool RTMPOutput::open(const DeviceConfig& config) {
#ifdef HAS_RTMP
    std::lock_guard<std::mutex> lock(impl_->mutex);
    const std::string url = impl_->serverUrl + "/" + impl_->streamKey;
    SDKLogger::info(TAG, "Opening RTMP output to " + url);
    impl_->name = config.name.empty() ? "RTMP Output" : config.name;

    int ret = avformat_alloc_output_context2(
                  &impl_->fmtCtx, nullptr, "flv", url.c_str());
    if (ret < 0 || !impl_->fmtCtx) {
        char errbuf[128];
        av_strerror(ret, errbuf, sizeof(errbuf));
        SDKLogger::error(TAG, std::string("avformat_alloc_output_context2 failed: ") + errbuf);
        return false;
    }
    impl_->isOpen = true;
    SDKLogger::info(TAG, "RTMP context created for " + url);
    return true;
#else
    SDKLogger::warn(TAG, "RTMP library not available — output will not open");
    return false;
#endif
}

void RTMPOutput::close() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen) return;
    SDKLogger::info(TAG, "Closing RTMP output: " + impl_->name);
    impl_->playing = false;
#ifdef HAS_RTMP
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
    SDKLogger::info(TAG, "RTMP output closed");
}

bool RTMPOutput::isOpen() const { return impl_->isOpen; }
std::string RTMPOutput::deviceName() const { return impl_->name; }
DeviceType RTMPOutput::deviceType() const { return DeviceType::PLAYOUT; }

std::vector<VideoMode> RTMPOutput::supportedModes() const {
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
bool RTMPOutput::startCapture(const VideoMode& /*mode*/) { return false; }
bool RTMPOutput::stopCapture() { return false; }
VideoFrame RTMPOutput::captureFrame() { return VideoFrame{}; }

bool RTMPOutput::startPlayout(const VideoMode& mode) {
#ifdef HAS_RTMP
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->isOpen || !impl_->fmtCtx) {
        SDKLogger::error(TAG, "startPlayout() called on closed device");
        return false;
    }
    SDKLogger::info(TAG, "Starting RTMP playout " +
                         std::to_string(mode.width) + "x" +
                         std::to_string(mode.height) + "@" +
                         std::to_string(mode.frameRate));

    // Create H.264 encoder
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        SDKLogger::error(TAG, "H.264 encoder not found");
        return false;
    }
    impl_->codecCtx = avcodec_alloc_context3(codec);
    impl_->codecCtx->bit_rate = 4000000;
    impl_->codecCtx->width    = mode.width;
    impl_->codecCtx->height   = mode.height;
    impl_->codecCtx->time_base = {1000, static_cast<int>(mode.frameRate * 1000)};
    impl_->codecCtx->framerate = {static_cast<int>(mode.frameRate * 1000), 1000};
    impl_->codecCtx->gop_size = 12;
    impl_->codecCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
    if (impl_->fmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
        impl_->codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    av_opt_set(impl_->codecCtx->priv_data, "preset", "veryfast", 0);
    av_opt_set(impl_->codecCtx->priv_data, "tune",   "zerolatency", 0);

    if (avcodec_open2(impl_->codecCtx, codec, nullptr) < 0) {
        SDKLogger::error(TAG, "avcodec_open2 failed");
        avcodec_free_context(&impl_->codecCtx);
        impl_->codecCtx = nullptr;
        return false;
    }

    // Create output stream
    impl_->stream = avformat_new_stream(impl_->fmtCtx, nullptr);
    if (!impl_->stream) {
        SDKLogger::error(TAG, "avformat_new_stream failed");
        return false;
    }
    avcodec_parameters_from_context(impl_->stream->codecpar, impl_->codecCtx);
    impl_->stream->time_base = impl_->codecCtx->time_base;

    // Open I/O and write header
    const std::string url = impl_->serverUrl + "/" + impl_->streamKey;
    if (avio_open(&impl_->fmtCtx->pb, url.c_str(), AVIO_FLAG_WRITE) < 0) {
        SDKLogger::error(TAG, "avio_open failed for " + url);
        return false;
    }
    if (avformat_write_header(impl_->fmtCtx, nullptr) < 0) {
        SDKLogger::error(TAG, "avformat_write_header failed");
        return false;
    }

    impl_->pkt      = av_packet_alloc();
    impl_->frameIdx = 0;
    impl_->currentMode = mode;
    impl_->playing = true;
    SDKLogger::info(TAG, "RTMP playout started");
    return true;
#else
    return false;
#endif
}

bool RTMPOutput::stopPlayout() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->playing) return true;
    SDKLogger::info(TAG, "Stopping RTMP playout");
#ifdef HAS_RTMP
    if (impl_->fmtCtx && impl_->fmtCtx->pb)
        av_write_trailer(impl_->fmtCtx);
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
    impl_->playing = false;
    return true;
}

bool RTMPOutput::sendFrame(const VideoFrame& frame) {
#ifdef HAS_RTMP
    if (!impl_->playing || !impl_->codecCtx || !impl_->pkt) return false;

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
            av_packet_rescale_ts(impl_->pkt,
                                 impl_->codecCtx->time_base,
                                 impl_->stream->time_base);
            impl_->pkt->stream_index = impl_->stream->index;
            int ret = av_interleaved_write_frame(impl_->fmtCtx, impl_->pkt);
            if (ret < 0) {
                char errbuf[128];
                av_strerror(ret, errbuf, sizeof(errbuf));
                SDKLogger::warn(TAG, std::string("av_interleaved_write_frame: ") + errbuf);
            }
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

void RTMPOutput::setVideoMode(const VideoMode& mode) { impl_->currentMode = mode; }
VideoMode RTMPOutput::currentMode() const { return impl_->currentMode; }

// RTMPOutput-specific
void RTMPOutput::setServerUrl(const std::string& url) {
    impl_->serverUrl = url;
}

std::string RTMPOutput::serverUrl() const {
    return impl_->serverUrl;
}

void RTMPOutput::setStreamKey(const std::string& key) {
    impl_->streamKey = key;
}

std::string RTMPOutput::streamKey() const {
    return impl_->streamKey;
}

bool RTMPOutput::isOutputActive() const {
    return impl_->playing;
}


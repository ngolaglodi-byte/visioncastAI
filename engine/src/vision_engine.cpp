/// @file vision_engine.cpp
/// @brief VisionEngine implementation — video capture, GPU pipeline,
///        preview window, and render loop.
///
/// When HAS_OPENGL is defined the engine creates a GLFW window with an
/// OpenGL 3.3 core-profile context, uploads frames as GL textures, and
/// renders them with a full-screen-quad shader.
///
/// Without HAS_OPENGL the engine falls back to OpenCV highgui for the
/// preview window (CPU path).

#include "visioncast/vision_engine.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "visioncast/gpu_compositor.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

// ---- Optional OpenGL path ------------------------------------------
#ifdef HAS_OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

namespace visioncast {

// =====================================================================
// GLSL sources (compiled at runtime when HAS_OPENGL is available)
// =====================================================================

#ifdef HAS_OPENGL
static const char* kVertexShaderSrc = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord    = aTexCoord;
}
)glsl";

static const char* kFragmentShaderSrc = R"glsl(
#version 330 core
in  vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
void main() {
    FragColor = texture(uTexture, TexCoord);
}
)glsl";
#endif // HAS_OPENGL

// =====================================================================
// Impl (PIMPL)
// =====================================================================

struct VisionEngine::Impl {
    VisionEngineConfig config;

    // ---- Capture (OpenCV) -------------------------------------------
    cv::VideoCapture   capture;
    cv::Mat            cvFrame;         // Latest captured frame (BGR/BGRA/GRAY).
    mutable std::mutex captureMutex;

    // Source tracking (for professional reopen/recovery)
    bool        isFileSource = false;
    int         lastDeviceIndex = 0;
    std::string lastUri;

    // Failure tracking / recovery throttling
    int failCount = 0;
    std::chrono::steady_clock::time_point lastRecoverAttempt{
        std::chrono::steady_clock::now()
    };

    // EOF logging (avoid spam)
    bool eofLogged = false;

    // ---- Frame buffer for the VideoFrame API ------------------------
    std::vector<uint8_t> frameBuffer;  // Pixel data backing store.

    // ---- Preview -----------------------------------------------------
    std::string windowName;
    bool        previewOpen = false;

    // ---- GPU pipeline -----------------------------------------------
    bool gpuReady = false;

#ifdef HAS_OPENGL
    GLFWwindow* glfwWindow      = nullptr;
    GLuint      shaderProgram   = 0;
    GLuint      vao             = 0;
    GLuint      vbo             = 0;
    GLuint      ebo             = 0;
    GLuint      previewTexture  = 0;   // Persistent texture for render loop.
#endif

    // ---- Render loop ------------------------------------------------
    std::atomic<bool>           running{false};
    VisionEngine::FrameCallback frameCallback;

    // ---- GPU Compositor ---------------------------------------------
    GpuCompositor compositor;

    // ---- Helpers ----------------------------------------------------

    /// Fill a VideoFrame struct from the internal cv::Mat.
    /// Converts BGR (3-channel) frames to BGRA (4-channel) so the pixel
    /// format always matches PixelFormat::BGRA8 for GPU upload.
    void matToVideoFrame(const cv::Mat& mat, VideoFrame& out) {
        if (mat.empty()) {
            out = VideoFrame{};
            return;
        }

        cv::Mat bgra;
        if (mat.channels() == 3) {
            cv::cvtColor(mat, bgra, cv::COLOR_BGR2BGRA);
        } else if (mat.channels() == 4) {
            bgra = mat;
        } else {
            cv::cvtColor(mat, bgra, cv::COLOR_GRAY2BGRA);
        }

        const size_t bytes = static_cast<size_t>(bgra.total() * bgra.elemSize());
        frameBuffer.resize(bytes);

        if (bgra.isContinuous()) {
            std::memcpy(frameBuffer.data(), bgra.data, bytes);
        } else {
            const size_t rowBytes = static_cast<size_t>(bgra.cols * bgra.elemSize());
            for (int y = 0; y < bgra.rows; ++y) {
                std::memcpy(frameBuffer.data() + static_cast<size_t>(y) * rowBytes,
                            bgra.ptr(y),
                            rowBytes);
            }
        }

        out.data        = frameBuffer.data();
        out.width       = bgra.cols;
        out.height      = bgra.rows;
        out.stride      = static_cast<int>(bgra.step[0]);
        out.format      = PixelFormat::BGRA8;
        out.timestampUs = std::chrono::duration_cast<std::chrono::microseconds>(
                              std::chrono::steady_clock::now().time_since_epoch())
                              .count();
    }

#ifdef HAS_OPENGL
    // ---- OpenGL helpers ---------------------------------------------

    static GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint ok = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            std::cerr << "[VisionEngine] Shader compile error: " << log << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    bool buildShaderProgram() {
        GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
        if (vs == 0 || fs == 0) return false;

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fs);
        glLinkProgram(shaderProgram);

        GLint ok = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[512];
            glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);
            std::cerr << "[VisionEngine] Shader link error: " << log << std::endl;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
        return ok != 0;
    }

    bool buildFullscreenQuad() {
        // Two triangles covering [-1,1] in NDC, with flipped-Y tex coords.
        // clang-format off
        const float vertices[] = {
            // pos        // texcoord
            -1.f,  1.f,   0.f, 0.f,
             1.f,  1.f,   1.f, 0.f,
             1.f, -1.f,   1.f, 1.f,
            -1.f, -1.f,   0.f, 1.f,
        };
        const unsigned int indices[] = { 0, 1, 2,  2, 3, 0 };
        // clang-format on

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // aPos (location 0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // aTexCoord (location 1)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              reinterpret_cast<void*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        return true;
    }

    /// Render one frame: bind texture → draw full-screen quad → swap.
    void renderFrame(GLuint texId) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }
#endif // HAS_OPENGL
};

// =====================================================================
// Construction / Destruction
// =====================================================================

VisionEngine::VisionEngine()
    : impl_(std::make_unique<Impl>()) {}

VisionEngine::VisionEngine(const VisionEngineConfig& config)
    : impl_(std::make_unique<Impl>()) {
    impl_->config = config;
}

VisionEngine::~VisionEngine() {
    shutdown();
}

// =====================================================================
// Lifecycle
// =====================================================================

bool VisionEngine::initialize() {
    const auto& cfg = impl_->config;

    // 1) Open capture source.
    bool captureOk = cfg.captureUri.empty()
                         ? openCapture(cfg.captureDeviceIndex)
                         : openCapture(cfg.captureUri);
    if (!captureOk) {
        std::cerr << "[VisionEngine] Failed to open capture source." << std::endl;
        return false;
    }

    // Professional: if capture reports real dimensions, use them for preview
    {
        std::lock_guard<std::mutex> lock(impl_->captureMutex);
        const int capW = static_cast<int>(impl_->capture.get(cv::CAP_PROP_FRAME_WIDTH));
        const int capH = static_cast<int>(impl_->capture.get(cv::CAP_PROP_FRAME_HEIGHT));
        if (capW > 0 && capH > 0) {
            impl_->config.previewWidth  = capW;
            impl_->config.previewHeight = capH;
        }
    }

    // 2) Create preview window.
    if (!createPreviewWindow(cfg.windowTitle, impl_->config.previewWidth, impl_->config.previewHeight)) {
        std::cerr << "[VisionEngine] Failed to create preview window." << std::endl;
        closeCapture();
        return false;
    }

    // 3) GPU pipeline (optional).
    if (cfg.enableGpu) {
        if (!initGpuPipeline()) {
            std::cerr << "[VisionEngine] GPU pipeline unavailable; using CPU fallback."
                      << std::endl;
        }
    }

    // 4) GPU compositor (works in both CPU and GPU modes).
    impl_->compositor.initialize(impl_->config.previewWidth, impl_->config.previewHeight);

    std::cout << "[VisionEngine] Initialized ("
              << impl_->config.previewWidth << "x" << impl_->config.previewHeight
              << " @ " << cfg.targetFps << " fps)." << std::endl;
    return true;
}

void VisionEngine::shutdown() {
    stop();
    impl_->compositor.shutdown();
    shutdownGpuPipeline();
    destroyPreviewWindow();
    closeCapture();
    std::cout << "[VisionEngine] Shutdown complete." << std::endl;
}

// =====================================================================
// Video Capture
// =====================================================================

bool VisionEngine::openCapture(int deviceIndex) {
    std::lock_guard<std::mutex> lock(impl_->captureMutex);

    impl_->isFileSource    = false;
    impl_->lastDeviceIndex = deviceIndex;
    impl_->lastUri.clear();
    impl_->failCount = 0;
    impl_->eofLogged = false;

    if (impl_->capture.isOpened()) {
        impl_->capture.release();
    }

#ifdef _WIN32
    // Prefer DirectShow on Windows for webcams.
    bool ok = impl_->capture.open(deviceIndex, cv::CAP_DSHOW);
    if (!ok) ok = impl_->capture.open(deviceIndex);
#else
    bool ok = impl_->capture.open(deviceIndex);
#endif

    if (!ok) return false;

#ifdef _WIN32
    // Force MJPG when possible (many webcams are more stable with MJPG).
    impl_->capture.set(cv::CAP_PROP_FOURCC,
                       cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
#endif

    // Request target settings (device may ignore)
    impl_->capture.set(cv::CAP_PROP_FRAME_WIDTH,  impl_->config.previewWidth);
    impl_->capture.set(cv::CAP_PROP_FRAME_HEIGHT, impl_->config.previewHeight);
    impl_->capture.set(cv::CAP_PROP_FPS,          impl_->config.targetFps);

    // Diagnostics
    const double w = impl_->capture.get(cv::CAP_PROP_FRAME_WIDTH);
    const double h = impl_->capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    const double fps = impl_->capture.get(cv::CAP_PROP_FPS);
    const double fourcc = impl_->capture.get(cv::CAP_PROP_FOURCC);

    char fcc[] = {
        static_cast<char>(static_cast<int>(fourcc) & 0xFF),
        static_cast<char>((static_cast<int>(fourcc) >> 8) & 0xFF),
        static_cast<char>((static_cast<int>(fourcc) >> 16) & 0xFF),
        static_cast<char>((static_cast<int>(fourcc) >> 24) & 0xFF),
        0
    };

    std::cout << "[VisionEngine] Capture opened (device " << deviceIndex << ")." << std::endl;
    std::cout << "[VisionEngine] Capture props: " << w << "x" << h
              << " @ " << fps << " fps"
              << " fourcc=" << fcc
              << std::endl;

    return true;
}

bool VisionEngine::openCapture(const std::string& uri) {
    std::lock_guard<std::mutex> lock(impl_->captureMutex);

    impl_->isFileSource = true;
    impl_->lastUri = uri;
    impl_->failCount = 0;
    impl_->eofLogged = false;

    if (impl_->capture.isOpened()) {
        impl_->capture.release();
    }

    // Normalize Windows paths for OpenCV: backslashes -> slashes.
    std::string norm = uri;
    for (char& c : norm) {
        if (c == '\\') c = '/';
    }

    bool ok = false;

#ifdef _WIN32
    // Prefer FFMPEG for files/URIs on Windows when available.
    ok = impl_->capture.open(norm, cv::CAP_FFMPEG);
    if (!ok) ok = impl_->capture.open(norm);  // AUTO fallback
    if (!ok) ok = impl_->capture.open(norm, cv::CAP_MSMF);
    if (!ok) ok = impl_->capture.open(norm, cv::CAP_DSHOW);
#else
    ok = impl_->capture.open(norm);
#endif

    if (!ok) {
        std::cerr << "[VisionEngine] Could not open capture URI: " << uri << std::endl;
#ifdef CV_VERSION
        std::cerr << "[VisionEngine] OpenCV version: " << CV_VERSION << std::endl;
#endif
        std::cerr << "[VisionEngine] Tried: "
#ifdef _WIN32
                  << "FFMPEG -> AUTO -> MSMF -> DSHOW"
#else
                  << "AUTO"
#endif
                  << std::endl;
        return false;
    }

    std::cout << "[VisionEngine] Capture opened (" << uri << ")." << std::endl;

    const double w = impl_->capture.get(cv::CAP_PROP_FRAME_WIDTH);
    const double h = impl_->capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    const double fps = impl_->capture.get(cv::CAP_PROP_FPS);
    const double frameCount = impl_->capture.get(cv::CAP_PROP_FRAME_COUNT);

    std::cout << "[VisionEngine] Capture props: " << w << "x" << h << " @ " << fps << " fps"
              << " frames=" << frameCount
              << std::endl;

    return true;
}

void VisionEngine::closeCapture() {
    std::lock_guard<std::mutex> lock(impl_->captureMutex);
    if (impl_->capture.isOpened()) {
        impl_->capture.release();
        std::cout << "[VisionEngine] Capture closed." << std::endl;
    }
}

bool VisionEngine::isCaptureOpen() const {
    std::lock_guard<std::mutex> lock(impl_->captureMutex);
    return impl_->capture.isOpened();
}

bool VisionEngine::grabFrame(VideoFrame& outFrame) {
    std::lock_guard<std::mutex> lock(impl_->captureMutex);

    if (!impl_->capture.isOpened()) {
        outFrame = VideoFrame{};
        return false;
    }

    if (!impl_->capture.read(impl_->cvFrame) || impl_->cvFrame.empty()) {
        const double pos   = impl_->capture.get(cv::CAP_PROP_POS_FRAMES);
        const double count = impl_->capture.get(cv::CAP_PROP_FRAME_COUNT);

        // Professional file behavior: configurable on EOF (OBS-like loop on/off)
        if (impl_->isFileSource && count > 0 && pos >= count - 1) {
            if (impl_->config.loopMedia) {
                impl_->capture.set(cv::CAP_PROP_POS_FRAMES, 0);
                if (impl_->capture.read(impl_->cvFrame) && !impl_->cvFrame.empty()) {
                    impl_->failCount = 0;
                    impl_->matToVideoFrame(impl_->cvFrame, outFrame);
                    return true;
                }
            } else {
                if (!impl_->eofLogged) {
                    impl_->eofLogged = true;
                    std::cout << "[VisionEngine] End of file reached; stopping (loopMedia=false)."
                              << std::endl;
                }
                impl_->running = false;
                outFrame = VideoFrame{};
                return false;
            }
        }

        impl_->failCount++;

        // Throttle log (every 30 fails)
        if (impl_->failCount == 1 || (impl_->failCount % 30) == 0) {
            std::cerr << "[VisionEngine] WARN: capture.read() failed/empty (count="
                      << impl_->failCount << "). pos=" << pos << " count=" << count
                      << std::endl;
        }

        // Professional webcam behavior: attempt recovery with backoff
        if (!impl_->isFileSource && impl_->failCount >= 60) {
            auto now = std::chrono::steady_clock::now();
            if (now - impl_->lastRecoverAttempt > std::chrono::seconds(2)) {
                impl_->lastRecoverAttempt = now;
                std::cerr << "[VisionEngine] Attempting webcam recovery (re-open device "
                          << impl_->lastDeviceIndex << ")." << std::endl;
#ifdef _WIN32
                impl_->capture.release();
                impl_->capture.open(impl_->lastDeviceIndex, cv::CAP_DSHOW);
#else
                impl_->capture.release();
                impl_->capture.open(impl_->lastDeviceIndex);
#endif
                impl_->failCount = 0;
            }
        }

        outFrame = VideoFrame{};
        return false;
    }

    impl_->failCount = 0;
    impl_->matToVideoFrame(impl_->cvFrame, outFrame);
    return true;
}

// =====================================================================
// Preview Window
// =====================================================================

bool VisionEngine::createPreviewWindow(const std::string& title, int width, int height) {
#ifdef HAS_OPENGL
    // When using the GPU path, the GLFW window IS the preview window.
    // It is created in initGpuPipeline(), so just record the intent here.
    impl_->windowName = title;
    impl_->config.previewWidth  = width;
    impl_->config.previewHeight = height;
    impl_->previewOpen = true;
    return true;
#else
    // OpenCV highgui fallback.
    impl_->windowName = title;
    cv::namedWindow(title, cv::WINDOW_NORMAL);
    cv::resizeWindow(title, width, height);
    impl_->previewOpen = true;
    std::cout << "[VisionEngine] Preview window created (OpenCV fallback)." << std::endl;
    return true;
#endif
}

void VisionEngine::destroyPreviewWindow() {
#ifdef HAS_OPENGL
    // GLFW window destroyed in shutdownGpuPipeline().
#else
    if (impl_->previewOpen && !impl_->windowName.empty()) {
        cv::destroyWindow(impl_->windowName);
    }
#endif
    impl_->previewOpen = false;
}

bool VisionEngine::isPreviewOpen() const {
#ifdef HAS_OPENGL
    return impl_->glfwWindow != nullptr && !glfwWindowShouldClose(impl_->glfwWindow);
#else
    return impl_->previewOpen;
#endif
}

// =====================================================================
// GPU Pipeline (OpenGL)
// =====================================================================

bool VisionEngine::initGpuPipeline() {
#ifdef HAS_OPENGL
    if (!glfwInit()) {
        std::cerr << "[VisionEngine] glfwInit failed." << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    impl_->glfwWindow = glfwCreateWindow(
        impl_->config.previewWidth,
        impl_->config.previewHeight,
        impl_->windowName.c_str(),
        nullptr,
        nullptr);

    if (!impl_->glfwWindow) {
        std::cerr << "[VisionEngine] glfwCreateWindow failed." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(impl_->glfwWindow);
    glfwSwapInterval(0);  // no vsync – we govern timing ourselves

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "[VisionEngine] glewInit failed." << std::endl;
        glfwDestroyWindow(impl_->glfwWindow);
        impl_->glfwWindow = nullptr;
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, impl_->config.previewWidth, impl_->config.previewHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    if (!impl_->buildShaderProgram()) {
        std::cerr << "[VisionEngine] Shader build failed." << std::endl;
        glfwDestroyWindow(impl_->glfwWindow);
        impl_->glfwWindow = nullptr;
        glfwTerminate();
        return false;
    }

    impl_->buildFullscreenQuad();

    glGenTextures(1, &impl_->previewTexture);
    glBindTexture(GL_TEXTURE_2D, impl_->previewTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    impl_->gpuReady = true;
    std::cout << "[VisionEngine] GPU pipeline initialized (OpenGL 3.3)." << std::endl;
    return true;
#else
    impl_->gpuReady = false;
    return false;
#endif
}

void VisionEngine::shutdownGpuPipeline() {
#ifdef HAS_OPENGL
    if (!impl_->gpuReady) return;

    if (impl_->previewTexture) {
        glDeleteTextures(1, &impl_->previewTexture);
        impl_->previewTexture = 0;
    }
    if (impl_->vao) { glDeleteVertexArrays(1, &impl_->vao); impl_->vao = 0; }
    if (impl_->vbo) { glDeleteBuffers(1, &impl_->vbo); impl_->vbo = 0; }
    if (impl_->ebo) { glDeleteBuffers(1, &impl_->ebo); impl_->ebo = 0; }
    if (impl_->shaderProgram) {
        glDeleteProgram(impl_->shaderProgram);
        impl_->shaderProgram = 0;
    }
    if (impl_->glfwWindow) {
        glfwDestroyWindow(impl_->glfwWindow);
        impl_->glfwWindow = nullptr;
        glfwTerminate();
    }

    impl_->gpuReady = false;
    std::cout << "[VisionEngine] GPU pipeline shut down." << std::endl;
#endif
}

bool VisionEngine::isGpuReady() const {
    return impl_->gpuReady;
}

// =====================================================================
// GPU Textures
// =====================================================================

GpuTexture VisionEngine::uploadTexture(const uint8_t* data, int width, int height, int channels) {
    GpuTexture tex;
#ifdef HAS_OPENGL
    if (!impl_->gpuReady || data == nullptr) return tex;

    const GLenum format = (channels == 4) ? GL_BGRA : GL_BGR;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);

    tex.width  = width;
    tex.height = height;
    tex.valid  = true;
#else
    (void)data; (void)width; (void)height; (void)channels;
#endif
    return tex;
}

bool VisionEngine::updateTexture(GpuTexture& texture, const uint8_t* data, int width, int height, int channels) {
#ifdef HAS_OPENGL
    if (!impl_->gpuReady || !texture.valid || data == nullptr) return false;

    const GLenum format = (channels == 4) ? GL_BGRA : GL_BGR;

    glBindTexture(GL_TEXTURE_2D, texture.id);
    if (texture.width == width && texture.height == height) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        format, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                     format, GL_UNSIGNED_BYTE, data);
        texture.width  = width;
        texture.height = height;
    }
    return true;
#else
    (void)texture; (void)data; (void)width; (void)height; (void)channels;
    return false;
#endif
}

void VisionEngine::deleteTexture(GpuTexture& texture) {
#ifdef HAS_OPENGL
    if (texture.valid && texture.id != 0) {
        glDeleteTextures(1, &texture.id);
    }
#endif
    texture = GpuTexture{};
}

// =====================================================================
// Render Loop
// =====================================================================

void VisionEngine::run() {
    if (impl_->running.exchange(true)) {
        return;  // Already running.
    }

    const auto frameDuration = std::chrono::microseconds(
        static_cast<int64_t>(1'000'000.0 / impl_->config.targetFps));

    std::cout << "[VisionEngine] Render loop started." << std::endl;

    while (impl_->running) {
        const auto loopStart = std::chrono::steady_clock::now();

        // 1) Capture
        VideoFrame frame{};
        grabFrame(frame);

        // 2) Callback
        if (frame.data && impl_->frameCallback) {
            impl_->frameCallback(frame);
        }

        // 3) Advance compositor clocks
        {
            const auto now = std::chrono::steady_clock::now();
            const double deltaMs =
                std::chrono::duration<double, std::milli>(now - loopStart).count();
            impl_->compositor.update(deltaMs);
        }

        // 4) Display / Render
#ifdef HAS_OPENGL
        if (impl_->gpuReady && impl_->glfwWindow) {
            if (glfwWindowShouldClose(impl_->glfwWindow)) {
                impl_->running = false;
                break;
            }

            if (frame.data) {
                const int ch = (frame.format == PixelFormat::BGRA8) ? 4 : 3;
                const GLenum fmt = (ch == 4) ? GL_BGRA : GL_BGR;

                glBindTexture(GL_TEXTURE_2D, impl_->previewTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                             frame.width, frame.height, 0,
                             fmt, GL_UNSIGNED_BYTE, frame.data);
            }

            impl_->renderFrame(impl_->previewTexture);

            if (impl_->compositor.isReady()) {
                impl_->compositor.compositeGpu(impl_->previewTexture, frame.width, frame.height);
            }
        }
#else
        if (frame.data && impl_->compositor.isReady()) {
            impl_->compositor.composite(frame);
        }
        if (impl_->previewOpen && !impl_->cvFrame.empty()) {
            cv::imshow(impl_->windowName, impl_->cvFrame);
            const int key = cv::waitKey(1);
            if (key == 27 /* ESC */ || key == 'q') {
                impl_->running = false;
                break;
            }
        }
#endif

        // 5) Frame-rate governor
        const auto elapsed = std::chrono::steady_clock::now() - loopStart;
        if (elapsed < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsed);
        }
    }

    std::cout << "[VisionEngine] Render loop stopped." << std::endl;
}

void VisionEngine::stop() {
    impl_->running = false;
}

bool VisionEngine::isRunning() const {
    return impl_->running;
}

// =====================================================================
// Configuration
// =====================================================================

void VisionEngine::setFrameRate(double fps) {
    if (fps > 0.0) {
        impl_->config.targetFps = fps;
    }
}

void VisionEngine::setResolution(int width, int height) {
    if (width > 0 && height > 0) {
        impl_->config.previewWidth  = width;
        impl_->config.previewHeight = height;
    }
}

void VisionEngine::setFrameCallback(FrameCallback callback) {
    impl_->frameCallback = std::move(callback);
}

// =====================================================================
// GPU Compositor Access
// =====================================================================

GpuCompositor& VisionEngine::compositor() {
    return impl_->compositor;
}

const GpuCompositor& VisionEngine::compositor() const {
    return impl_->compositor;
}

} // namespace visioncast
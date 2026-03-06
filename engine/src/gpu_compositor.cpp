/// @file gpu_compositor.cpp
/// @brief GpuCompositor implementation — PNG alpha, animations, transitions,
///        lower thirds, and dynamic JSON templates.
///
/// CPU fallback uses OpenCV alpha blending.  When HAS_OPENGL is defined the
/// compositor uploads RGBA textures and renders overlay quads with GL blending.

#include "visioncast/gpu_compositor.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#ifdef HAS_OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

// Minimal JSON helpers (no external dependency) -------------------------
// We only need to parse the template files which have a small, fixed schema.
// For robustness we use a lightweight recursive-descent approach.

namespace {

/// Trim whitespace from both ends.
std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

/// Read a quoted JSON string value for a given key from raw JSON text.
std::string jsonString(const std::string& json, const std::string& key,
                       const std::string& fallback = "") {
    std::string needle = "\"" + key + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return fallback;
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return fallback;
    pos = json.find('"', pos + 1);
    if (pos == std::string::npos) return fallback;
    auto end = json.find('"', pos + 1);
    if (end == std::string::npos) return fallback;
    return json.substr(pos + 1, end - pos - 1);
}

/// Read a numeric JSON value for a given key.
double jsonNumber(const std::string& json, const std::string& key,
                  double fallback = 0.0) {
    std::string needle = "\"" + key + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return fallback;
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return fallback;
    ++pos;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
    char* end = nullptr;
    double val = std::strtod(json.c_str() + pos, &end);
    if (end == json.c_str() + pos) return fallback;
    return val;
}

/// Extract a JSON sub-object for a given key (returns the raw substring).
std::string jsonObject(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return "{}";
    pos = json.find('{', pos + needle.size());
    if (pos == std::string::npos) return "{}";
    int depth = 0;
    size_t start = pos;
    for (size_t i = pos; i < json.size(); ++i) {
        if (json[i] == '{') ++depth;
        else if (json[i] == '}') { --depth; if (depth == 0) return json.substr(start, i - start + 1); }
    }
    return "{}";
}

/// Map a string to AnimationType.
visioncast::AnimationType parseAnimationType(const std::string& s) {
    if (s == "fade_in")     return visioncast::AnimationType::FadeIn;
    if (s == "fade_out")    return visioncast::AnimationType::FadeOut;
    if (s == "slide_left")  return visioncast::AnimationType::SlideLeft;
    if (s == "slide_right") return visioncast::AnimationType::SlideRight;
    if (s == "slide_up")    return visioncast::AnimationType::SlideUp;
    if (s == "slide_down")  return visioncast::AnimationType::SlideDown;
    if (s == "scale_in")    return visioncast::AnimationType::ScaleIn;
    if (s == "scale_out")   return visioncast::AnimationType::ScaleOut;
    return visioncast::AnimationType::None;
}

/// Map a string to TransitionType.
visioncast::TransitionType parseTransitionType(const std::string& s) {
    if (s == "cut")        return visioncast::TransitionType::Cut;
    if (s == "fade")       return visioncast::TransitionType::Fade;
    if (s == "wipe_left")  return visioncast::TransitionType::WipeLeft;
    if (s == "wipe_right") return visioncast::TransitionType::WipeRight;
    if (s == "wipe_up")    return visioncast::TransitionType::WipeUp;
    if (s == "wipe_down")  return visioncast::TransitionType::WipeDown;
    if (s == "dissolve")   return visioncast::TransitionType::Dissolve;
    return visioncast::TransitionType::None;
}

/// Map a string to EasingType.
visioncast::EasingType parseEasingType(const std::string& s) {
    if (s == "linear")      return visioncast::EasingType::Linear;
    if (s == "ease_in")     return visioncast::EasingType::EaseIn;
    if (s == "ease_out")    return visioncast::EasingType::EaseOut;
    if (s == "ease_in_out") return visioncast::EasingType::EaseInOut;
    return visioncast::EasingType::EaseInOut;
}

/// Evaluate an easing function at parameter t ∈ [0,1].
double ease(visioncast::EasingType type, double t) {
    t = std::max(0.0, std::min(1.0, t));
    switch (type) {
        case visioncast::EasingType::Linear:    return t;
        case visioncast::EasingType::EaseIn:    return t * t;
        case visioncast::EasingType::EaseOut:   return t * (2.0 - t);
        case visioncast::EasingType::EaseInOut:
            return (t < 0.5) ? 2.0 * t * t : -1.0 + (4.0 - 2.0 * t) * t;
    }
    return t;
}

} // anonymous namespace

namespace visioncast {

// =====================================================================
// Impl
// =====================================================================

struct GpuCompositor::Impl {
    bool ready = false;
    int outputWidth  = 1920;
    int outputHeight = 1080;

    // ---- Layer stack ------------------------------------------------
    std::vector<CompositingLayer> layers;

    // ---- Templates --------------------------------------------------
    std::vector<DynamicTemplate> templates;
    int activeTemplateIdx = -1;

    // ---- Transition state -------------------------------------------
    TransitionDesc currentTransition;
    double transitionElapsedMs = 0.0;
    bool   transitioning       = false;

    // ---- Lower-third state ------------------------------------------
    bool   lowerThirdActive    = false;
    double lowerThirdElapsedMs = 0.0;
    double lowerThirdDuration  = 5000.0;
    LowerThirdDesc lowerThirdDesc;
    CompositingLayer lowerThirdLayer;

#ifdef HAS_OPENGL
    GLuint overlayShader = 0;
    GLuint overlayVao    = 0;
    GLuint overlayVbo    = 0;
    GLuint overlayEbo    = 0;
    std::vector<GLuint> layerTextures;  // parallel to layers vector
    GLuint lowerThirdTex = 0;
#endif

    // ---- Helpers ----------------------------------------------------

    CompositingLayer* findLayer(const std::string& id) {
        for (auto& l : layers) {
            if (l.id == id) return &l;
        }
        return nullptr;
    }

    void sortLayers() {
        std::sort(layers.begin(), layers.end(),
                  [](const CompositingLayer& a, const CompositingLayer& b) {
                      return a.zOrder < b.zOrder;
                  });
    }

    /// Build the lower-third RGBA bitmap into lowerThirdLayer.
    void buildLowerThirdBitmap(int frameW, int frameH) {
        const auto& lt = lowerThirdDesc;
        int bandH = static_cast<int>(frameH * lt.bandHeightRatio);
        int bandY = frameH - bandH;

        // Create BGRA image (transparent).
        cv::Mat img(frameH, frameW, CV_8UC4, cv::Scalar(0, 0, 0, 0));

        // Draw semi-transparent background band.
        cv::Mat band = img(cv::Rect(0, bandY, frameW, bandH));
        band.setTo(cv::Scalar(lt.bgColor[0], lt.bgColor[1],
                               lt.bgColor[2], lt.bgColor[3]));

        // Draw title text.
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        cv::putText(img, lt.title,
                    cv::Point(40, bandY + static_cast<int>(bandH * 0.45)),
                    fontFace, lt.fontScale * 1.2,
                    cv::Scalar(lt.textColor[0], lt.textColor[1],
                               lt.textColor[2], lt.textColor[3]),
                    lt.thickness, cv::LINE_AA);

        // Draw subtitle text.
        cv::putText(img, lt.subtitle,
                    cv::Point(40, bandY + static_cast<int>(bandH * 0.80)),
                    fontFace, lt.fontScale * 0.9,
                    cv::Scalar(lt.subtitleColor[0], lt.subtitleColor[1],
                               lt.subtitleColor[2], lt.subtitleColor[3]),
                    lt.thickness - 1, cv::LINE_AA);

        // Store in lowerThirdLayer.
        lowerThirdLayer.id = "__lower_third__";
        lowerThirdLayer.texWidth  = frameW;
        lowerThirdLayer.texHeight = frameH;
        lowerThirdLayer.channels  = 4;
        size_t bytes = static_cast<size_t>(img.total() * img.elemSize());
        lowerThirdLayer.pixels.resize(bytes);
        std::memcpy(lowerThirdLayer.pixels.data(), img.data, bytes);
        lowerThirdLayer.posX = 0.0f;
        lowerThirdLayer.posY = 0.0f;
        lowerThirdLayer.width  = 1.0f;
        lowerThirdLayer.height = 1.0f;
        lowerThirdLayer.visible = true;
        lowerThirdLayer.opacity = 1.0f;
    }

    /// Alpha-blend a BGRA overlay onto a BGRA frame (CPU path).
    /// Uses OpenCV split/merge for vectorised blending on the visible ROI.
    static void alphaBlend(cv::Mat& dst, const cv::Mat& overlay,
                           int x, int y, float opacity) {
        // Compute the visible ROI (clipped to destination bounds).
        int srcX = std::max(0, -x);
        int srcY = std::max(0, -y);
        int dstX = std::max(0, x);
        int dstY = std::max(0, y);
        int roiW = std::min(overlay.cols - srcX, dst.cols - dstX);
        int roiH = std::min(overlay.rows - srcY, dst.rows - dstY);
        if (roiW <= 0 || roiH <= 0) return;

        cv::Mat srcROI = overlay(cv::Rect(srcX, srcY, roiW, roiH));
        cv::Mat dstROI = dst(cv::Rect(dstX, dstY, roiW, roiH));

        // Split into channels.
        cv::Mat srcCh[4], dstCh[4];
        cv::split(srcROI, srcCh);
        cv::split(dstROI, dstCh);

        // Build normalised alpha mask [0,1].
        cv::Mat alpha;
        srcCh[3].convertTo(alpha, CV_32F, opacity / 255.0);
        cv::Mat invAlpha = cv::Scalar(1.0) - alpha;

        // Blend each colour channel: out = src*a + dst*(1-a).
        for (int c = 0; c < 3; ++c) {
            cv::Mat s, d;
            srcCh[c].convertTo(s, CV_32F);
            dstCh[c].convertTo(d, CV_32F);
            cv::Mat blended = s.mul(alpha) + d.mul(invAlpha);
            blended.convertTo(dstCh[c], CV_8U);
        }
        dstCh[3].setTo(255);

        cv::merge(dstCh, 4, dstROI);
    }

    /// Apply animation to a layer and return effective opacity + position.
    void applyAnimation(CompositingLayer& layer, float& outOpacity,
                        float& outPosX, float& outPosY,
                        float& outW, float& outH) const {
        outOpacity = layer.opacity;
        outPosX = layer.posX;
        outPosY = layer.posY;
        outW = layer.width;
        outH = layer.height;

        if (!layer.animActive) return;

        double t = 0.0;
        if (layer.animation.durationMs > 0.0) {
            double elapsed = std::max(0.0, layer.animElapsedMs - layer.animation.delayMs);
            t = std::min(1.0, elapsed / layer.animation.durationMs);
        } else {
            t = 1.0;
        }
        double e = ease(layer.animation.easing, t);

        switch (layer.animation.type) {
            case AnimationType::FadeIn:
                outOpacity = static_cast<float>(e) * layer.opacity;
                break;
            case AnimationType::FadeOut:
                outOpacity = static_cast<float>(1.0 - e) * layer.opacity;
                break;
            case AnimationType::SlideLeft:
                outPosX = static_cast<float>(1.0 - e) * 1.0f + e * layer.posX;
                break;
            case AnimationType::SlideRight:
                outPosX = static_cast<float>(1.0 - e) * (-layer.width) + e * layer.posX;
                break;
            case AnimationType::SlideUp:
                outPosY = static_cast<float>(1.0 - e) * 1.0f + e * layer.posY;
                break;
            case AnimationType::SlideDown:
                outPosY = static_cast<float>(1.0 - e) * (-layer.height) + e * layer.posY;
                break;
            case AnimationType::ScaleIn: {
                float s = static_cast<float>(e);
                outW = layer.width * s;
                outH = layer.height * s;
                outPosX = layer.posX + (layer.width - outW) * 0.5f;
                outPosY = layer.posY + (layer.height - outH) * 0.5f;
                break;
            }
            case AnimationType::ScaleOut: {
                float s = static_cast<float>(1.0 - e);
                outW = layer.width * s;
                outH = layer.height * s;
                outPosX = layer.posX + (layer.width - outW) * 0.5f;
                outPosY = layer.posY + (layer.height - outH) * 0.5f;
                break;
            }
            default:
                break;
        }
    }
};

// =====================================================================
// Construction / Destruction
// =====================================================================

GpuCompositor::GpuCompositor()
    : impl_(std::make_unique<Impl>()) {}

GpuCompositor::~GpuCompositor() {
    shutdown();
}

// =====================================================================
// Lifecycle
// =====================================================================

bool GpuCompositor::initialize(int width, int height) {
    if (width <= 0 || height <= 0) return false;
    impl_->outputWidth  = width;
    impl_->outputHeight = height;

#ifdef HAS_OPENGL
    // ----- Overlay shader (simple textured quad with uniform opacity) -----
    static const char* kOverlayVS = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    uniform vec4 uRect;   // x, y, w, h in NDC
    out vec2 TexCoord;
    void main() {
        vec2 p = uRect.xy + aPos * uRect.zw;
        gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
    )glsl";

    static const char* kOverlayFS = R"glsl(
    #version 330 core
    in  vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D uTexture;
    uniform float     uOpacity;
    void main() {
        vec4 c = texture(uTexture, TexCoord);
        FragColor = vec4(c.rgb, c.a * uOpacity);
    }
    )glsl";

    auto compile = [](GLenum type, const char* src) -> GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512];
            glGetShaderInfoLog(s, sizeof(log), nullptr, log);
            std::cerr << "[GpuCompositor] Shader error: " << log << std::endl;
            glDeleteShader(s);
            return 0;
        }
        return s;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, kOverlayVS);
    GLuint fs = compile(GL_FRAGMENT_SHADER, kOverlayFS);
    if (vs && fs) {
        impl_->overlayShader = glCreateProgram();
        glAttachShader(impl_->overlayShader, vs);
        glAttachShader(impl_->overlayShader, fs);
        glLinkProgram(impl_->overlayShader);
    }
    if (vs) glDeleteShader(vs);
    if (fs) glDeleteShader(fs);

    // Unit quad [0,1]
    const float verts[] = {
        0.f, 1.f,  0.f, 0.f,
        1.f, 1.f,  1.f, 0.f,
        1.f, 0.f,  1.f, 1.f,
        0.f, 0.f,  0.f, 1.f,
    };
    const unsigned int idx[] = { 0, 1, 2,  2, 3, 0 };

    glGenVertexArrays(1, &impl_->overlayVao);
    glGenBuffers(1, &impl_->overlayVbo);
    glGenBuffers(1, &impl_->overlayEbo);
    glBindVertexArray(impl_->overlayVao);
    glBindBuffer(GL_ARRAY_BUFFER, impl_->overlayVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, impl_->overlayEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
#endif

    impl_->ready = true;
    std::cout << "[GpuCompositor] Initialized (" << width << "x" << height << ")."
              << std::endl;
    return true;
}

void GpuCompositor::shutdown() {
    if (!impl_->ready) return;

#ifdef HAS_OPENGL
    for (GLuint t : impl_->layerTextures) {
        if (t) glDeleteTextures(1, &t);
    }
    impl_->layerTextures.clear();
    if (impl_->lowerThirdTex) {
        glDeleteTextures(1, &impl_->lowerThirdTex);
        impl_->lowerThirdTex = 0;
    }
    if (impl_->overlayVao) { glDeleteVertexArrays(1, &impl_->overlayVao); impl_->overlayVao = 0; }
    if (impl_->overlayVbo) { glDeleteBuffers(1, &impl_->overlayVbo); impl_->overlayVbo = 0; }
    if (impl_->overlayEbo) { glDeleteBuffers(1, &impl_->overlayEbo); impl_->overlayEbo = 0; }
    if (impl_->overlayShader) { glDeleteProgram(impl_->overlayShader); impl_->overlayShader = 0; }
#endif

    impl_->layers.clear();
    impl_->templates.clear();
    impl_->activeTemplateIdx = -1;
    impl_->ready = false;
    std::cout << "[GpuCompositor] Shutdown." << std::endl;
}

bool GpuCompositor::isReady() const {
    return impl_->ready;
}

// =====================================================================
// Layer Management
// =====================================================================

bool GpuCompositor::addLayer(const std::string& id,
                              const uint8_t* bgraData, int texW, int texH,
                              float posX, float posY, float w, float h,
                              int zOrder) {
    if (!impl_->ready || !bgraData || texW <= 0 || texH <= 0) return false;
    if (impl_->findLayer(id)) return false; // duplicate

    CompositingLayer layer;
    layer.id = id;
    layer.zOrder = zOrder;
    layer.posX = posX;
    layer.posY = posY;
    layer.width  = w;
    layer.height = h;
    layer.texWidth  = texW;
    layer.texHeight = texH;
    layer.channels  = 4;

    size_t bytes = static_cast<size_t>(texW) * texH * 4;
    layer.pixels.resize(bytes);
    std::memcpy(layer.pixels.data(), bgraData, bytes);

#ifdef HAS_OPENGL
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texW, texH, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, bgraData);
    impl_->layerTextures.push_back(tex);
#endif

    impl_->layers.push_back(std::move(layer));
    impl_->sortLayers();
    return true;
}

bool GpuCompositor::addPngLayer(const std::string& id,
                                 const std::string& pngPath,
                                 float posX, float posY, float w, float h,
                                 int zOrder) {
    cv::Mat img = cv::imread(pngPath, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        std::cerr << "[GpuCompositor] Failed to load PNG: " << pngPath << std::endl;
        return false;
    }

    // Ensure 4-channel BGRA.
    if (img.channels() == 3) {
        cv::cvtColor(img, img, cv::COLOR_BGR2BGRA);
    } else if (img.channels() == 1) {
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGRA);
    }

    return addLayer(id, img.data, img.cols, img.rows, posX, posY, w, h, zOrder);
}

void GpuCompositor::removeLayer(const std::string& id) {
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (impl_->layers[i].id == id) {
#ifdef HAS_OPENGL
            if (i < impl_->layerTextures.size() && impl_->layerTextures[i]) {
                glDeleteTextures(1, &impl_->layerTextures[i]);
                impl_->layerTextures.erase(impl_->layerTextures.begin() +
                                           static_cast<ptrdiff_t>(i));
            }
#endif
            impl_->layers.erase(impl_->layers.begin() + static_cast<ptrdiff_t>(i));
            return;
        }
    }
}

bool GpuCompositor::updateLayerPixels(const std::string& id,
                                       const uint8_t* bgraData,
                                       int texW, int texH) {
    if (!bgraData || texW <= 0 || texH <= 0) return false;
    auto* layer = impl_->findLayer(id);
    if (!layer) return false;

    size_t bytes = static_cast<size_t>(texW) * texH * 4;
    layer->pixels.resize(bytes);
    std::memcpy(layer->pixels.data(), bgraData, bytes);
    layer->texWidth  = texW;
    layer->texHeight = texH;

#ifdef HAS_OPENGL
    // Find the corresponding GL texture.
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (impl_->layers[i].id == id && i < impl_->layerTextures.size()) {
            glBindTexture(GL_TEXTURE_2D, impl_->layerTextures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texW, texH, 0,
                         GL_BGRA, GL_UNSIGNED_BYTE, bgraData);
            break;
        }
    }
#endif
    return true;
}

void GpuCompositor::setLayerVisible(const std::string& id, bool visible) {
    if (auto* l = impl_->findLayer(id)) l->visible = visible;
}

void GpuCompositor::setLayerOpacity(const std::string& id, float opacity) {
    if (auto* l = impl_->findLayer(id))
        l->opacity = std::max(0.0f, std::min(1.0f, opacity));
}

// =====================================================================
// Animations
// =====================================================================

void GpuCompositor::animateLayer(const std::string& id,
                                  const AnimationDesc& anim) {
    auto* l = impl_->findLayer(id);
    if (!l) return;
    l->animation     = anim;
    l->animElapsedMs = 0.0;
    l->animActive    = (anim.type != AnimationType::None);
}

// =====================================================================
// Transitions
// =====================================================================

void GpuCompositor::startTransition(const TransitionDesc& transition) {
    impl_->currentTransition    = transition;
    impl_->transitionElapsedMs  = 0.0;
    impl_->transitioning        = (transition.type != TransitionType::None);
}

bool GpuCompositor::isTransitioning() const {
    return impl_->transitioning;
}

// =====================================================================
// Lower Thirds
// =====================================================================

void GpuCompositor::showLowerThird(const std::string& title,
                                    const std::string& subtitle,
                                    double durationMs,
                                    const LowerThirdDesc* desc) {
    LowerThirdDesc lt;
    if (desc) {
        lt = *desc;
    } else if (impl_->activeTemplateIdx >= 0) {
        lt = impl_->templates[static_cast<size_t>(impl_->activeTemplateIdx)].lowerThird;
    }
    lt.title    = title;
    lt.subtitle = subtitle;

    // Apply template entry animation if none explicitly set.
    if (lt.entryAnim.type == AnimationType::None &&
        impl_->activeTemplateIdx >= 0) {
        lt.entryAnim = impl_->templates[static_cast<size_t>(impl_->activeTemplateIdx)].entryAnimation;
    }
    if (lt.exitAnim.type == AnimationType::None &&
        impl_->activeTemplateIdx >= 0) {
        lt.exitAnim = impl_->templates[static_cast<size_t>(impl_->activeTemplateIdx)].exitAnimation;
    }

    impl_->lowerThirdDesc     = lt;
    impl_->lowerThirdDuration = (lt.displayDurationMs > 0.0) ? lt.displayDurationMs : durationMs;
    impl_->lowerThirdElapsedMs = 0.0;
    impl_->lowerThirdActive   = true;

    impl_->buildLowerThirdBitmap(impl_->outputWidth, impl_->outputHeight);

    // Set entry animation on the layer.
    impl_->lowerThirdLayer.animation     = lt.entryAnim;
    impl_->lowerThirdLayer.animElapsedMs = 0.0;
    impl_->lowerThirdLayer.animActive    = (lt.entryAnim.type != AnimationType::None);

#ifdef HAS_OPENGL
    if (!impl_->lowerThirdTex) {
        glGenTextures(1, &impl_->lowerThirdTex);
    }
    glBindTexture(GL_TEXTURE_2D, impl_->lowerThirdTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 impl_->lowerThirdLayer.texWidth,
                 impl_->lowerThirdLayer.texHeight, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE,
                 impl_->lowerThirdLayer.pixels.data());
#endif

    std::cout << "[GpuCompositor] Lower third shown: " << title << std::endl;
}

void GpuCompositor::hideLowerThird() {
    impl_->lowerThirdActive = false;
    impl_->lowerThirdLayer.visible = false;
    std::cout << "[GpuCompositor] Lower third hidden." << std::endl;
}

// =====================================================================
// Dynamic Templates
// =====================================================================

bool GpuCompositor::loadTemplate(const std::string& jsonPath) {
    std::ifstream ifs(jsonPath);
    if (!ifs.is_open()) {
        std::cerr << "[GpuCompositor] Cannot open template: " << jsonPath
                  << std::endl;
        return false;
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    std::string json = ss.str();

    DynamicTemplate tmpl;
    tmpl.name        = jsonString(json, "name", "unnamed");
    tmpl.description = jsonString(json, "description");

    // --- Style sub-object ---
    std::string style = jsonObject(json, "style");
    tmpl.lowerThird.backgroundOpacity = static_cast<float>(
        jsonNumber(style, "background_opacity", 0.6));
    tmpl.lowerThird.fontScale = static_cast<float>(
        jsonNumber(style, "font_scale", 1.0));
    tmpl.lowerThird.thickness = static_cast<int>(
        jsonNumber(style, "thickness", 2));
    tmpl.lowerThird.bandHeightRatio = static_cast<float>(
        jsonNumber(style, "band_height_ratio", 0.18));

    // Parse entry animation from style.
    std::string animStr = jsonString(style, "animation", "none");
    double animDuration  = jsonNumber(style, "animation_duration_ms", 500.0);
    std::string easingStr = jsonString(style, "easing", "ease_in_out");
    tmpl.entryAnimation.type       = parseAnimationType(animStr);
    tmpl.entryAnimation.durationMs = animDuration;
    tmpl.entryAnimation.easing     = parseEasingType(easingStr);
    tmpl.lowerThird.entryAnim      = tmpl.entryAnimation;

    // Parse exit animation.
    std::string exitStr = jsonString(style, "exit_animation", "fade_out");
    double exitDuration = jsonNumber(style, "exit_animation_duration_ms", animDuration);
    tmpl.exitAnimation.type       = parseAnimationType(exitStr);
    tmpl.exitAnimation.durationMs = exitDuration;
    tmpl.exitAnimation.easing     = parseEasingType(easingStr);
    tmpl.lowerThird.exitAnim      = tmpl.exitAnimation;

    // Parse configurable lower-third display duration.
    tmpl.lowerThird.displayDurationMs = jsonNumber(style, "display_duration_ms", 0.0);

    // --- Transition sub-object ---
    std::string transObj = jsonObject(json, "transition");
    tmpl.transition.type       = parseTransitionType(jsonString(transObj, "type", "none"));
    tmpl.transition.durationMs = jsonNumber(transObj, "duration_ms", 500.0);
    tmpl.transition.easing     = parseEasingType(jsonString(transObj, "easing", "ease_in_out"));

    // --- Logo path ---
    std::string layout = jsonObject(json, "layout");
    tmpl.logoPath = jsonString(layout, "logo_path");

    // Avoid duplicates — overwrite if name matches.
    for (auto& t : impl_->templates) {
        if (t.name == tmpl.name) {
            t = tmpl;
            std::cout << "[GpuCompositor] Template updated: " << tmpl.name
                      << std::endl;
            return true;
        }
    }

    impl_->templates.push_back(std::move(tmpl));
    std::cout << "[GpuCompositor] Template loaded: " << tmpl.name << std::endl;
    return true;
}

bool GpuCompositor::activateTemplate(const std::string& name) {
    for (size_t i = 0; i < impl_->templates.size(); ++i) {
        if (impl_->templates[i].name == name) {
            impl_->activeTemplateIdx = static_cast<int>(i);
            std::cout << "[GpuCompositor] Active template: " << name << std::endl;
            return true;
        }
    }
    return false;
}

const DynamicTemplate* GpuCompositor::activeTemplate() const {
    if (impl_->activeTemplateIdx < 0 ||
        static_cast<size_t>(impl_->activeTemplateIdx) >= impl_->templates.size())
        return nullptr;
    return &impl_->templates[static_cast<size_t>(impl_->activeTemplateIdx)];
}

std::vector<std::string> GpuCompositor::templateNames() const {
    std::vector<std::string> names;
    names.reserve(impl_->templates.size());
    for (const auto& t : impl_->templates) names.push_back(t.name);
    return names;
}

// =====================================================================
// Per-Frame Update
// =====================================================================

void GpuCompositor::update(double deltaMs) {
    if (!impl_->ready) return;

    // Advance layer animations.
    for (auto& layer : impl_->layers) {
        if (layer.animActive) {
            layer.animElapsedMs += deltaMs;
            double total = layer.animation.delayMs + layer.animation.durationMs;
            if (layer.animElapsedMs >= total) {
                layer.animActive = false;
            }
        }
    }

    // Advance lower-third timer.
    if (impl_->lowerThirdActive) {
        impl_->lowerThirdElapsedMs += deltaMs;

        // Handle entry animation.
        if (impl_->lowerThirdLayer.animActive) {
            impl_->lowerThirdLayer.animElapsedMs += deltaMs;
            double total = impl_->lowerThirdLayer.animation.delayMs +
                           impl_->lowerThirdLayer.animation.durationMs;
            if (impl_->lowerThirdLayer.animElapsedMs >= total) {
                impl_->lowerThirdLayer.animActive = false;
            }
        }

        // Trigger exit animation when approaching the end.
        double exitStart = impl_->lowerThirdDuration -
                           impl_->lowerThirdDesc.exitAnim.durationMs;
        if (impl_->lowerThirdElapsedMs >= exitStart &&
            !impl_->lowerThirdLayer.animActive &&
            impl_->lowerThirdDesc.exitAnim.type != AnimationType::None &&
            impl_->lowerThirdElapsedMs < impl_->lowerThirdDuration) {
            impl_->lowerThirdLayer.animation     = impl_->lowerThirdDesc.exitAnim;
            impl_->lowerThirdLayer.animElapsedMs = 0.0;
            impl_->lowerThirdLayer.animActive    = true;
        }

        // Auto-hide when duration expires.
        if (impl_->lowerThirdElapsedMs >= impl_->lowerThirdDuration) {
            hideLowerThird();
        }
    }

    // Advance transition.
    if (impl_->transitioning) {
        impl_->transitionElapsedMs += deltaMs;
        if (impl_->transitionElapsedMs >= impl_->currentTransition.durationMs) {
            impl_->transitioning = false;
        }
    }
}

// =====================================================================
// CPU Compositing
// =====================================================================

void GpuCompositor::composite(VideoFrame& frame) {
    if (!impl_->ready || !frame.data) return;

    // Wrap frame in a cv::Mat (no copy).
    int ch = (frame.format == PixelFormat::BGRA8) ? 4 : 3;
    int cvType = (ch == 4) ? CV_8UC4 : CV_8UC3;
    cv::Mat dst(frame.height, frame.width, cvType, frame.data, frame.stride);

    // Ensure BGRA for alpha blending.
    bool converted = false;
    if (ch == 3) {
        cv::Mat bgra;
        cv::cvtColor(dst, bgra, cv::COLOR_BGR2BGRA);
        dst = bgra;
        converted = true;
    }

    // Apply global transition opacity.
    float globalOpacity = 1.0f;
    if (impl_->transitioning && impl_->currentTransition.durationMs > 0.0) {
        double t = impl_->transitionElapsedMs / impl_->currentTransition.durationMs;
        double e = ease(impl_->currentTransition.easing, t);
        if (impl_->currentTransition.type == TransitionType::Fade ||
            impl_->currentTransition.type == TransitionType::Dissolve) {
            globalOpacity = static_cast<float>(e);
        }
    }

    // Render each layer.
    for (auto& layer : impl_->layers) {
        if (!layer.visible || layer.pixels.empty()) continue;

        float opacity, px, py, w, h;
        impl_->applyAnimation(layer, opacity, px, py, w, h);
        opacity *= globalOpacity;
        if (opacity <= 0.0f) continue;

        int dstX = static_cast<int>(px * dst.cols);
        int dstY = static_cast<int>(py * dst.rows);
        int dstW = static_cast<int>(w * dst.cols);
        int dstH = static_cast<int>(h * dst.rows);
        if (dstW <= 0 || dstH <= 0) continue;

        cv::Mat overlay(layer.texHeight, layer.texWidth, CV_8UC4,
                        layer.pixels.data());
        if (dstW != layer.texWidth || dstH != layer.texHeight) {
            cv::resize(overlay, overlay, cv::Size(dstW, dstH), 0, 0,
                       cv::INTER_LINEAR);
        }

        Impl::alphaBlend(dst, overlay, dstX, dstY, opacity);
    }

    // Render lower third.
    if (impl_->lowerThirdActive && impl_->lowerThirdLayer.visible &&
        !impl_->lowerThirdLayer.pixels.empty()) {
        float opacity, px, py, w, h;
        impl_->applyAnimation(impl_->lowerThirdLayer, opacity, px, py, w, h);
        opacity *= globalOpacity;
        if (opacity > 0.0f) {
            cv::Mat ltMat(impl_->lowerThirdLayer.texHeight,
                          impl_->lowerThirdLayer.texWidth, CV_8UC4,
                          impl_->lowerThirdLayer.pixels.data());
            Impl::alphaBlend(dst, ltMat,
                             static_cast<int>(px * dst.cols),
                             static_cast<int>(py * dst.rows), opacity);
        }
    }

    // Write back if we created a temporary BGRA mat.
    if (converted) {
        cv::cvtColor(dst, dst, cv::COLOR_BGRA2BGR);
        std::memcpy(frame.data, dst.data,
                    static_cast<size_t>(frame.height * frame.stride));
    }
}

// =====================================================================
// GPU Compositing (OpenGL)
// =====================================================================

#ifdef HAS_OPENGL
void GpuCompositor::compositeGpu(unsigned int baseTextureId,
                                  int frameW, int frameH) {
    if (!impl_->ready || !impl_->overlayShader) return;
    (void)baseTextureId; // base already rendered by VisionEngine.

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(impl_->overlayShader);
    glBindVertexArray(impl_->overlayVao);

    // Apply global transition opacity.
    float globalOpacity = 1.0f;
    if (impl_->transitioning && impl_->currentTransition.durationMs > 0.0) {
        double t = impl_->transitionElapsedMs / impl_->currentTransition.durationMs;
        double e = ease(impl_->currentTransition.easing, t);
        if (impl_->currentTransition.type == TransitionType::Fade ||
            impl_->currentTransition.type == TransitionType::Dissolve) {
            globalOpacity = static_cast<float>(e);
        }
    }

    auto drawLayer = [&](CompositingLayer& layer, GLuint tex) {
        if (!layer.visible || tex == 0) return;

        float opacity, px, py, w, h;
        impl_->applyAnimation(layer, opacity, px, py, w, h);
        opacity *= globalOpacity;
        if (opacity <= 0.0f) return;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(impl_->overlayShader, "uTexture"), 0);
        glUniform1f(glGetUniformLocation(impl_->overlayShader, "uOpacity"), opacity);
        glUniform4f(glGetUniformLocation(impl_->overlayShader, "uRect"),
                    px, py, w, h);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    };

    // Draw layers in z-order.
    for (size_t i = 0; i < impl_->layers.size(); ++i) {
        if (i < impl_->layerTextures.size()) {
            drawLayer(impl_->layers[i], impl_->layerTextures[i]);
        }
    }

    // Draw lower third.
    if (impl_->lowerThirdActive && impl_->lowerThirdTex) {
        drawLayer(impl_->lowerThirdLayer, impl_->lowerThirdTex);
    }

    glBindVertexArray(0);
    glDisable(GL_BLEND);

    (void)frameW;
    (void)frameH;
}
#endif

} // namespace visioncast

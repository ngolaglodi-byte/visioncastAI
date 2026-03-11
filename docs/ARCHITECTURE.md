# VisionCast-AI — Architecture Document

## 1. System Overview

VisionCast-AI is a professional 4K broadcast production system powered by AI. It combines real-time facial recognition, automatic overlay generation, a high-performance C++ video engine, and a Qt-based control room — all integrated with professional video I/O hardware.

### Core Capabilities

| Capability | Description |
|---|---|
| **AI Face Recognition** | Real-time detection and identification of on-screen talents |
| **4K Video Engine** | GPU-accelerated compositing, overlays, filters, color correction |
| **Broadcast Control Room** | Qt-based UI for live mixing, monitoring, and configuration |
| **Professional I/O** | DeckLink, AJA, Magewell capture/playout + NDI, SRT, RTMP streaming |

### Architecture Diagram (PDF-ready)

A full-colour, professional architecture diagram is available:

- **SVG (vector / PDF-ready):** [`docs/architecture_diagram.svg`](architecture_diagram.svg)
- **Source (Graphviz DOT):** [`docs/architecture_diagram.dot`](architecture_diagram.dot)

To regenerate the diagram (requires Graphviz):

```bash
python docs/generate_diagram.py          # SVG + PDF
python docs/generate_diagram.py --svg    # SVG only
```

### High-Level Architecture

```
┌───────────────────────────────────────────────────────────────────────┐
│                         VisionCast-AI System                         │
├───────────────┬───────────────────────────────┬───────────────────────┤
│  Python AI    │      C++ Video Engine         │   Qt Control Room    │
│  Module       │                               │                      │
│ ┌───────────┐ │ ┌──────────┐  ┌────────────┐  │ ┌──────────────────┐ │
│ │  Face     │ │ │ Capture  │→ │ Pipeline   │  │ │  Preview Panel   │ │
│ │  Detect   │ │ │ Manager  │  │ Manager    │  │ │  + Multiview     │ │
│ │  + Match  │ │ └──────────┘  └────────────┘  │ └──────────────────┘ │
│ └─────┬─────┘ │       │            │          │ ┌──────────────────┐ │
│       │       │ ┌──────────┐  ┌────────────┐  │ │  Overlay Editor  │ │
│  ┌────▼────┐  │ │ Overlay  │← │ Filter     │  │ │  + Talent DB     │ │
│  │Metadata │──┼→│ Renderer │  │ Chain      │  │ └──────────────────┘ │
│  │  IPC    │  │ └──────────┘  └────────────┘  │ ┌──────────────────┐ │
│  └─────────┘  │       │            │          │ │  Output Config   │ │
│               │ ┌──────────┐  ┌────────────┐  │ │  + SDK Control   │ │
│               │ │ Output   │→ │ Broadcast  │  │ └──────────────────┘ │
│               │ │ Manager  │  │ Encoder    │  │                      │
│               │ └──────────┘  └────────────┘  │                      │
├───────────────┴───────────────────────────────┴───────────────────────┤
│                     SDK Abstraction Layer                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐             │
│  │ DeckLink │  │   AJA    │  │ Magewell │  │   NDI    │             │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘             │
│  ┌──────────┐  ┌──────────┐                                         │
│  │   SRT    │  │   RTMP   │                                         │
│  └──────────┘  └──────────┘                                         │
└───────────────────────────────────────────────────────────────────────┘
```

---

## 2. Directory Structure

```
visioncast-ai/
│
├── python/                        # Python AI Module
│   ├── main.py                    # Entry point (face recognition pipeline)
│   ├── requirements.txt           # Python dependencies
│   ├── README.md                  # Module documentation
│   ├── ai/                        # AI sub-module
│   │   ├── __init__.py
│   │   ├── face_detector.py       # Face detection engine
│   │   ├── face_matcher.py        # Face matching against talent DB
│   │   └── talent_db.py           # Talent database loader
│   └── ipc/                       # Inter-Process Communication
│       ├── __init__.py
│       ├── metadata_sender.py     # ZeroMQ metadata publisher
│       └── protocol.py            # Message schema definitions
│
├── engine/                        # C++ Video Engine
│   ├── CMakeLists.txt             # Top-level CMake build
│   ├── README.md                  # Module documentation
│   ├── include/                   # Public headers
│   │   └── visioncast/
│   │       ├── capture_manager.h  # Video capture abstraction
│   │       ├── pipeline_manager.h # Video processing pipeline
│   │       ├── overlay_renderer.h # Lower-third / overlay compositing
│   │       ├── filter_chain.h     # Video filter pipeline
│   │       ├── output_manager.h   # Broadcast output routing
│   │       ├── metadata_receiver.h # IPC metadata from Python
│   │       ├── vision_engine.h    # GPU vision engine (capture + preview + render)
│   │       ├── ffmpeg_rtmp.h      # NEW: FFmpeg RTMP streaming output
│   │       └── multi_ffmpeg_rtmp_manager.h # NEW: Multi-platform RTMP manager
│   └── src/                       # Implementation
│       ├── main.cpp               # Engine entry point
│       ├── capture_manager.cpp
│       ├── pipeline_manager.cpp
│       ├── overlay_renderer.cpp
│       ├── filter_chain.cpp
│       ├── output_manager.cpp
│       ├── metadata_receiver.cpp
│       ├── vision_engine.cpp      # VisionEngine implementation
│       ├── ffmpeg_rtmp.cpp        # NEW: FFmpeg RTMP implementation
│       └── multi_ffmpeg_rtmp_manager.cpp # NEW: Multi-RTMP manager implementation
│
├── ui/                            # Qt Control Room
│   ├── CMakeLists.txt             # Qt CMake build
│   ├── README.md                  # Module documentation
│   ├── include/
│   │   └── visioncast_ui/
│   │       ├── main_window.h      # Main application window
│   │       ├── preview_panel.h    # Video preview + multiview
│   │       ├── overlay_editor.h   # Overlay template editor
│   │       ├── talent_manager.h   # Talent database management
│   │       ├── output_config.h    # Output/encoder configuration
│   │       └── monitoring_panel.h # System health monitoring
│   └── src/
│       ├── main.cpp               # UI entry point
│       ├── main_window.cpp
│       ├── preview_panel.cpp
│       ├── overlay_editor.cpp
│       ├── talent_manager.cpp
│       ├── output_config.cpp
│       └── monitoring_panel.cpp
│
├── sdk/                           # Hardware SDK Abstraction Layer
│   ├── README.md                  # SDK integration guide
│   ├── CMakeLists.txt             # SDK build integration
│   ├── include/
│   │   └── visioncast_sdk/
│   │       ├── video_device.h     # Abstract base device interface
│   │       ├── decklink_device.h  # Blackmagic DeckLink wrapper
│   │       ├── decklink_input.h   # DeckLink capture (SDI/HDMI, 1080p/4K)
│   │       ├── decklink_output.h  # DeckLink playout (SDI/HDMI, 1080p/4K)
│   │       ├── aja_device.h       # AJA video device wrapper
│   │       ├── aja_input.h        # AJA capture
│   │       ├── aja_output.h       # AJA playout
│   │       ├── magewell_device.h  # Magewell capture wrapper
│   │       ├── magewell_input.h   # Magewell capture
│   │       ├── ndi_device.h       # NDI network device wrapper
│   │       ├── ndi_input.h        # NDI receive
│   │       ├── ndi_output.h       # NDI send
│   │       ├── srt_output.h       # SRT streaming output
│   │       └── rtmp_output.h      # RTMP streaming output (DEPRECATED - use engine/ffmpeg_rtmp.h)
│   └── src/
│       ├── video_device.cpp
│       ├── decklink_device.cpp
│       ├── decklink_input.cpp
│       ├── decklink_output.cpp
│       ├── aja_device.cpp
│       ├── aja_input.cpp
│       ├── aja_output.cpp
│       ├── magewell_device.cpp
│       ├── magewell_input.cpp
│       ├── ndi_device.cpp
│       ├── ndi_input.cpp
│       ├── ndi_output.cpp
│       ├── srt_output.cpp
│       └── rtmp_output.cpp        # (DEPRECATED - use engine/ffmpeg_rtmp.cpp)
│
├── overlays/                      # Overlay Templates & Assets
│   ├── README.md
│   └── templates/
│       └── default.json           # Default overlay template definition
│
├── talents/                       # Talent Database
│   ├── talents.json               # Talent metadata
│   └── glody.jpeg                 # Sample talent image
│
├── docs/                          # Documentation
│   ├── README.md
│   └── ARCHITECTURE.md            # This document
│
├── CMakeLists.txt                 # Top-level project CMake
├── README.md                      # Project readme
└── .gitignore                     # Ignore rules
```

---

## 3. Python AI Module (`python/`)

### 3.1 Purpose

The Python module handles all AI-related processing: face detection, recognition, talent matching, and metadata emission. It runs as an independent process that communicates with the C++ engine via ZeroMQ IPC.

### 3.2 Sub-Modules

#### `ai/face_detector.py`

Wraps the `face_recognition` library for real-time face detection.

```python
class FaceDetector:
    """Detects face bounding boxes in video frames."""

    def __init__(self, model: str = "hog", upsample: int = 1):
        self.model = model        # "hog" (CPU) or "cnn" (GPU)
        self.upsample = upsample

    def detect(self, frame: np.ndarray) -> list[FaceLocation]:
        """Returns list of face locations [(top, right, bottom, left), ...]."""
        ...

    def encode(self, frame: np.ndarray, locations: list) -> list[np.ndarray]:
        """Returns 128-d face encodings for detected faces."""
        ...
```

#### `ai/face_matcher.py`

Matches detected faces against the known talent database.

```python
class FaceMatcher:
    """Matches face encodings against a known talent database."""

    def __init__(self, talent_db: TalentDB, tolerance: float = 0.45):
        self.talent_db = talent_db
        self.tolerance = tolerance

    def match(self, encoding: np.ndarray) -> Optional[TalentInfo]:
        """Returns matched TalentInfo or None."""
        ...
```

#### `ai/talent_db.py`

Loads and manages the talent database from `talents/talents.json`.

```python
class TalentDB:
    """Loads talent faces and metadata from the JSON database."""

    def __init__(self, talents_path: str):
        self.talents: list[TalentInfo] = []
        self.encodings: list[np.ndarray] = []

    def load(self) -> None:
        """Loads talent images, computes face encodings."""
        ...

    def get_encoding(self, index: int) -> np.ndarray: ...
    def get_talent(self, index: int) -> TalentInfo: ...
```

#### `ipc/metadata_sender.py`

Sends recognized talent metadata to the C++ engine over ZeroMQ.

```python
class MetadataSender:
    """Publishes face recognition metadata to the C++ engine via ZeroMQ."""

    def __init__(self, endpoint: str = "tcp://127.0.0.1:5557"):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PUB)
        self.socket.bind(endpoint)

    def send(self, metadata: RecognitionResult) -> None:
        """Serializes and sends recognition result as JSON."""
        ...

    def close(self) -> None: ...
```

#### `ipc/protocol.py`

Defines the message schema for Python↔C++ communication.

```python
@dataclass
class FaceLocation:
    top: int
    right: int
    bottom: int
    left: int

@dataclass
class TalentInfo:
    id: str
    name: str
    role: str
    overlay_template: str

@dataclass
class RecognitionResult:
    timestamp_ms: int
    frame_id: int
    faces: list[RecognizedFace]

@dataclass
class RecognizedFace:
    location: FaceLocation
    talent: Optional[TalentInfo]
    confidence: float
```

### 3.3 Dependencies

| Package | Version | Purpose |
|---|---|---|
| `opencv-python` | ≥ 4.8.0 | Video capture and frame processing |
| `face_recognition` | ≥ 1.3.0 | Face detection and encoding (dlib) |
| `numpy` | ≥ 1.24.0 | Numerical operations |
| `Pillow` | ≥ 10.0.0 | Image processing utilities |
| `pyzmq` | ≥ 25.0.0 | ZeroMQ IPC for metadata publishing |

---

## 4. C++ Video Engine (`engine/`)

### 4.1 Purpose

The C++ engine is the real-time video processing core. It handles 4K frame capture, compositing, overlay rendering, video filtering, and broadcast output at professional frame rates (23.98/25/29.97/50/59.94 fps).

### 4.2 Classes

#### `CaptureManager`

Manages video input sources (cameras, SDI, NDI, file playback).

```cpp
class CaptureManager {
public:
    CaptureManager();
    ~CaptureManager();

    bool addSource(const std::string& name, std::shared_ptr<IVideoDevice> device);
    bool removeSource(const std::string& name);

    VideoFrame captureFrame(const std::string& sourceName);
    std::vector<std::string> listSources() const;

    void setActiveSource(const std::string& name);
    VideoFrame captureActiveFrame();

private:
    std::map<std::string, std::shared_ptr<IVideoDevice>> sources_;
    std::string activeSource_;
    std::mutex mutex_;
};
```

#### `PipelineManager`

Orchestrates the full frame processing pipeline.

```cpp
class PipelineManager {
public:
    PipelineManager(CaptureManager& capture,
                    FilterChain& filters,
                    OverlayRenderer& overlays,
                    OutputManager& output);

    void start();   // Begin pipeline processing loop
    void stop();    // Stop processing
    bool isRunning() const;

    void processFrame();  // Single-frame pipeline step

    // Pipeline configuration
    void setFrameRate(double fps);
    void setResolution(int width, int height);  // e.g. 3840x2160

private:
    CaptureManager& capture_;
    FilterChain& filters_;
    OverlayRenderer& overlays_;
    OutputManager& output_;

    std::atomic<bool> running_;
    std::thread pipelineThread_;
    double targetFps_ = 25.0;
    int width_ = 3840;
    int height_ = 2160;
};
```

#### `OverlayRenderer`

Composites overlay graphics (lower-thirds, name tags, logos) onto frames.

```cpp
class OverlayRenderer {
public:
    OverlayRenderer();

    // Load overlay templates
    bool loadTemplate(const std::string& templatePath);

    // Render overlay onto frame based on metadata
    void renderLowerThird(VideoFrame& frame,
                          const std::string& name,
                          const std::string& role,
                          const OverlayStyle& style);

    void renderLogo(VideoFrame& frame,
                    const std::string& logoPath,
                    Position position);

    // Metadata-driven rendering
    void applyMetadata(VideoFrame& frame,
                       const RecognitionMetadata& metadata);

private:
    std::map<std::string, OverlayTemplate> templates_;
    FreeType fontRenderer_;  // Font rendering engine
};
```

#### `FilterChain`

Applies sequential video filters (color correction, LUTs, blur, sharpening).

```cpp
class FilterChain {
public:
    FilterChain();

    void addFilter(std::unique_ptr<IVideoFilter> filter);
    void removeFilter(const std::string& filterName);
    void reorderFilters(const std::vector<std::string>& order);

    VideoFrame apply(const VideoFrame& input);

    void setEnabled(const std::string& filterName, bool enabled);

private:
    std::vector<std::unique_ptr<IVideoFilter>> filters_;
};

// Filter interface
class IVideoFilter {
public:
    virtual ~IVideoFilter() = default;
    virtual std::string name() const = 0;
    virtual VideoFrame process(const VideoFrame& input) = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
};
```

#### `OutputManager`

Routes processed frames to broadcast outputs (SDI, NDI, SRT, RTMP, file recording).

```cpp
class OutputManager {
public:
    OutputManager();

    bool addOutput(const std::string& name, std::shared_ptr<IVideoDevice> device);
    bool removeOutput(const std::string& name);

    void sendFrame(const VideoFrame& frame);  // Broadcast to all outputs
    void sendFrame(const std::string& outputName, const VideoFrame& frame);

    // Recording
    void startRecording(const std::string& path, const EncoderConfig& config);
    void stopRecording();

private:
    std::map<std::string, std::shared_ptr<IVideoDevice>> outputs_;
    std::unique_ptr<VideoEncoder> recorder_;
    std::mutex mutex_;
};
```

#### `MetadataReceiver`

Receives face recognition metadata from the Python AI module via ZeroMQ.

```cpp
class MetadataReceiver {
public:
    MetadataReceiver(const std::string& endpoint = "tcp://127.0.0.1:5557");
    ~MetadataReceiver();

    void start();  // Begin listening in background thread
    void stop();

    // Get latest recognition metadata (thread-safe)
    RecognitionMetadata getLatestMetadata() const;
    bool hasNewMetadata() const;

private:
    zmq::context_t context_;
    zmq::socket_t socket_;
    std::thread listenerThread_;
    std::atomic<bool> running_;
    mutable std::mutex dataMutex_;
    RecognitionMetadata latestMetadata_;
};
```

#### `VisionEngine`

Self-contained vision engine combining live video capture, an OpenGL GPU rendering pipeline with texture upload, a real-time preview window, and a frame-rate-governed render loop.

When built with `HAS_OPENGL=1` (GLFW + GLEW + OpenGL 3.3), the engine creates a native GPU window and renders captured frames through a full-screen-quad shader. Without OpenGL it falls back to an OpenCV highgui preview.

```cpp
/// Start-up configuration.
struct VisionEngineConfig {
    int captureDeviceIndex = 0;
    std::string captureUri;           // File or stream URI
    int previewWidth  = 1280;
    int previewHeight = 720;
    std::string windowTitle = "VisionCast Preview";
    double targetFps  = 25.0;
    bool enableGpu    = true;         // Attempt GPU pipeline
};

/// Lightweight GPU texture handle.
struct GpuTexture {
    unsigned int id = 0;              // OpenGL texture name
    int width  = 0;
    int height = 0;
    bool valid = false;
};

class VisionEngine {
public:
    VisionEngine();
    explicit VisionEngine(const VisionEngineConfig& config);
    ~VisionEngine();

    // Lifecycle
    bool initialize();                // Capture + window + GPU
    void shutdown();

    // Video capture (OpenCV)
    bool openCapture(int deviceIndex);
    bool openCapture(const std::string& uri);
    void closeCapture();
    bool isCaptureOpen() const;
    bool grabFrame(VideoFrame& outFrame);

    // Preview window
    bool createPreviewWindow(const std::string& title, int w, int h);
    void destroyPreviewWindow();
    bool isPreviewOpen() const;

    // GPU pipeline (OpenGL 3.3 / Vulkan-ready)
    bool initGpuPipeline();
    void shutdownGpuPipeline();
    bool isGpuReady() const;

    // GPU textures
    GpuTexture uploadTexture(const uint8_t* data, int w, int h, int ch);
    bool updateTexture(GpuTexture& tex, const uint8_t* data, int w, int h, int ch);
    void deleteTexture(GpuTexture& tex);

    // Render loop (blocking)
    void run();
    void stop();
    bool isRunning() const;

    // Configuration
    void setFrameRate(double fps);
    void setResolution(int width, int height);
    using FrameCallback = std::function<void(VideoFrame&)>;
    void setFrameCallback(FrameCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;  // PIMPL hides OpenCV/GL/GLFW types
};
```

### 4.3 Video Frame Structure

```cpp
struct VideoFrame {
    uint8_t* data;          // Pixel data (BGRA 8-bit or 10-bit)
    int width;              // Frame width (e.g. 3840)
    int height;             // Frame height (e.g. 2160)
    int stride;             // Bytes per row
    PixelFormat format;     // BGRA8, UYVY, V210, NV12
    int64_t timestampUs;    // Presentation timestamp (microseconds)
    int frameNumber;        // Sequential frame counter
};

enum class PixelFormat {
    BGRA8,     // 8-bit BGRA (GPU-friendly)
    UYVY,      // 4:2:2 packed YCbCr (SDI standard)
    V210,      // 10-bit 4:2:2 packed YCbCr (broadcast standard)
    NV12       // 4:2:0 semi-planar (encoding)
};
```

### 4.4 Build Dependencies

| Library | Purpose |
|---|---|
| **OpenCV** (≥ 4.8) | Image processing, color conversion |
| **FFmpeg** (≥ 6.0) | Video encoding/decoding |
| **ZeroMQ** (≥ 4.3) | IPC with Python AI module |
| **nlohmann/json** | JSON parsing (metadata) |
| **FreeType** (≥ 2.13) | Text rendering for overlays |
| **GLFW + OpenGL** | GPU-accelerated compositing (optional) |

---

## 5. Qt Control Room (`ui/`)

### 5.1 Purpose

The Qt control room is the operator interface for the broadcast system. It provides live preview, overlay editing, talent management, output configuration, and system monitoring.

### 5.2 Classes

#### `MainWindow`

Top-level application window with dockable panels.

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onSourceChanged(const QString& source);
    void onOverlayToggled(bool enabled);
    void onGoLive();
    void onStopBroadcast();

private:
    PreviewPanel* previewPanel_;
    OverlayEditor* overlayEditor_;
    TalentManager* talentManager_;
    OutputConfig* outputConfig_;
    MonitoringPanel* monitoringPanel_;
    QDockWidget* createDock(const QString& title, QWidget* widget);
};
```

#### `PreviewPanel`

Displays live video preview with multiview support.

```cpp
class PreviewPanel : public QWidget {
    Q_OBJECT
public:
    explicit PreviewPanel(QWidget* parent = nullptr);

    void setPreviewFrame(const QImage& frame);
    void setMultiviewLayout(int rows, int cols);

signals:
    void sourceSelected(const QString& sourceName);
    void previewClicked(const QPoint& position);

private:
    QLabel* previewLabel_;
    QGridLayout* multiviewGrid_;
    std::vector<QLabel*> multiviewPanels_;
};
```

#### `OverlayEditor`

Template editor for lower-thirds and other overlay graphics.

```cpp
class OverlayEditor : public QWidget {
    Q_OBJECT
public:
    explicit OverlayEditor(QWidget* parent = nullptr);

    void loadTemplate(const QString& path);
    void saveTemplate(const QString& path);

signals:
    void templateChanged(const OverlayTemplate& tmpl);

private:
    QLineEdit* titleEdit_;
    QLineEdit* subtitleEdit_;
    QComboBox* styleCombo_;
    QColorDialog* colorPicker_;
    QPushButton* applyButton_;
};
```

#### `TalentManager`

CRUD interface for the talent database.

```cpp
class TalentManager : public QWidget {
    Q_OBJECT
public:
    explicit TalentManager(QWidget* parent = nullptr);

    void loadDatabase(const QString& path);
    void saveDatabase(const QString& path);

signals:
    void talentSelected(const TalentInfo& talent);
    void databaseUpdated();

private:
    QTableView* talentTable_;
    QStandardItemModel* model_;
    QPushButton* addButton_;
    QPushButton* removeButton_;
    QPushButton* editButton_;
};
```

#### `OutputConfig`

Configuration panel for output devices and encoding settings.

```cpp
class OutputConfig : public QWidget {
    Q_OBJECT
public:
    explicit OutputConfig(QWidget* parent = nullptr);

    void refreshDevices();
    OutputSettings getCurrentSettings() const;

signals:
    void settingsChanged(const OutputSettings& settings);

private:
    QComboBox* deviceCombo_;       // DeckLink, AJA, etc.
    QComboBox* resolutionCombo_;   // 1080p, 2160p
    QComboBox* frameRateCombo_;    // 25, 29.97, 50, 59.94
    QComboBox* formatCombo_;       // SDI, HDMI, NDI
    QPushButton* applyButton_;
};
```

#### `MonitoringPanel`

System health monitoring (CPU, GPU, latency, dropped frames).

```cpp
class MonitoringPanel : public QWidget {
    Q_OBJECT
public:
    explicit MonitoringPanel(QWidget* parent = nullptr);

    void updateMetrics(const SystemMetrics& metrics);

private:
    QLabel* cpuLabel_;
    QLabel* gpuLabel_;
    QLabel* latencyLabel_;
    QLabel* fpsLabel_;
    QLabel* droppedFramesLabel_;
    QProgressBar* cpuBar_;
    QProgressBar* gpuBar_;
    QTimer* refreshTimer_;
};
```

---

## 6. SDK Abstraction Layer (`sdk/`)

### 6.1 Purpose

Provides a unified C++ interface for professional video capture/playout hardware, allowing the engine to work with any supported device through a common API.

### 6.2 Abstract Interface

```cpp
class IVideoDevice {
public:
    virtual ~IVideoDevice() = default;

    // Lifecycle
    virtual bool open(const DeviceConfig& config) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    // Capabilities
    virtual std::string deviceName() const = 0;
    virtual DeviceType deviceType() const = 0;  // CAPTURE, PLAYOUT, BIDIRECTIONAL
    virtual std::vector<VideoMode> supportedModes() const = 0;

    // Capture
    virtual bool startCapture(const VideoMode& mode) = 0;
    virtual bool stopCapture() = 0;
    virtual VideoFrame captureFrame() = 0;

    // Playout
    virtual bool startPlayout(const VideoMode& mode) = 0;
    virtual bool stopPlayout() = 0;
    virtual bool sendFrame(const VideoFrame& frame) = 0;

    // Configuration
    virtual void setVideoMode(const VideoMode& mode) = 0;
    virtual VideoMode currentMode() const = 0;
};

struct VideoMode {
    int width;          // 1920, 3840
    int height;         // 1080, 2160
    double frameRate;   // 25.0, 29.97, 50.0, 59.94
    PixelFormat format; // UYVY, V210, BGRA8
    bool interlaced;    // true for 1080i
};

struct DeviceConfig {
    int deviceIndex;    // Hardware device index
    int subDeviceIndex; // Sub-device (e.g. SDI input 1-4)
    std::string name;   // User-friendly name
};

enum class DeviceType {
    CAPTURE,
    PLAYOUT,
    BIDIRECTIONAL
};
```

### 6.3 Supported Hardware

#### Blackmagic DeckLink (`decklink_device.h`)

```cpp
class DeckLinkDevice : public IVideoDevice {
public:
    DeckLinkDevice();
    ~DeckLinkDevice() override;

    // IVideoDevice interface implementation
    bool open(const DeviceConfig& config) override;
    void close() override;
    // ... (all virtual methods)

    // DeckLink-specific
    static std::vector<DeviceConfig> enumerateDevices();

private:
    IDeckLink* deckLink_ = nullptr;
    IDeckLinkInput* input_ = nullptr;
    IDeckLinkOutput* output_ = nullptr;
};
```

#### AJA (`aja_device.h`)

```cpp
class AJADevice : public IVideoDevice {
public:
    AJADevice();
    ~AJADevice() override;

    // IVideoDevice interface implementation
    bool open(const DeviceConfig& config) override;
    void close() override;
    // ... (all virtual methods)

    // AJA-specific
    static std::vector<DeviceConfig> enumerateDevices();

private:
    CNTV2Card* card_ = nullptr;
    NTV2Channel channel_ = NTV2_CHANNEL1;
};
```

#### Magewell (`magewell_device.h`)

```cpp
class MagewellDevice : public IVideoDevice {
public:
    MagewellDevice();
    ~MagewellDevice() override;

    // IVideoDevice interface implementation
    bool open(const DeviceConfig& config) override;
    void close() override;
    // ... (all virtual methods)

    // Magewell-specific
    static std::vector<DeviceConfig> enumerateDevices();

private:
    HCHANNEL channel_ = nullptr;
};
```

#### NDI (`ndi_device.h`)

```cpp
class NDIDevice : public IVideoDevice {
public:
    NDIDevice();
    ~NDIDevice() override;

    // IVideoDevice interface implementation
    bool open(const DeviceConfig& config) override;
    void close() override;
    // ... (all virtual methods)

    // NDI-specific
    static std::vector<DeviceConfig> discoverSources();
    void setSourceName(const std::string& name);

private:
    NDIlib_send_instance_t sender_ = nullptr;
    NDIlib_recv_instance_t receiver_ = nullptr;
    NDIlib_find_instance_t finder_ = nullptr;
};
```

### 6.4 Role-Specific Input/Output Classes

In addition to the bidirectional device wrappers above, the SDK provides specialised
input-only and output-only classes.  Each class implements `IVideoDevice` and exposes
role-specific helpers (signal detection, connector selection, genlock reference, etc.).

#### DeckLinkInput / DeckLinkOutput (`decklink_input.h` / `decklink_output.h`)

```cpp
enum class DeckLinkConnector { SDI, HDMI, AUTO };
enum class DeckLinkReference  { FREE_RUN, EXTERNAL, INPUT };

class DeckLinkInput : public IVideoDevice {
public:
    // IVideoDevice (capture path only; playout methods are no-ops)
    void setConnector(DeckLinkConnector connector);
    DeckLinkConnector connector() const;
    bool hasSignal() const;
    VideoMode detectedMode() const;
    static std::vector<DeviceConfig> enumerateDevices();
};

class DeckLinkOutput : public IVideoDevice {
public:
    // IVideoDevice (playout path only; capture methods are no-ops)
    void setConnector(DeckLinkConnector connector);
    DeckLinkConnector connector() const;
    void setReferenceSource(DeckLinkReference ref);
    DeckLinkReference referenceSource() const;
    bool isOutputActive() const;
    static std::vector<DeviceConfig> enumerateDevices();
};
```

#### AJAInput / AJAOutput (`aja_input.h` / `aja_output.h`)

```cpp
enum class AJAReference { FREE_RUN, EXTERNAL, INPUT };

class AJAInput : public IVideoDevice {
public:
    void setChannel(int channel);
    int channel() const;
    bool hasSignal() const;
    VideoMode detectedMode() const;
    static std::vector<DeviceConfig> enumerateDevices();
};

class AJAOutput : public IVideoDevice {
public:
    void setChannel(int channel);
    int channel() const;
    void setReferenceSource(AJAReference ref);
    AJAReference referenceSource() const;
    bool isOutputActive() const;
    static std::vector<DeviceConfig> enumerateDevices();
};
```

#### MagewellInput (`magewell_input.h`)

```cpp
class MagewellInput : public IVideoDevice {
public:
    bool hasSignal() const;
    VideoMode detectedMode() const;
    static std::vector<DeviceConfig> enumerateDevices();
};
```

#### NDIInput / NDIOutput (`ndi_input.h` / `ndi_output.h`)

```cpp
class NDIInput : public IVideoDevice {
public:
    static std::vector<DeviceConfig> discoverSources();
    bool hasSignal() const;
    VideoMode detectedMode() const;
};

class NDIOutput : public IVideoDevice {
public:
    void setSourceName(const std::string& name);
    std::string sourceName() const;
    bool isOutputActive() const;
};
```

#### SRTOutput (`srt_output.h`)

```cpp
class SRTOutput : public IVideoDevice {
public:
    void setDestination(const std::string& url);
    std::string destination() const;
    void setLatency(int ms);
    int latency() const;
    bool isOutputActive() const;
};
```

#### RTMPOutput (`rtmp_output.h`) - DEPRECATED

> **Deprecated:** Use `FFmpegRtmpOutput` from `engine/include/visioncast/ffmpeg_rtmp.h` instead.

```cpp
class RTMPOutput : public IVideoDevice {
public:
    void setServerUrl(const std::string& url);
    std::string serverUrl() const;
    void setStreamKey(const std::string& key);
    std::string streamKey() const;
    bool isOutputActive() const;
};
```

#### FFmpegRtmpOutput (`engine/include/visioncast/ffmpeg_rtmp.h`) - NEW

```cpp
namespace visioncast {

struct RtmpStreamConfig {
    std::string serverUrl;
    std::string streamKey;
    int width = 1920;
    int height = 1080;
    double frameRate = 30.0;
    int bitrateBps = 4000000;
    std::string preset = "veryfast";
    std::string tune = "zerolatency";
};

class FFmpegRtmpOutput {
public:
    void setConfig(const RtmpStreamConfig& config);
    bool open();
    void close();
    bool start();
    bool stop();
    bool sendFrame(const RtmpFrame& frame);
    RtmpStatus status() const;
};

} // namespace visioncast
```

---

## 7. Data Flow & Video Pipeline

### 7.1 Frame Processing Pipeline

```
┌─────────┐    ┌──────────┐    ┌────────────┐    ┌──────────┐    ┌──────────┐
│ Capture  │───→│  Decode  │───→│   Filter   │───→│ Overlay  │───→│  Output  │
│ (SDK)    │    │ (FFmpeg) │    │   Chain    │    │ Compose  │    │ (SDK)    │
└─────────┘    └──────────┘    └────────────┘    └──────────┘    └──────────┘
                                                       ▲
                                                       │
                                               ┌───────┴───────┐
                                               │   Metadata    │
                                               │  (from Python │
                                               │   via ZeroMQ) │
                                               └───────────────┘
```

### 7.2 Detailed Data Flow

1. **Capture**: SDK device captures raw video frame (UYVY/V210) from SDI/HDMI/NDI
2. **Decode**: Convert to internal working format (BGRA8 for GPU processing)
3. **AI Detection** (parallel): Python module receives downscaled frames, performs face detection/recognition, sends metadata via ZeroMQ
4. **Filter Chain**: Apply color correction, LUTs, sharpening, denoising sequentially
5. **Overlay Compositing**: Render lower-thirds, logos, tickers based on AI metadata
6. **Output Encoding**: Convert to output format (V210 for SDI, H.264/H.265 for recording)
7. **Playout**: Send to SDK device for broadcast output + NDI/SRT/RTMP for streaming

### 7.3 Threading Model

```
Thread 1: Capture Thread
  └─ Continuously captures frames from the active source
  └─ Places frames in a lock-free ring buffer

Thread 2: AI Feed Thread
  └─ Takes every Nth frame, downscales to 720p
  └─ Sends to Python module via shared memory or ZeroMQ

Thread 3: Pipeline Thread (main processing)
  └─ Dequeues frames from capture buffer
  └─ Applies filter chain
  └─ Reads latest AI metadata
  └─ Renders overlays
  └─ Sends to output manager

Thread 4: Output Thread
  └─ Takes processed frames from output buffer
  └─ Sends to all active outputs (SDI, NDI, SRT, RTMP, recording)

Thread 5: UI Thread (Qt event loop)
  └─ Handles user interaction
  └─ Updates preview panels
  └─ Sends configuration changes to engine
```

---

## 8. Python ↔ C++ IPC Protocol

### 8.1 Transport Layer

| Protocol | Use Case | Port |
|---|---|---|
| **ZeroMQ PUB/SUB** | AI metadata (Python → C++) | `tcp://127.0.0.1:5557` |
| **ZeroMQ REQ/REP** | Configuration commands (C++ → Python) | `tcp://127.0.0.1:5556` |
| **Shared Memory** | Frame exchange for AI processing (optional, high-perf) | N/A |

### 8.2 Message Format (JSON over ZeroMQ)

#### Recognition Result (Python → C++ Engine)

```json
{
  "type": "recognition_result",
  "timestamp_ms": 1709654400000,
  "frame_id": 42,
  "faces": [
    {
      "location": {
        "top": 120,
        "right": 450,
        "bottom": 320,
        "left": 280
      },
      "talent": {
        "id": "glody",
        "name": "Glody",
        "role": "Présentateur",
        "overlay_template": "overlays/default.json"
      },
      "confidence": 0.92
    }
  ]
}
```

#### Configuration Command (C++ Engine → Python)

```json
{
  "type": "config",
  "command": "set_tolerance",
  "params": {
    "tolerance": 0.45
  }
}
```

#### Heartbeat (bidirectional)

```json
{
  "type": "heartbeat",
  "module": "python_ai",
  "timestamp_ms": 1709654400000,
  "status": "running",
  "fps": 12.5
}
```

### 8.3 Topic-Based Routing

ZeroMQ PUB/SUB messages use topic prefixes:

| Topic | Direction | Content |
|---|---|---|
| `face.result` | Python → Engine | Face recognition results |
| `face.heartbeat` | Python → Engine | Python module health status |
| `config.ai` | Engine → Python | AI parameter updates |
| `config.pipeline` | UI → Engine | Pipeline configuration |
| `monitor.status` | Engine → UI | System metrics |

---

## 9. Configuration Files

### 9.1 System Configuration (`config/system.json`)

```json
{
  "engine": {
    "resolution": { "width": 3840, "height": 2160 },
    "frame_rate": 25.0,
    "pixel_format": "V210"
  },
  "ai": {
    "model": "hog",
    "tolerance": 0.45,
    "process_every_n_frames": 3,
    "zmq_pub_endpoint": "tcp://127.0.0.1:5557"
  },
  "output": {
    "primary": {
      "type": "decklink",
      "device_index": 0,
      "mode": "2160p25"
    },
    "ndi": {
      "enabled": true,
      "source_name": "VisionCast Program"
    },
    "recording": {
      "enabled": false,
      "path": "/recordings/",
      "codec": "h265",
      "bitrate_mbps": 50
    }
  }
}
```

---

## 10. Build System

### 10.1 Top-Level CMake

The project uses CMake with three main targets:

- `visioncast_engine` — C++ video processing engine
- `visioncast_ui` — Qt control room application
- `visioncast_sdk` — Hardware SDK abstraction library

### 10.2 Build Instructions

```bash
# Build the C++ engine + SDK + UI
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# Install Python dependencies
cd ../python
pip install -r requirements.txt

# Run the system
./build/bin/visioncast_engine &   # Start C++ engine
python python/main.py &           # Start AI module
./build/bin/visioncast_ui         # Start Qt control room
```

---

## 11. Future Extensions

| Feature | Description | Priority |
|---|---|---|
| **GPU Compositing** | OpenGL/Vulkan accelerated overlay rendering | High |
| **Multi-Camera** | Simultaneous capture from multiple sources | High |
| **Remote Control** | Web-based control panel via WebSocket | Medium |
| **Replay System** | Instant replay with slow-motion | Medium |
| **Audio Mixing** | Embedded audio processing pipeline | Medium |
| **Graphics Engine** | HTML/CSS-based overlay templates (CEF) | Low |
| **Cloud Integration** | SRT/RIST streaming to cloud platforms | Low |

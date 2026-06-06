# 实时变声器 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers-extended-cc:subagent-driven-development (recommended) or superpowers-extended-cc:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a Windows desktop real-time voice changer with 3 neural character presets (甜妹音/御姐音/温青音) and a minimal floating window UI.

**Architecture:** C++ application with Qt 6 QML floating window UI, PortAudio/WASAPI audio I/O, WORLD vocoder for feature extraction, ONNX Runtime for neural voice conversion inference, and HiFi-GAN for waveform synthesis. Three independent RVC→ONNX models are loaded and switched via a ModelManager.

**Tech Stack:** C++17, Qt 6 (QML), CMake, vcpkg, PortAudio, ONNX Runtime, WORLD vocoder, MSVC 2022

---

## Prerequisites (before starting tasks)

Before any task, ensure these are available on the Windows machine:

```bash
# Install Qt 6.5+ via Qt Online Installer or vcpkg
# Install Visual Studio 2022 with C++ Desktop workload
# vcpkg should be available at %VCPKG_ROOT%
```

---

### Task 1: Project scaffolding with CMake and vcpkg

**Goal:** Create the project skeleton that compiles a minimal Qt application with all dependencies linked.

**Files:**
- Create: `E:/项目/voice-changer/CMakeLists.txt`
- Create: `E:/项目/voice-changer/vcpkg.json`
- Create: `E:/项目/voice-changer/CMakePresets.json`
- Create: `E:/项目/voice-changer/src/main.cpp`
- Create: `E:/项目/voice-changer/.gitignore`

**Acceptance Criteria:**
- [ ] `cmake --preset default` configures successfully
- [ ] `cmake --build build` compiles without errors
- [ ] Running the binary opens an empty Qt window with title "变声器"

**Verify:** `cmake --build build && ./build/Debug/voice-changer.exe` → window titled "变声器" appears

**Steps:**

- [ ] **Step 1: Write vcpkg.json manifest**

```json
{
  "name": "voice-changer",
  "version": "0.1.0",
  "dependencies": [
    "qtbase",
    "qtdeclarative",
    "portaudio",
    "onnxruntime",
    "spdlog"
  ]
}
```

- [ ] **Step 2: Write CMakePresets.json**

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "displayName": "Windows x64",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "VCPKG_TARGET_TRIPLET": "x64-windows"
      }
    }
  ]
}
```

- [ ] **Step 3: Write CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.21)
project(VoiceChanger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Gui)
find_package(portaudio REQUIRED)
find_package(onnxruntime REQUIRED)
find_package(spdlog REQUIRED)

# WORLD vocoder — compiled from source
add_library(world STATIC
  third_party/world/src/world/cheaptrick.cpp
  third_party/world/src/world/common.cpp
  third_party/world/src/world/d4c.cpp
  third_party/world/src/world/dio.cpp
  third_party/world/src/world/harvest.cpp
  third_party/world/src/world/matlabfunctions.cpp
  third_party/world/src/world/stonemask.cpp
  third_party/world/src/world/synthesis.cpp
  third_party/world/src/world/synthesisrealtime.cpp
)
target_include_directories(world PUBLIC third_party/world/src)

add_subdirectory(src)

qt_add_executable(voice-changer
  src/main.cpp
  ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

target_link_libraries(voice-changer PRIVATE
  voicechanger_lib
  Qt6::Core Qt6::Quick Qt6::Gui
)
```

- [ ] **Step 4: Write src/CMakeLists.txt**

```cmake
add_library(voicechanger_lib STATIC
  app/Application.cpp
  util/Logger.cpp
  util/Config.cpp
)

target_include_directories(voicechanger_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(voicechanger_lib PUBLIC
  Qt6::Core Qt6::Quick Qt6::Gui
  portaudio onnxruntime spdlog world
)
```

- [ ] **Step 5: Write minimal main.cpp**

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("变声器");
    app.setApplicationVersion("0.1.0");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

- [ ] **Step 6: Write .gitignore**

```
build/
.vs/
*.user
*.onnx
*.pth
```

- [ ] **Step 7: Write resources.qrc**

```xml
<RCC>
    <qresource prefix="/">
        <file>resources/qml/main.qml</file>
    </qresource>
</RCC>
```

- [ ] **Step 8: Write placeholder main.qml**

```qml
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 200
    height: 60
    title: "变声器"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "#1a1a2e"

    Rectangle {
        anchors.fill: parent
        radius: 12
        color: "#1a1a2e"
        border.color: "#333355"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: "变声器"
            color: "white"
            font.pixelSize: 16
        }
    }
}
```

- [ ] **Step 9: Build and verify**

```bash
cmake --preset default
cmake --build build
```

---

### Task 2: Logger and Config utilities

**Goal:** Implement spdlog-based logging and QSettings-based config management.

**Files:**
- Create: `E:/项目/voice-changer/src/util/Logger.h`
- Create: `E:/项目/voice-changer/src/util/Logger.cpp`
- Create: `E:/项目/voice-changer/src/util/Config.h`
- Create: `E:/项目/voice-changer/src/util/Config.cpp`

**Acceptance Criteria:**
- [ ] `Logger::init()` sets up file + console sinks
- [ ] `LOG_INFO/LOG_ERROR` macros work across the project
- [ ] `Config` loads/saves preset name, volume, default model from QSettings
- [ ] Unit test verifies config round-trip

**Verify:** Custom build target `voice-changer-tests` → `./build/Debug/tests.exe` → all pass

**Steps:**

- [ ] **Step 1: Write Logger.h**

```cpp
#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

namespace vc {

class Logger {
public:
    static void init(const std::string& logPath = "voice-changer.log");
    static std::shared_ptr<spdlog::logger> get();
private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

} // namespace vc

#define VC_LOG_INFO(...)  vc::Logger::get()->info(__VA_ARGS__)
#define VC_LOG_ERROR(...) vc::Logger::get()->error(__VA_ARGS__)
#define VC_LOG_DEBUG(...) vc::Logger::get()->debug(__VA_ARGS__)
```

- [ ] **Step 2: Write Logger.cpp**

```cpp
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace vc {

std::shared_ptr<spdlog::logger> Logger::s_logger;

void Logger::init(const std::string& logPath) {
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);

    spdlog::sinks_init_list sinks = {consoleSink, fileSink};
    s_logger = std::make_shared<spdlog::logger>("voice-changer", sinks);
    s_logger->set_level(spdlog::level::debug);
    spdlog::register_logger(s_logger);
}

std::shared_ptr<spdlog::logger> Logger::get() { return s_logger; }

} // namespace vc
```

- [ ] **Step 3: Write Config.h**

```cpp
#pragma once
#include <QString>
#include <QSettings>

namespace vc {

struct AppConfig {
    QString currentPreset = "tianmei";   // default model
    int outputVolume = 80;               // 0-100
    bool voiceEnabled = false;           // start disabled
    QString modelPath = "models/";
};

class Config {
public:
    static void load(AppConfig& cfg);
    static void save(const AppConfig& cfg);
};

} // namespace vc
```

- [ ] **Step 4: Write Config.cpp**

```cpp
#include "Config.h"

namespace vc {

void Config::load(AppConfig& cfg) {
    QSettings settings("VoiceChanger", "VoiceChanger");
    cfg.currentPreset = settings.value("preset", "tianmei").toString();
    cfg.outputVolume = settings.value("volume", 80).toInt();
    cfg.voiceEnabled = settings.value("enabled", false).toBool();
    cfg.modelPath = settings.value("modelPath", "models/").toString();
}

void Config::save(const AppConfig& cfg) {
    QSettings settings("VoiceChanger", "VoiceChanger");
    settings.setValue("preset", cfg.currentPreset);
    settings.setValue("volume", cfg.outputVolume);
    settings.setValue("enabled", cfg.voiceEnabled);
    settings.setValue("modelPath", cfg.modelPath);
}

} // namespace vc
```

- [ ] **Step 5: Write tests/test_config.cpp**, write tests/test_main.cpp, update CMakeLists for tests

- [ ] **Step 6: Build, run tests, commit**

---

### Task 3: AudioCapture — Microphone input via PortAudio

**Goal:** Capture 48kHz mono 16-bit audio in 256-sample frames from the default microphone using PortAudio with WASAPI host.

**Files:**
- Create: `E:/项目/voice-changer/src/audio/AudioCapture.h`
- Create: `E:/项目/voice-changer/src/audio/AudioCapture.cpp`

**Acceptance Criteria:**
- [ ] Opens default microphone at 48kHz, mono, 16-bit PCM
- [ ] Callback delivers 256-sample frames
- [ ] `start()`/`stop()` start and stop the capture stream
- [ ] Ring buffer decouples capture callback from consumer thread

**Verify:** Write test that calls `start()`, sleeps 1 second, calls `stop()`, asserts buffer contains ~187 frames (48000/256 * 1s)

**Steps:**

- [ ] **Step 1: Write AudioCapture.h**

```cpp
#pragma once
#include <portaudio.h>
#include <vector>
#include <atomic>
#include <mutex>
#include <cstdint>

namespace vc {

struct AudioFrame {
    static constexpr int FRAME_SIZE = 256;
    int16_t samples[FRAME_SIZE];
};

class AudioCapture {
public:
    AudioCapture();
    ~AudioCapture();

    bool start();
    void stop();
    bool isRunning() const;

    // Consumer: pop one frame, returns false if buffer empty
    bool popFrame(AudioFrame& frame);

    double sampleRate() const { return 48000.0; }

private:
    static int paCallback(const void* input, void* output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData);

    PaStream* m_stream = nullptr;
    std::atomic<bool> m_running{false};

    // Ring buffer: 2 seconds worth of frames
    static constexpr int RING_SIZE = 512;
    AudioFrame m_ringBuffer[RING_SIZE];
    std::atomic<int> m_writeIdx{0};
    std::atomic<int> m_readIdx{0};
};

} // namespace vc
```

- [ ] **Step 2: Write AudioCapture.cpp with ring buffer logic**

- [ ] **Step 3: Write test test_audio_capture.cpp, build, run, verify frames collected**

---

### Task 4: AudioOutput — WASAPI virtual speaker output

**Goal:** Output processed audio to a virtual audio device (VB-Cable) via PortAudio with WASAPI host API.

**Files:**
- Create: `E:/项目/voice-changer/src/audio/AudioOutput.h`
- Create: `E:/项目/voice-changer/src/audio/AudioOutput.cpp`

**Acceptance Criteria:**
- [ ] Opens the virtual output device (VB-Audio CABLE Input) at 48kHz mono
- [ ] `pushFrame()` accepts processed AudioFrame for playback
- [ ] Device not found → logs error and returns false gracefully
- [ ] Ring buffer prevents underruns

**Verify:** Write test that pushes 100 silent frames, verifies no crash and no buffer overflow

**Steps:**

- [ ] **Step 1: Write AudioOutput.h** — similar structure as AudioCapture but for output direction

```cpp
#pragma once
#include <portaudio.h>
#include <atomic>
#include "AudioCapture.h" // for AudioFrame

namespace vc {

class AudioOutput {
public:
    AudioOutput();
    ~AudioOutput();

    bool start(const char* deviceName = nullptr); // nullptr = auto-find VB-Cable
    void stop();
    bool isRunning() const;

    // Producer: push processed frame to output buffer
    bool pushFrame(const AudioFrame& frame);

private:
    static int paCallback(const void* input, void* output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData);

    PaStream* m_stream = nullptr;
    std::atomic<bool> m_running{false};

    static constexpr int RING_SIZE = 256;
    AudioFrame m_ringBuffer[RING_SIZE];
    std::atomic<int> m_writeIdx{0};
    std::atomic<int> m_readIdx{0};
};

} // namespace vc
```

- [ ] **Step 2: Write AudioOutput.cpp** — enumerate devices, find "CABLE Input", open stream

- [ ] **Step 3: Write test, build, verify**

---

### Task 5: AudioPipeline — Capture→Process→Output threading

**Goal:** Wire AudioCapture → processing placeholder → AudioOutput with a dedicated processing thread and double-buffering.

**Files:**
- Create: `E:/项目/voice-changer/src/audio/AudioPipeline.h`
- Create: `E:/项目/voice-changer/src/audio/AudioPipeline.cpp`
- Modify: `E:/项目/voice-changer/src/CMakeLists.txt`

**Acceptance Criteria:**
- [ ] `start()` launches capture stream + processing thread + output stream
- [ ] Processing callback receives each captured frame and passes result to output
- [ ] `stop()` cleanly shuts down all threads
- [ ] Processing callback is configurable via `std::function`

**Verify:** Test with pass-through (identity) processing — mic input should echo to output

**Steps:**

- [ ] **Step 1: Write AudioPipeline.h**

```cpp
#pragma once
#include "AudioCapture.h"
#include "AudioOutput.h"
#include <functional>
#include <thread>
#include <atomic>

namespace vc {

using ProcessCallback = std::function<void(const AudioFrame& in, AudioFrame& out)>;

class AudioPipeline {
public:
    AudioPipeline();
    ~AudioPipeline();

    bool start(ProcessCallback processor);
    void stop();
    bool isRunning() const;

    AudioCapture& capture() { return m_capture; }
    AudioOutput& output() { return m_output; }

private:
    void processLoop();

    AudioCapture m_capture;
    AudioOutput m_output;
    ProcessCallback m_processor;
    std::thread m_processThread;
    std::atomic<bool> m_running{false};
};

} // namespace vc
```

- [ ] **Step 2: Write AudioPipeline.cpp** — processLoop: pop from capture → call processor → push to output

- [ ] **Step 3: Write test, build, verify**

---

### Task 6: FeatureExtractor — WORLD vocoder wrapper

**Goal:** Wrap WORLD vocoder to extract F0, spectral envelope (SP), and aperiodicity (AP) from each 256-sample audio frame. Required as input to the ONNX voice conversion model.

**Files:**
- Create: `E:/项目/voice-changer/src/audio/FeatureExtractor.h`
- Create: `E:/项目/voice-changer/src/audio/FeatureExtractor.cpp`
- Create: `E:/项目/voice-changer/third_party/world/` (clone WORLD source)

**Acceptance Criteria:**
- [ ] Given 256 int16 samples, returns F0 contour, spectral envelope (SP), and aperiodicity (AP)
- [ ] SP output dimensions match expected ONNX model input (e.g., 513 bins for 1024-FFT)
- [ ] F0 array size = number of frames processed (typically 1 for single-frame extraction)
- [ ] No memory leaks (WORLD uses malloc/free internally)

**Verify:** Test with a known sine wave input, assert valid F0 and non-zero SP values

**Steps:**

- [ ] **Step 1: Clone WORLD vocoder into third_party/**

```bash
git clone https://github.com/mmorise/World.git third_party/world
```

- [ ] **Step 2: Write FeatureExtractor.h**

```cpp
#pragma once
#include <vector>
#include <cstdint>

namespace vc {

struct WorldFeatures {
    std::vector<double> f0;          // fundamental frequency per frame
    std::vector<std::vector<double>> sp;   // spectral envelope [n_frames][fft_bins]
    std::vector<std::vector<double>> ap;   // aperiodicity [n_frames][fft_bins]
    int fftSize = 1024;
    int spDim = 513;                 // fftSize/2 + 1
};

class FeatureExtractor {
public:
    FeatureExtractor(int sampleRate = 48000, int frameSize = 256);
    ~FeatureExtractor();

    // Extract features from single audio frame
    WorldFeatures extract(const int16_t* samples, int numSamples);

    int fftSize() const { return 1024; }
    int spDim() const { return 513; }

private:
    int m_sampleRate;
    int m_frameSize;
    std::vector<double> m_buffer; // internal double buffer for WORLD
};

} // namespace vc
```

- [ ] **Step 3: Write FeatureExtractor.cpp** — call CheapTrick, D4C, DIO/Harvest

- [ ] **Step 4: Write test, build, verify**

---

### Task 7: OnnxInference — ONNX Runtime C++ wrapper

**Goal:** Generic wrapper around ONNX Runtime C API for loading models and running inference on float tensors.

**Files:**
- Create: `E:/项目/voice-changer/src/model/OnnxInference.h`
- Create: `E:/项目/voice-changer/src/model/OnnxInference.cpp`

**Acceptance Criteria:**
- [ ] `loadModel(path)` loads an ONNX model and creates a session
- [ ] `run(inputs)` accepts named float tensors, returns named output tensors
- [ ] Works with CPU execution provider
- [ ] Handles model load failure gracefully (returns false, logs error)

**Verify:** Create a tiny ONNX model (constant → output) via Python, load it, run inference, assert output matches expected

**Steps:**

- [ ] **Step 1: Write OnnxInference.h**

```cpp
#pragma once
#include <onnxruntime_cxx_api.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace vc {

struct TensorData {
    std::vector<int64_t> shape;
    std::vector<float> data;
};

class OnnxInference {
public:
    OnnxInference();
    ~OnnxInference();

    bool loadModel(const std::string& modelPath);
    bool isLoaded() const;

    // Run inference: input name → tensor → output name → tensor
    std::unordered_map<std::string, TensorData>
    run(const std::unordered_map<std::string, TensorData>& inputs);

private:
    Ort::Env m_env;
    Ort::SessionOptions m_sessionOpts;
    std::unique_ptr<Ort::Session> m_session;
    std::vector<std::string> m_inputNames;
    std::vector<std::string> m_outputNames;
};

} // namespace vc
```

- [ ] **Step 2: Write OnnxInference.cpp** — Ort::MemoryInfo, Ort::Value creation from TensorData, session Run

- [ ] **Step 3: Write test with a Python-generated tiny ONNX model, build, verify**

---

### Task 8: ModelManager — Load, cache, and switch neural models

**Goal:** Manage the lifecycle of 3 converter models + 1 shared HiFi-GAN vocoder. Load on background thread, support hot-swap without interrupting audio.

**Files:**
- Create: `E:/项目/voice-changer/src/model/ModelManager.h`
- Create: `E:/项目/voice-changer/src/model/ModelManager.cpp`

**Acceptance Criteria:**
- [ ] `switchModel(name)` starts async load of target converter model
- [ ] During load, current model continues serving inference
- [ ] Once loaded, atomic swap replaces active model
- [ ] `loadVocoder()` loads the shared HiFi-GAN model on startup
- [ ] Preset names: "tianmei", "yujie", "wenqing"

**Verify:** Test with mock ONNX models — switch tianmei → yujie, assert old model active during load, new model active after completion

**Steps:**

- [ ] **Step 1: Write ModelManager.h** — activeModel pointer, model path mapping, async load thread

- [ ] **Step 2: Write ModelManager.cpp** — background loading via std::async, atomic pointer swap

- [ ] **Step 3: Write test, build, verify**

---

### Task 9: VoiceConverter — Full neural voice conversion pipeline

**Goal:** Combine FeatureExtractor + ONNX converter model + HiFi-GAN vocoder to convert one AudioFrame (mic input) into one AudioFrame (processed output).

**Files:**
- Create: `E:/项目/voice-changer/src/audio/VoiceConverter.h`
- Create: `E:/项目/voice-changer/src/audio/VoiceConverter.cpp`

**Acceptance Criteria:**
- [ ] `processFrame(in, out)` extracts features → runs ONNX inference → runs HiFi-GAN → writes output
- [ ] Works with both real models and identity pass-through when no model loaded
- [ ] Returns false if inference fails (logs error, passes through original audio)

**Verify:** Test with pass-through mock — input frame should equal output frame. Test with dummy model — output should differ from input.

**Steps:**

- [ ] **Step 1: Write VoiceConverter.h** — holds refs to FeatureExtractor, ModelManager, OnnxInference (vocoder)

- [ ] **Step 2: Write VoiceConverter.cpp** — pipeline: extract features → build ONNX input tensor → run converter → run vocoder → de-interleave to int16

- [ ] **Step 3: Write test, build, verify**

---

### Task 10: Qt Application class — App lifecycle and C++/QML bridge

**Goal:** Application class that owns the AudioPipeline, ModelManager, and Config, exposing properties/slots to QML.

**Files:**
- Create: `E:/项目/voice-changer/src/app/Application.h`
- Create: `E:/项目/voice-changer/src/app/Application.cpp`

**Acceptance Criteria:**
- [ ] `Application` is a `QObject` registered as a QML context property
- [ ] Properties: `currentPreset` (QString), `voiceEnabled` (bool), `volume` (int)
- [ ] Slots: `switchPreset(name)`, `toggleVoice()`, `setVolume(int)`
- [ ] On startup, loads config, initializes audio pipeline with VoiceConverter

**Verify:** QML can read `app.currentPreset` and call `app.toggleVoice()`

**Steps:**

- [ ] **Step 1: Write Application.h**

```cpp
#pragma once
#include <QObject>
#include <QString>
#include <memory>

namespace vc {
class AudioPipeline;
class ModelManager;
class VoiceConverter;
struct AppConfig;

class Application : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentPreset READ currentPreset NOTIFY currentPresetChanged)
    Q_PROPERTY(bool voiceEnabled READ voiceEnabled NOTIFY voiceEnabledChanged)
    Q_PROPERTY(int volume READ volume NOTIFY volumeChanged)

public:
    explicit Application(QObject* parent = nullptr);
    ~Application();

    QString currentPreset() const { return m_config.currentPreset; }
    bool voiceEnabled() const { return m_config.voiceEnabled; }
    int volume() const { return m_config.outputVolume; }

    Q_INVOKABLE void switchPreset(const QString& name);
    Q_INVOKABLE void toggleVoice();
    Q_INVOKABLE void setVolume(int vol);

signals:
    void currentPresetChanged();
    void voiceEnabledChanged();
    void volumeChanged();
    void presetLoadStarted(const QString& name);
    void presetLoadComplete(const QString& name);

private:
    AppConfig m_config;
    std::unique_ptr<AudioPipeline> m_pipeline;
    std::unique_ptr<ModelManager> m_modelManager;
    std::unique_ptr<VoiceConverter> m_converter;
};

} // namespace vc
```

- [ ] **Step 2: Write Application.cpp** — init logger, load config, init audio with VoiceConverter callback, register as QML type

- [ ] **Step 3: Build, verify QML property access**

---

### Task 11: FloatingWindow QML — Main UI

**Goal:** Implement the minimal floating window UI per the design spec: collapsed state with avatar + name + status dot, drag to move.

**Files:**
- Create: `E:/项目/voice-changer/resources/qml/main.qml`
- Create: `E:/项目/voice-changer/resources/qml/FloatingWindow.qml`
- Create: `E:/项目/voice-changer/resources/qml/theme.js`

**Acceptance Criteria:**
- [ ] Frameless, always-on-top, translucent rounded rectangle window
- [ ] Shows current preset's emoji + name + green/red status dot
- [ ] Left-click drag moves window
- [ ] Click on avatar toggles voice on/off
- [ ] Scroll wheel adjusts volume
- [ ] Window position persists via QSettings

**Verify:** Run app → floating window appears top-left → drag works → click toggles color

**Steps:**

- [ ] **Step 1: Write theme.js** — color definitions for 3 presets

- [ ] **Step 2: Write FloatingWindow.qml** — main visual component

```qml
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 180
    height: 48
    radius: 12
    color: "#1a1a2e"
    border.color: "#333355"
    border.width: 1

    property string presetName: "甜妹音"
    property string presetIcon: "🌸"
    property color presetColor: "#ff6b9d"
    property bool voiceOn: false

    // Drag support
    MouseArea {
        anchors.fill: parent
        property point lastPos: Qt.point(0, 0)
        onPressed: lastPos = Qt.point(mouseX, mouseY)
        onPositionChanged: {
            root.parent.x += mouseX - lastPos.x
            root.parent.y += mouseY - lastPos.y
        }
    }

    Row {
        anchors.centerIn: parent
        spacing: 8

        // Avatar circle
        Rectangle {
            width: 36; height: 36
            radius: 18
            gradient: Gradient {
                GradientStop { position: 0; color: root.presetColor }
                GradientStop { position: 1; color: Qt.darker(root.presetColor, 1.3) }
            }
            Text {
                anchors.centerIn: parent
                text: root.presetIcon
                font.pixelSize: 18
            }

            MouseArea {
                anchors.fill: parent
                onClicked: app.toggleVoice()
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            Text {
                text: root.presetName
                color: "white"
                font.pixelSize: 12
                font.bold: true
            }
            Row {
                spacing: 4
                Rectangle {
                    width: 6; height: 6
                    radius: 3
                    color: root.voiceOn ? "#44ff44" : "#ff4444"
                }
                Text {
                    text: root.voiceOn ? "已开启" : "已关闭"
                    color: "#888888"
                    font.pixelSize: 10
                }
            }
        }
    }
}
```

- [ ] **Step 3: Write main.qml** — instantiates FloatingWindow, connects to Application signals

- [ ] **Step 4: Build, launch, visually verify**

---

### Task 12: ContextMenu QML + System tray

**Goal:** Right-click context menu for preset switching and system tray integration.

**Files:**
- Create: `E:/项目/voice-changer/resources/qml/ContextMenu.qml`
- Modify: `E:/项目/voice-changer/resources/qml/main.qml`
- Modify: `E:/项目/voice-changer/src/main.cpp`
- Modify: `E:/项目/voice-changer/src/app/Application.cpp`

**Acceptance Criteria:**
- [ ] Right-click on floating window opens context menu with 3 preset options + fine-tune + close
- [ ] Selecting a preset triggers `app.switchPreset(name)`
- [ ] System tray icon appears, right-click shows "退出" option
- [ ] Window hides to tray instead of closing

**Verify:** Right-click floating window → see 5 menu items → click 御姐音 → window updates to purple/御姐

**Steps:**

- [ ] **Step 1: Write ContextMenu.qml** — popup menu with preset items using ListView

- [ ] **Step 2: Update FloatingWindow.qml** — add right-click MouseArea that shows ContextMenu

- [ ] **Step 3: Update main.cpp** — add QSystemTrayIcon with icon and quit action

- [ ] **Step 4: Build, verify interactions visually**

---

### Task 13: Fine-tune panel QML

**Goal:** Small popup panel for adjusting per-preset parameters: pitch offset, formant shift, mix level.

**Files:**
- Create: `E:/项目/voice-changer/resources/qml/FineTunePanel.qml`
- Modify: `E:/项目/voice-changer/resources/qml/ContextMenu.qml`
- Modify: `E:/项目/voice-changer/src/app/Application.h`
- Modify: `E:/项目/voice-changer/src/app/Application.cpp`

**Acceptance Criteria:**
- [ ] Clicking "微调" in context menu opens a small panel with 3 sliders
- [ ] Sliders: 音调偏移 (-12 to +12 semitones), 共振峰 (0.5x to 2.0x), 混音比例 (0% to 100%)
- [ ] Changing sliders updates live audio output
- [ ] Values persist per-preset in QSettings

**Verify:** Open fine-tune → drag pitch slider → confirm property updates in C++

**Steps:**

- [ ] **Step 1: Write FineTunePanel.qml** — compact slider panel

- [ ] **Step 2: Add fine-tune properties and slots to Application.h/cpp**

- [ ] **Step 3: Wire to VoiceConverter parameters**

- [ ] **Step 4: Build, verify visually**

---

### Task 14: Model download and ONNX conversion script

**Goal:** Python script that downloads community RVC voice models and converts them to FP16 ONNX format for use with this application.

**Files:**
- Create: `E:/项目/voice-changer/scripts/download_models.py`
- Create: `E:/项目/voice-changer/scripts/convert_to_onnx.py`
- Create: `E:/项目/voice-changer/scripts/requirements.txt`

**Acceptance Criteria:**
- [ ] Script searches HuggingFace for relevant RVC models (female voices matching 甜妹/御姐/温青 profiles)
- [ ] Downloads .pth files to models/ directory
- [ ] Converts each to FP16 ONNX via onnxruntime.quantization
- [ ] Also downloads/converts HiFi-GAN vocoder
- [ ] Verification step: runs test inference with the converted model

**Verify:** `python scripts/download_models.py && python scripts/convert_to_onnx.py` → 4 .onnx files in models/

**Steps:**

- [ ] **Step 1: Write requirements.txt** — torch, onnx, onnxruntime, huggingface_hub

- [ ] **Step 2: Write download_models.py** — search/download from HuggingFace (RVC community models)

- [ ] **Step 3: Write convert_to_onnx.py** — PyTorch → ONNX → FP16 quantize

- [ ] **Step 4: Run scripts, verify output files**

---

### Task 15: End-to-end integration and smoke test

**Goal:** Wire all components together and verify the complete flow: mic → capture → WORLD → ONNX → HiFi-GAN → output.

**Files:**
- Modify: `E:/项目/voice-changer/src/app/Application.cpp`
- Modify: `E:/项目/voice-changer/src/CMakeLists.txt`

**Acceptance Criteria:**
- [ ] App launches with floating window showing "甜妹音" preset
- [ ] Toggling voice starts/stops audio pipeline
- [ ] Switching preset triggers model load (log message confirms)
- [ ] System tray icon works
- [ ] No crash on exit (clean shutdown of audio threads)

**Verify:** Launch app → click avatar to enable → speak into mic → hear processed voice from VB-Cable

**Steps:**

- [ ] **Step 1: Update Application.cpp** — full initialization sequence

- [ ] **Step 2: Add shutdown sequence** — clean up audio pipeline before Qt exits

- [ ] **Step 3: Smoke test all interactions**

- [ ] **Step 4: Fix any threading/shutdown issues found**

---

### Task 16: Performance tuning and latency measurement

**Goal:** Measure end-to-end latency, profile CPU usage, and optimize hot paths to meet the < 50ms target.

**Files:**
- Modify: `E:/项目/voice-changer/src/audio/AudioPipeline.cpp` (add latency measurement)
- Modify: `E:/项目/voice-changer/src/model/OnnxInference.cpp` (thread affinity, session options)

**Acceptance Criteria:**
- [ ] End-to-end latency < 50ms (measured via loopback test)
- [ ] CPU usage < 20% single core during operation
- [ ] Model inference time logged per frame (target < 10ms)
- [ ] WORLD extraction time logged per frame (target < 8ms)
- [ ] HiFi-GAN vocoder time logged per frame (target < 5ms)
- [ ] Pipeline latency breakdown logged at INFO level every 1000 frames

**Verify:** Run app, speak, check logs for timing breakdown

**Steps:**

- [ ] **Step 1: Add high-resolution timers at each pipeline stage**

```cpp
// In AudioPipeline::processLoop()
auto t0 = std::chrono::high_resolution_clock::now();
// ... capture ...
auto t1 = std::chrono::high_resolution_clock::now();
// ... extract features ...
auto t2 = std::chrono::high_resolution_clock::now();
// ... ONNX inference ...
auto t3 = std::chrono::high_resolution_clock::now();
// ... HiFi-GAN ...
auto t4 = std::chrono::high_resolution_clock::now();
// ... output ...
auto t5 = std::chrono::high_resolution_clock::now();
// Log timings every 1000 frames
```

- [ ] **Step 2: Set ONNX intra-op thread affinity to high-performance core**

- [ ] **Step 3: Run loopback test (output → input via VB-Cable), measure round-trip**

- [ ] **Step 4: Profile with Windows Performance Recorder, identify bottlenecks, fix**

---

## Dependency Order

```
Task 1 ──→ Task 2 ──→ Task 3 ──→ Task 4 ──→ Task 5 ──→ Task 6 ──→ Task 7
                                                                         │
                                                                         ↓
                                              Task 14 ←── Task 9 ←── Task 8
                                                 │
                                                 ↓
Task 10 ──→ Task 11 ──→ Task 12 ──→ Task 13
    │           │           │           │
    └───────────┴───────────┴───────────┘
                    │
                    ↓
              Task 15 ──→ Task 16
```

- Task 14 (model scripts) can run in parallel with Tasks 10-13
- Tasks 10-13 depend on Application class being defined (Task 10)

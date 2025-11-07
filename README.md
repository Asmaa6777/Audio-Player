# 🪐 JUCE Dual Audio Player with Mixer and Audio Slicer

## 🎧 Overview

A modern **dual audio player** built with the JUCE framework, featuring **two synchronized players**, a **real-time mixer**, and an **innovative audio slicer**.
Designed for creative experimentation, music production, and DJ-style control, it delivers a clean interface and powerful playback tools.

---

## 🚀 Features

### 🎵 1. Dual Audio Players

* Two independent **PlayerGUI** components.
* Each can **load, play, stop, loop, mute,** and **seek** within an audio file.
* Displays **waveform previews** for both players.
* Only one player outputs audio at a time for clarity (unless **mixing mode** is enabled).

### 🎚️ 2. Mixer (New Feature)

* **Blend** two tracks by adjusting each player’s volume.
* Seamlessly mix outputs in real time.
* Independent **mute** and **loop** control per player.
* Enables creative **transitions** and **overlays** between tracks.

### ✂️ 3. Innovation: Audio Slicer

* Define **A–B markers** to slice audio segments.
* **Extract or loop** specific sections for creative remixing.
* Ideal for making **short samples**, **loops**, or **beat snippets**.
* Future versions will support **exporting slices** as standalone audio files.

---

## 🧩 Core Components

| File                       | Description                                                                    |
| -------------------------- | ------------------------------------------------------------------------------ |
| **MainComponent.h / .cpp** | Manages both players, the mixer logic, and waveform rendering.                 |
| **PlayerAudio.h / .cpp**   | Handles audio transport, playback, looping, volume, and markers.               |
| **PlayerGUI.h / .cpp**     | Provides the user interface (buttons, sliders, waveform, and marker controls). |
| **assets/icons/**          | Contains UI button icons (play, stop, loop, etc.).                             |
| **BinaryData**             | Compiled image and icon resources embedded in the app.                         |

---

## 🖥️ GUI Overview

* Gradient background with a **space-themed color palette** 🌌
* Each player includes:

  * **Load / Play / Stop / Loop / Mute / Skip** buttons
  * **Waveform preview**
  * **Volume and position sliders**
  * **A–B marker controls**
* A **central mixer panel** (coming soon) for smooth crossfades between Player 1 and Player 2.

---

## 🧠 How It Works

### 🎧 JUCE Audio Engine

* Uses `AudioTransportSource` for each player.
* Handles **real-time playback**, **looping**, and **position tracking**.

### 🎚️ Mixing Logic

* Combines two `AudioSourceChannelInfo` streams in the mixer.
* Each player’s **volume** determines its contribution to the final output buffer.

### ✂️ Slicing System

* Users set **markers A and B** to define a loop segment.
* The segment can be **looped**, **previewed**, or (in future versions) **exported**.

---

## 🛠️ Build Instructions

### Prerequisites

* **JUCE Framework (v7 or later)**
* **C++17** compatible compiler
* **Visual Studio**, **CLion**, or **Xcode**

### Build Steps

1. Open the `.jucer` project in **Projucer**.
2. Configure your preferred exporter (e.g., Visual Studio, Xcode).
3. Click **“Save and Open in IDE.”**
4. Build and run the project.

#### Alternatively, using CMake:

```bash
cmake -B build
cmake --build build
```

---

## 🔮 Future Improvements

* 🎚️ Dedicated mixer panel with smooth crossfades
* 🎛️ Per-player EQ and filter controls
* 💾 Export sliced clips as new audio files
* 🌈 Dynamic visualizer synchronized with playback
* 🎹 MIDI controller support for live performance

---

## 👩‍💻Team

**Asmaa Faroug**
**Arwa Bashier**
**Mariam Sherif**
**Mariam Ahmed**


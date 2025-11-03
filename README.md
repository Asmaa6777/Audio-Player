🪐 JUCE Dual Audio Player with Mixer and Audio Slicer
🎧 Overview

This project is a modern dual audio player built with JUCE, featuring two synchronized players, a mixer, and a unique audio slicer innovation.
It provides an immersive experience for playing, looping, and mixing tracks — ideal for creative experimentation, music production, or DJ-like control.

🚀 Features
🎵 1. Dual Audio Players

Two independent PlayerGUI components.

Each can load, play, loop, mute, and seek within an audio file.

Visually displays waveform previews for both players.

Only one player outputs audio at a time (for clarity), unless mixing mode is enabled.

🎚️ 2. Mixer (New Feature)

Blend two tracks by adjusting each player’s volume.

Mix their outputs seamlessly in real time.

Independent mute and loop control for each player.

Enables users to test simple transitions or overlay tracks creatively.

✂️ 3. Innovation: Audio Slicer

A creative feature that lets you slice audio segments.

Define start and end markers (A–B) and extract or loop just that region.

Useful for creating short samples, looping beats, or preparing remix snippets.

Can preview slices and export them as standalone clips (future extension).

🧩 Core Components
File	Description
MainComponent.h / .cpp	Manages both players, mixing logic, and waveform rendering.
PlayerAudio.h / .cpp	Handles audio transport, playback, volume, and markers.
PlayerGUI.h / .cpp	Manages all UI controls: play, pause, loop, markers, sliders, etc.
assets/icons/	Contains all icons for the UI buttons (play, stop, loop, etc).
BinaryData	Compiled image and icon resources included in the app.
🖥️ GUI Overview

Gradient background with a space-themed color palette 🌌

Each player has:

Load / Play / Stop / Loop / Mute / Skip buttons

A waveform preview

Volume and position sliders

A–B marker controls

Mixer control: central panel (to be added) lets you fade between Player 1 and Player 2

🧠 How It Works

JUCE Audio Engine

Uses AudioTransportSource for each player.

Handles real-time playback, looping, and position tracking.

Mixing Logic

Two AudioSourceChannelInfo streams combined in the mixer.

Each player’s volume determines its contribution to the final buffer.

Slicing System

Uses user-defined markers (A & B) to define loop segments.

Optional future extension: export slice to new file.

🛠️ Build Instructions
Prerequisites

JUCE Framework
 (version 7 or later)

C++17 compatible compiler

Visual Studio / CLion / Xcode

 
🔮 Future Improvements

🎚️ Dedicated Mixer Panel UI with smooth crossfades

🎛️ EQ and Filter controls per player

💾 Export sliced clips as new audio files

🌈 Dynamic visualizer synchronized with playback

🎚️ MIDI controller support for live performance

👩‍💻 Authors
Asmaa Faroug
Arwa bashier
Mariam Sherif
Mariam Ahmed

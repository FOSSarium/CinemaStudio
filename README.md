# CinemaStudio - Modern Video Editor

A full-fledged, modern video editor built with **C++ and Qt**, similar to Movavi, Adobe Premiere, and DaVinci Resolve.

![CinemaStudio](resources/icons/icon.png)

## Features

### Core Editing
- ✅ **Multi-track Timeline** - Video, audio, and text tracks
- ✅ **Real-time Preview** - Instant video playback
- ✅ **Cut, Trim, Split** - Professional editing tools
- ✅ **Drag & Drop** - Intuitive timeline editing
- ✅ **Zoom & Navigation** - Precise timeline control

### Effects & Filters
- ✅ **Video Effects** - Blur, sharpen, color correction, vignette, sepia, etc.
- ✅ **Transitions** - Fade, crossfade, wipe, slide, zoom, dissolve
- ✅ **Color Correction** - Brightness, contrast, hue, saturation
- ✅ **Audio Effects** - Fade in/out, normalize, equalizer, reverb

### Text & Titles
- ✅ **Text Overlays** - Customizable text with fonts, colors, positioning
- ✅ **Title Templates** - Pre-built title designs
- ✅ **Animated Text** - Fade in/out animations

### Audio
- ✅ **Audio Mixer** - Per-track volume and mute controls
- ✅ **Waveform Visualization** - See audio peaks on timeline
- ✅ **Multi-track Audio** - Sync multiple audio tracks

### Export
- ✅ **Multiple Formats** - MP4, AVI, MOV, MKV, WMV
- ✅ **Quality Settings** - CRF, bitrate, resolution, FPS
- ✅ **Hardware Acceleration** - Fast encoding with presets
- ✅ **Progress Tracking** - Real-time encoding status

### Project Management
- ✅ **Save/Load Projects** - `.csp` project files
- ✅ **Undo/Redo** - Full history support
- ✅ **Auto-save** - Never lose your work

## Architecture

```
CinemaStudio/
├── src/
│   ├── main.cpp                    # Application entry point
│   ├── MainWindow.cpp/h            # Main application window
│   ├── VideoPlayer.cpp/h           # Video preview player
│   ├── Timeline.cpp/h              # Multi-track timeline
│   ├── MediaLibrary.cpp/h          # Media asset browser
│   ├── Toolbar.cpp/h               # Editing tools toolbar
│   ├── EffectsPanel.cpp/h          # Effects browser
│   ├── FilterPanel.cpp/h           # Filter controls
│   ├── AudioMixer.cpp/h            # Audio mixing panel
│   ├── PropertiesPanel.cpp/h       # Clip properties
│   ├── TextTool.cpp/h              # Text overlay tool
│   ├── ExportDialog.cpp/h          # Export settings dialog
│   ├── UndoRedoManager.cpp/h       # Undo/redo system
│   │
│   ├── core/                       # Core data models
│   │   ├── Clip.cpp/h              # Clip data structure
│   │   ├── Track.cpp/h             # Track data structure
│   │   ├── ProjectFile.cpp/h       # Project management
│   │   ├── Effect.cpp/h            # Effect definitions
│   │   ├── Transition.cpp/h        # Transition definitions
│   │   ├── TextOverlay.cpp/h       # Text overlay data
│   │   └── AudioTrack.cpp/h        # Audio track data
│   │
│   ├── ffmpeg/                     # FFmpeg integration
│   │   ├── FFmpegEngine.cpp/h      # FFmpeg wrapper
│   │   ├── VideoDecoder.cpp/h      # Video decoding
│   │   ├── AudioDecoder.cpp/h      # Audio decoding
│   │   ├── VideoEncoder.cpp/h      # Video encoding
│   │   └── FilterGraph.cpp/h       # FFmpeg filters
│   │
│   └── utils/                      # Utility classes
│       ├── Utils.cpp/h             # Helper functions
│       ├── ThumbnailGenerator.cpp/h # Thumbnail generation
│       └── WaveformGenerator.cpp/h # Audio waveform generation
│
├── resources/
│   └── resources.qrc               # Qt resources
│
├── icons/                          # Application icons
├── CMakeLists.txt                  # CMake build configuration
└── build.sh                        # Build script
```

## Technology Stack

| Component | Technology |
|-----------|-----------|
| **Language** | C++17 |
| **UI Framework** | Qt 6 (Widgets) |
| **Video Engine** | FFmpeg (libavformat, libavcodec, libswscale, libswresample) |
| **Graphics** | OpenGL 3.3 Core Profile |
| **Build System** | CMake 3.16+ |
| **Platform** | Linux, Windows, macOS |

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config
sudo apt install -y qt6-base-dev qt6-multimedia-dev qt6-tools-dev
sudo apt install -y libavformat-dev libavcodec-dev libavutil-dev \
                    libswscale-dev libswresample-dev libavfilter-dev
```

### Fedora
```bash
sudo dnf install -y gcc-c++ cmake pkg-config
sudo dnf install -y qt6-qtbase-devel qt6-qtmultimedia-devel
sudo dnf install -y ffmpeg-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake
sudo pacman -S qt6-base qt6-multimedia
sudo pacman -S ffmpeg
```

### Windows (MSVC)
1. Install Visual Studio 2022
2. Install Qt 6 from https://www.qt.io/download
3. Install FFmpeg from https://ffmpeg.org/download.html
4. Use vcpkg to install dependencies:
   ```bash
   vcpkg install qt6-base qt6-multimedia ffmpeg
   ```

### macOS
```bash
brew install cmake pkg-config
brew install qt
brew install ffmpeg
```

## Building

### Quick Build (Linux)
```bash
chmod +x build.sh
./build.sh
```

### Manual Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Windows Build (MSVC)
```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Cross-compile to Windows .exe (from Linux)
```bash
sudo apt install mingw-w64
mkdir build-win && cd build-win
x86_64-w64-mingw32-cmake ..
make -j$(nproc)
# Output: CinemaStudio.exe
```

## Running

```bash
./build/CinemaStudio
```

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | New project |
| `Ctrl+O` | Open project |
| `Ctrl+S` | Save project |
| `Ctrl+E` | Export video |
| `Ctrl+I` | Import media |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` | Redo |
| `Ctrl+B` | Split at playhead |
| `V` | Select tool |
| `C` | Cut tool |
| `T` | Trim tool |
| `S` | Split tool |
| `H` | Hand tool |
| `Space` | Play/Pause |
| `+`/`-` | Zoom in/out |
| `F11` | Fullscreen |

## Project File Format

CinemaStudio uses `.csp` (CinemaStudio Project) files, which are JSON-based:

```json
{
  "name": "My Video",
  "resolution": { "width": 1920, "height": 1080 },
  "fps": 30.0,
  "tracks": [
    {
      "id": "...",
      "name": "Video 1",
      "type": 0,
      "visible": true,
      "locked": false,
      "clips": [
        {
          "id": "...",
          "filePath": "/path/to/video.mp4",
          "startTime": 0.0,
          "endTime": 10.5,
          "timelinePosition": 0.0,
          "volume": 1.0
        }
      ]
    }
  ],
  "effects": []
}
```

## Export Formats

| Format | Extension | Codec | Use Case |
|--------|-----------|-------|----------|
| MP4 | .mp4 | H.264/AAC | YouTube, social media |
| AVI | .avi | H.264/MP3 | Windows compatibility |
| MOV | .mov | H.264/AAC | Apple devices |
| MKV | .mkv | H.265/AAC | High quality, small size |
| WMV | .wmv | WMV/MP3 | Windows Media Player |

## Roadmap

### Phase 1 (Current) ✅
- [x] Core architecture
- [x] FFmpeg integration
- [x] Timeline UI
- [x] Basic editing tools
- [x] Project save/load

### Phase 2 (Next)
- [ ] GPU-accelerated rendering
- [ ] Real-time effects preview
- [ ] Advanced color grading
- [ ] Keyframe animations
- [ ] Chroma key (green screen)

### Phase 3
- [ ] Multi-cam editing
- [ ] Motion tracking
- [ ] Stabilization
- [ ] Speed ramping
- [ ] Audio ducking

### Phase 4
- [ ] Plugin system
- [ ] Scripting (Python/Lua)
- [ ] Cloud collaboration
- [ ] Template library
- [ ] Mobile companion app

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source and available under the MIT License.

## Credits

- **FFmpeg** - Video/audio processing
- **Qt Framework** - UI components
- **x264** - H.264 encoding
- **LAME** - MP3 encoding
- **Opus** - Audio codec

---

Built with ❤️ using C++ and Qt

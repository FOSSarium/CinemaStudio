cmake_minimum_required(VERSION 3.16)
project(CinemaStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets Network OpenGL OpenGLWidgets)

# Find FFmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED
    libavformat
    libavcodec
    libavutil
    libswscale
    libswresample
    libavfilter
)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/VideoPlayer.cpp
    src/Timeline.cpp
    src/TimelineTrack.cpp
    src/TimelineClip.cpp
    src/MediaLibrary.cpp
    src/Project.cpp
    src/ExportDialog.cpp
    src/EffectsPanel.cpp
    src/TextTool.cpp
    src/AudioMixer.cpp
    src/FilterPanel.cpp
    src/Toolbar.cpp
    src/PropertiesPanel.cpp
    src/ffmpeg/FFmpegEngine.cpp
    src/ffmpeg/VideoDecoder.cpp
    src/ffmpeg/AudioDecoder.cpp
    src/ffmpeg/VideoEncoder.cpp
    src/ffmpeg/FilterGraph.cpp
    src/core/Clip.cpp
    src/core/Track.cpp
    src/core/ProjectFile.cpp
    src/core/Effect.cpp
    src/core/Transition.cpp
    src/core/TextOverlay.cpp
    src/core/AudioTrack.cpp
    src/utils/Utils.cpp
    src/utils/ThumbnailGenerator.cpp
    src/utils/WaveformGenerator.cpp
)

set(HEADERS
    src/MainWindow.h
    src/VideoPlayer.h
    src/Timeline.h
    src/TimelineTrack.h
    src/TimelineClip.h
    src/MediaLibrary.h
    src/Project.h
    src/UndoRedoManager.h
    src/ExportDialog.h
    src/EffectsPanel.h
    src/TextTool.h
    src/AudioMixer.h
    src/FilterPanel.h
    src/Toolbar.h
    src/PropertiesPanel.h
    src/ffmpeg/FFmpegEngine.h
    src/ffmpeg/VideoDecoder.h
    src/ffmpeg/AudioDecoder.h
    src/ffmpeg/VideoEncoder.h
    src/ffmpeg/FilterGraph.h
    src/core/Clip.h
    src/core/Track.h
    src/core/ProjectFile.h
    src/core/Effect.h
    src/core/Transition.h
    src/core/TextOverlay.h
    src/core/AudioTrack.h
    src/utils/Utils.h
    src/utils/ThumbnailGenerator.h
    src/utils/WaveformGenerator.h
)

# Resource files
set(RESOURCES resources/resources.qrc)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${FFMPEG_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Network
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    ${FFMPEG_LIBRARIES}
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -O2
)

# Windows specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "CinemaStudio"
    )
    target_link_libraries(${PROJECT_NAME}
        ${FFMPEG_LIBRARIES}
    )
endif()

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

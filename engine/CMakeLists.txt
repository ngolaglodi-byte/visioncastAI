cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# --- Dependencies ---
# On Windows with vcpkg (classic mode), the opencv4 package installs its
# CMake config to share/opencv4/ — set OpenCV_DIR so find_package can locate it.
if(WIN32 AND NOT DEFINED OpenCV_DIR AND DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/opencv4"
        CACHE PATH "Path to OpenCVConfig.cmake (vcpkg hint)" FORCE)
endif()

find_package(OpenCV REQUIRED)
find_package(PkgConfig QUIET)

# ZeroMQ: try multiple detection strategies for cross-platform compatibility.
# 1. find_package(cppzmq)  — provided by cppzmq's CMake install (vcpkg / Homebrew cppzmq)
# 2. find_package(ZeroMQ)  — provided by libzmq's own CMake install
# 3. pkg-config            — Linux fallback
# 4. find_library          — last-resort path search (covers Homebrew without CMake configs)
find_package(cppzmq QUIET)
if(cppzmq_FOUND)
    set(ZMQ_FOUND TRUE)
    set(ZMQ_TARGET cppzmq)
    message(STATUS "VisionEngine: ZeroMQ found via find_package(cppzmq)")
else()
    find_package(ZeroMQ QUIET)
    if(ZeroMQ_FOUND)
        set(ZMQ_FOUND TRUE)
        set(ZMQ_TARGET libzmq)
        message(STATUS "VisionEngine: ZeroMQ found via find_package(ZeroMQ)")
    elseif(PkgConfig_FOUND)
        pkg_check_modules(ZMQ libzmq)
        if(ZMQ_FOUND)
            set(ZMQ_TARGET "")   # use ZMQ_LIBRARIES / ZMQ_INCLUDE_DIRS variables
            message(STATUS "VisionEngine: ZeroMQ found via pkg-config")
        endif()
    endif()

    if(NOT ZMQ_FOUND)
        # Last resort: direct library search (e.g. Homebrew without CMake config files)
        find_library(ZMQ_LIB NAMES zmq libzmq)
        find_path(ZMQ_INC NAMES zmq.h)
        if(ZMQ_LIB AND ZMQ_INC)
            set(ZMQ_FOUND TRUE)
            set(ZMQ_TARGET "")
            set(ZMQ_LIBRARIES "${ZMQ_LIB}")
            set(ZMQ_INCLUDE_DIRS "${ZMQ_INC}")
            message(STATUS "VisionEngine: ZeroMQ found via find_library: ${ZMQ_LIB}")
        else()
            message(STATUS "VisionEngine: ZeroMQ not found — ZMQ IPC will be disabled at runtime")
        endif()
    endif()
endif()

# FreeType: prefer find_package (CMake built-in + vcpkg), fall back to pkg-config
find_package(Freetype QUIET)
if(Freetype_FOUND)
    message(STATUS "VisionEngine: FreeType found via find_package")
elseif(PkgConfig_FOUND)
    pkg_check_modules(FREETYPE freetype2)
    if(FREETYPE_FOUND)
        message(STATUS "VisionEngine: FreeType found via pkg-config")
    else()
        message(STATUS "VisionEngine: FreeType not found — text rendering will be unavailable")
    endif()
else()
    message(STATUS "VisionEngine: FreeType not found — text rendering will be unavailable")
endif()

# FFmpeg (>= 6.0) — optional, enables hardware-accelerated encode/decode.
# Use pkg-config IMPORTED_TARGET (Linux/macOS) so the resulting CMake targets
# carry the correct library-search directories (e.g. /opt/homebrew/lib on macOS).
# Fall back to find_library for Windows / vcpkg where pkg-config is unavailable.
set(_ffmpeg_via_pkgconfig FALSE)
if(PkgConfig_FOUND)
    pkg_check_modules(AVCODEC  IMPORTED_TARGET libavcodec)
    pkg_check_modules(AVFORMAT IMPORTED_TARGET libavformat)
    pkg_check_modules(AVUTIL   IMPORTED_TARGET libavutil)
    pkg_check_modules(SWSCALE  IMPORTED_TARGET libswscale)
    if(AVCODEC_FOUND AND AVFORMAT_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND)
        set(_ffmpeg_via_pkgconfig TRUE)
    endif()
endif()

if(NOT _ffmpeg_via_pkgconfig)
    # Fallback: manual search — works with vcpkg-installed FFmpeg on Windows
    find_path(_ffmpeg_inc libavcodec/avcodec.h)
    find_library(_avcodec_lib  NAMES avcodec)
    find_library(_avformat_lib NAMES avformat)
    find_library(_avutil_lib   NAMES avutil)
    find_library(_swscale_lib  NAMES swscale)
    if(_ffmpeg_inc AND _avcodec_lib AND _avformat_lib AND _avutil_lib AND _swscale_lib)
        set(AVCODEC_FOUND  TRUE)
        set(AVFORMAT_FOUND TRUE)
        set(AVUTIL_FOUND   TRUE)
        set(SWSCALE_FOUND  TRUE)
        set(AVCODEC_LIBRARIES  ${_avcodec_lib})
        set(AVFORMAT_LIBRARIES ${_avformat_lib})
        set(AVUTIL_LIBRARIES   ${_avutil_lib})
        set(SWSCALE_LIBRARIES  ${_swscale_lib})
        # FFmpeg headers usually share one include root
        set(AVCODEC_INCLUDE_DIRS  ${_ffmpeg_inc})
        set(AVFORMAT_INCLUDE_DIRS ${_ffmpeg_inc})
        set(AVUTIL_INCLUDE_DIRS   ${_ffmpeg_inc})
        set(SWSCALE_INCLUDE_DIRS  ${_ffmpeg_inc})
        message(STATUS "VisionEngine: FFmpeg found via find_library")
    endif()
endif()

# nlohmann/json — prefer an installed system copy, fall back to FetchContent
find_package(nlohmann_json 3.11 QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found via find_package — fetching via FetchContent")
    FetchContent_Declare(
        nlohmann_json
        URL      https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Optional: OpenGL + GLFW + GLEW for GPU pipeline
find_package(OpenGL QUIET)
find_package(glfw3 3.3 QUIET)
find_package(GLEW QUIET)

# --- VisionCast Engine Library ---
set(ENGINE_SOURCES
    src/capture_manager.cpp
    src/pipeline_manager.cpp
    src/overlay_renderer.cpp
    src/filter_chain.cpp
    src/video_filters.cpp
    src/output_manager.cpp
    src/metadata_receiver.cpp
    src/zmq_receiver.cpp
    src/vision_engine.cpp
    src/gpu_compositor.cpp
    src/ffmpeg_rtmp.cpp
    src/multi_ffmpeg_rtmp_manager.cpp
)

add_library(visioncast_engine_lib STATIC ${ENGINE_SOURCES})

target_include_directories(visioncast_engine_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(visioncast_engine_lib
    PUBLIC
        visioncast_sdk
        ${OpenCV_LIBS}
)

if(ZMQ_FOUND)
    if(ZMQ_TARGET)
        # CMake target-based import (cppzmq or libzmq CMake target)
        target_link_libraries(visioncast_engine_lib PUBLIC ${ZMQ_TARGET})
    else()
        # pkg-config or find_library variables
        target_link_libraries(visioncast_engine_lib PUBLIC ${ZMQ_LIBRARIES})
        target_include_directories(visioncast_engine_lib PUBLIC ${ZMQ_INCLUDE_DIRS})
    endif()
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_ZMQ=1)
endif()

if(Freetype_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC Freetype::Freetype)
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_FREETYPE=1)
elseif(FREETYPE_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC ${FREETYPE_LIBRARIES})
    target_include_directories(visioncast_engine_lib PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_FREETYPE=1)
endif()

# nlohmann/json
target_link_libraries(visioncast_engine_lib PUBLIC nlohmann_json::nlohmann_json)

# FFmpeg (>= 6.0): libavcodec, libavformat, libavutil, libswscale
if(_ffmpeg_via_pkgconfig OR (AVCODEC_FOUND AND AVFORMAT_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND))
    if(_ffmpeg_via_pkgconfig)
        # Use the IMPORTED_TARGET targets that carry library-search dirs automatically
        target_link_libraries(visioncast_engine_lib PUBLIC
            PkgConfig::AVCODEC
            PkgConfig::AVFORMAT
            PkgConfig::AVUTIL
            PkgConfig::SWSCALE
        )
    else()
        target_link_libraries(visioncast_engine_lib PUBLIC
            ${AVCODEC_LIBRARIES}
            ${AVFORMAT_LIBRARIES}
            ${AVUTIL_LIBRARIES}
            ${SWSCALE_LIBRARIES}
        )
        target_include_directories(visioncast_engine_lib PUBLIC
            ${AVCODEC_INCLUDE_DIRS}
            ${AVFORMAT_INCLUDE_DIRS}
            ${AVUTIL_INCLUDE_DIRS}
            ${SWSCALE_INCLUDE_DIRS}
        )
    endif()
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_FFMPEG=1)
    message(STATUS "VisionEngine: FFmpeg enabled (avcodec, avformat, avutil, swscale)")
else()
    message(STATUS "VisionEngine: FFmpeg not found — hardware encode/decode unavailable")
endif()

# GPU pipeline: OpenGL + GLFW + GLEW
if(OpenGL_FOUND AND TARGET glfw AND GLEW_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC OpenGL::GL glfw GLEW::GLEW)
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_OPENGL=1)
    message(STATUS "VisionEngine: OpenGL GPU pipeline enabled")
else()
    message(STATUS "VisionEngine: OpenGL not found — CPU fallback (OpenCV highgui)")
endif()

# --- Engine Executable ---
add_executable(visioncast_engine src/main.cpp)
target_link_libraries(visioncast_engine PRIVATE visioncast_engine_lib)

# --- Install Rules ---
install(TARGETS visioncast_engine RUNTIME DESTINATION bin)
install(TARGETS visioncast_engine_lib ARCHIVE DESTINATION lib)
install(DIRECTORY include/visioncast DESTINATION include)

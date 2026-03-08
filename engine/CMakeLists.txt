cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# --- Dependencies ---
find_package(OpenCV REQUIRED)
find_package(PkgConfig QUIET)

# ZeroMQ: prefer find_package (ships with libzmq CMake install), fall back to
# pkg-config.  When neither is available the build continues without ZMQ and
# the runtime logs that ZMQ is unavailable.
find_package(ZeroMQ QUIET)
if(ZeroMQ_FOUND)
    set(ZMQ_FOUND TRUE)
    set(ZMQ_LIBRARIES libzmq)
    set(ZMQ_INCLUDE_DIRS "")   # target-based; handled via target_link_libraries
    message(STATUS "VisionEngine: ZeroMQ found via find_package")
elseif(PkgConfig_FOUND)
    pkg_check_modules(ZMQ libzmq)
    if(ZMQ_FOUND)
        message(STATUS "VisionEngine: ZeroMQ found via pkg-config")
    else()
        message(STATUS "VisionEngine: ZeroMQ not found — ZMQ IPC will be disabled at runtime")
    endif()
else()
    message(STATUS "VisionEngine: ZeroMQ not found — ZMQ IPC will be disabled at runtime")
endif()

if(PkgConfig_FOUND)
    pkg_check_modules(FREETYPE freetype2)
    # FFmpeg (>= 6.0) — optional, enables hardware-accelerated encode/decode
    pkg_check_modules(AVCODEC  libavcodec)
    pkg_check_modules(AVFORMAT libavformat)
    pkg_check_modules(AVUTIL   libavutil)
    pkg_check_modules(SWSCALE  libswscale)
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
    if(ZeroMQ_FOUND)
        # CMake target-based import (find_package(ZeroMQ))
        target_link_libraries(visioncast_engine_lib PUBLIC libzmq)
    else()
        # pkg-config variables
        target_link_libraries(visioncast_engine_lib PUBLIC ${ZMQ_LIBRARIES})
        target_include_directories(visioncast_engine_lib PUBLIC ${ZMQ_INCLUDE_DIRS})
    endif()
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_ZMQ=1)
endif()

if(FREETYPE_FOUND)
    target_link_libraries(visioncast_engine_lib PUBLIC ${FREETYPE_LIBRARIES})
    target_include_directories(visioncast_engine_lib PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_compile_definitions(visioncast_engine_lib PUBLIC HAS_FREETYPE=1)
endif()

# nlohmann/json
target_link_libraries(visioncast_engine_lib PUBLIC nlohmann_json::nlohmann_json)

# FFmpeg (>= 6.0): libavcodec, libavformat, libavutil, libswscale
if(AVCODEC_FOUND AND AVFORMAT_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND)
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

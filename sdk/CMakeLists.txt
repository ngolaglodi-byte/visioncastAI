cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# sdk_find_root(<out_var> <sdk_name>)
#
# Resolves the root directory for a proprietary SDK by checking two locations:
#   1. sdk/<sdk_name>/include  — legacy / side-by-side layout
#   2. sdk/third_party/<sdk_name>/include  — recommended drop-zone
#
# Sets <out_var> to the directory that *contains* the include/ sub-folder, or
# to an empty string when the SDK is not present.
# ---------------------------------------------------------------------------
macro(sdk_find_root _out _name)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}/include")
        set(${_out} "${CMAKE_CURRENT_SOURCE_DIR}/${_name}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/${_name}/include")
        set(${_out} "${CMAKE_CURRENT_SOURCE_DIR}/third_party/${_name}")
    else()
        set(${_out} "")
    endif()
endmacro()

# --- VisionCast SDK Abstraction Library ---
# Always-present sources (no proprietary SDK headers required)
set(SDK_SOURCES
    src/video_device.cpp
    src/sdk_logger.cpp
)

add_library(visioncast_sdk STATIC ${SDK_SOURCES})

target_include_directories(visioncast_sdk
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Optional: DeckLink SDK (Blackmagic Design Desktop Video)
# DeckLink is COM-based on Windows — no import library required, only headers.
sdk_find_root(_decklink_root decklink)
if(_decklink_root)
    target_sources(visioncast_sdk PRIVATE
        src/decklink_device.cpp
        src/decklink_input.cpp
        src/decklink_output.cpp)
    target_include_directories(visioncast_sdk PRIVATE
        ${_decklink_root}/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_DECKLINK=1)
    if(WIN32)
        # DeckLink uses Windows COM — link against ole32 and oleaut32
        target_link_libraries(visioncast_sdk PUBLIC ole32 oleaut32)
    endif()
    message(STATUS "VisionCast SDK: DeckLink SDK found at ${_decklink_root}")
else()
    message(STATUS "VisionCast SDK: DeckLink SDK not found — DeckLink support disabled")
endif()

# Optional: AJA NTV2 SDK
sdk_find_root(_aja_root aja)
if(_aja_root)
    target_sources(visioncast_sdk PRIVATE
        src/aja_device.cpp
        src/aja_input.cpp
        src/aja_output.cpp)
    target_include_directories(visioncast_sdk PRIVATE
        ${_aja_root}/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_AJA=1)
    if(EXISTS "${_aja_root}/lib")
        target_link_directories(visioncast_sdk PRIVATE
            ${_aja_root}/lib)
        target_link_libraries(visioncast_sdk PUBLIC ajantv2)
    endif()
    message(STATUS "VisionCast SDK: AJA NTV2 SDK found at ${_aja_root}")
else()
    message(STATUS "VisionCast SDK: AJA NTV2 SDK not found — AJA support disabled")
endif()

# Optional: Magewell SDK (Pro Capture)
sdk_find_root(_magewell_root magewell)
if(_magewell_root)
    target_sources(visioncast_sdk PRIVATE
        src/magewell_device.cpp
        src/magewell_input.cpp)
    target_include_directories(visioncast_sdk PRIVATE
        ${_magewell_root}/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_MAGEWELL=1)
    if(EXISTS "${_magewell_root}/lib")
        target_link_directories(visioncast_sdk PRIVATE
            ${_magewell_root}/lib)
        target_link_libraries(visioncast_sdk PUBLIC LibMWCapture)
    endif()
    if(WIN32)
        # Magewell uses Windows synchronisation primitives
        target_link_libraries(visioncast_sdk PUBLIC synchronization)
    endif()
    message(STATUS "VisionCast SDK: Magewell SDK found at ${_magewell_root}")
else()
    message(STATUS "VisionCast SDK: Magewell SDK not found — Magewell support disabled")
endif()

# Optional: NDI SDK (NewTek/Vizrt NDI Tools)
sdk_find_root(_ndi_root ndi)
if(_ndi_root)
    target_sources(visioncast_sdk PRIVATE
        src/ndi_device.cpp
        src/ndi_input.cpp
        src/ndi_output.cpp)
    target_include_directories(visioncast_sdk PRIVATE
        ${_ndi_root}/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_NDI=1)
    if(WIN32)
        if(EXISTS "${_ndi_root}/lib/x64")
            target_link_directories(visioncast_sdk PRIVATE
                ${_ndi_root}/lib/x64)
        endif()
        target_link_libraries(visioncast_sdk PUBLIC Processing.NDI.Lib.x64)
    endif()
    message(STATUS "VisionCast SDK: NDI SDK found at ${_ndi_root}")
else()
    message(STATUS "VisionCast SDK: NDI SDK not found — NDI support disabled")
endif()

# Optional: SRT (libsrt) + FFmpeg for encoding
sdk_find_root(_srt_root srt)
if(_srt_root)
    target_sources(visioncast_sdk PRIVATE src/srt_output.cpp)
    target_include_directories(visioncast_sdk PRIVATE
        ${_srt_root}/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_SRT=1)
    if(EXISTS "${_srt_root}/lib")
        target_link_directories(visioncast_sdk PRIVATE
            ${_srt_root}/lib)
    endif()
    target_link_libraries(visioncast_sdk PUBLIC srt)
    if(WIN32)
        target_link_libraries(visioncast_sdk PUBLIC ws2_32)
    endif()
    # FFmpeg is needed for the H.264 encoder used in SRTOutput::startPlayout()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(FFMPEG QUIET libavcodec libavutil)
        if(FFMPEG_FOUND)
            target_include_directories(visioncast_sdk PRIVATE ${FFMPEG_INCLUDE_DIRS})
            target_link_libraries(visioncast_sdk PUBLIC ${FFMPEG_LIBRARIES})
        endif()
    endif()
    message(STATUS "VisionCast SDK: SRT SDK found at ${_srt_root}")
else()
    message(STATUS "VisionCast SDK: SRT SDK not found — SRT output disabled")
endif()

# Optional: RTMP (via FFmpeg libavformat)
sdk_find_root(_rtmp_root rtmp)
if(_rtmp_root)
    target_sources(visioncast_sdk PRIVATE src/rtmp_output.cpp)
    target_include_directories(visioncast_sdk PRIVATE
        ${_rtmp_root}/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_RTMP=1)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(FFMPEG_RTMP QUIET libavformat libavcodec libavutil)
        if(FFMPEG_RTMP_FOUND)
            target_include_directories(visioncast_sdk PRIVATE ${FFMPEG_RTMP_INCLUDE_DIRS})
            target_link_libraries(visioncast_sdk PUBLIC ${FFMPEG_RTMP_LIBRARIES})
        endif()
    endif()
    message(STATUS "VisionCast SDK: RTMP root found at ${_rtmp_root}")
else()
    message(STATUS "VisionCast SDK: RTMP SDK not found — RTMP output disabled")
endif()

# --- Install Rules ---
install(TARGETS visioncast_sdk ARCHIVE DESTINATION lib)
install(DIRECTORY include/visioncast_sdk DESTINATION include)


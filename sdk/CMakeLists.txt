cmake_minimum_required(VERSION 3.20)

# --- VisionCast SDK Abstraction Library ---
set(SDK_SOURCES
    src/video_device.cpp
    src/sdk_logger.cpp
    src/decklink_device.cpp
    src/decklink_input.cpp
    src/decklink_output.cpp
    src/aja_device.cpp
    src/aja_input.cpp
    src/aja_output.cpp
    src/magewell_device.cpp
    src/magewell_input.cpp
    src/ndi_device.cpp
    src/ndi_input.cpp
    src/ndi_output.cpp
    src/srt_output.cpp
    src/rtmp_output.cpp
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
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/decklink/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/decklink/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_DECKLINK=1)
    if(WIN32)
        # DeckLink uses Windows COM — link against ole32 and oleaut32
        target_link_libraries(visioncast_sdk PUBLIC ole32 oleaut32)
    endif()
endif()

# Optional: AJA NTV2 SDK
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/aja/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/aja/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_AJA=1)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/aja/lib")
        target_link_directories(visioncast_sdk PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/aja/lib)
        target_link_libraries(visioncast_sdk PUBLIC ajantv2)
    endif()
endif()

# Optional: Magewell SDK (Pro Capture)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/magewell/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/magewell/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_MAGEWELL=1)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/magewell/lib")
        target_link_directories(visioncast_sdk PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/magewell/lib)
        target_link_libraries(visioncast_sdk PUBLIC LibMWCapture)
    endif()
    if(WIN32)
        # Magewell uses Windows synchronisation primitives
        target_link_libraries(visioncast_sdk PUBLIC synchronization)
    endif()
endif()

# Optional: NDI SDK (NewTek/Vizrt NDI Tools)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ndi/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ndi/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_NDI=1)
    if(WIN32)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ndi/lib/x64")
            target_link_directories(visioncast_sdk PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/ndi/lib/x64)
        endif()
        target_link_libraries(visioncast_sdk PUBLIC Processing.NDI.Lib.x64)
    endif()
endif()

# Optional: SRT (libsrt) + FFmpeg for encoding
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/srt/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/srt/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_SRT=1)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/srt/lib")
        target_link_directories(visioncast_sdk PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/srt/lib)
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
endif()

# Optional: RTMP (via FFmpeg libavformat)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rtmp/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/rtmp/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_RTMP=1)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(FFMPEG_RTMP QUIET libavformat libavcodec libavutil)
        if(FFMPEG_RTMP_FOUND)
            target_include_directories(visioncast_sdk PRIVATE ${FFMPEG_RTMP_INCLUDE_DIRS})
            target_link_libraries(visioncast_sdk PUBLIC ${FFMPEG_RTMP_LIBRARIES})
        endif()
    endif()
endif()

# --- Install Rules ---
install(TARGETS visioncast_sdk ARCHIVE DESTINATION lib)
install(DIRECTORY include/visioncast_sdk DESTINATION include)

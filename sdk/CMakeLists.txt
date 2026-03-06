cmake_minimum_required(VERSION 3.20)

# --- VisionCast SDK Abstraction Library ---
set(SDK_SOURCES
    src/video_device.cpp
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
)

add_library(visioncast_sdk STATIC ${SDK_SOURCES})

target_include_directories(visioncast_sdk
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optional: DeckLink SDK
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/decklink/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/decklink/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_DECKLINK=1)
endif()

# Optional: AJA NTV2 SDK
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/aja/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/aja/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_AJA=1)
endif()

# Optional: Magewell SDK
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/magewell/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/magewell/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_MAGEWELL=1)
endif()

# Optional: NDI SDK
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ndi/include")
    target_include_directories(visioncast_sdk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ndi/include)
    target_compile_definitions(visioncast_sdk PUBLIC HAS_NDI=1)
endif()

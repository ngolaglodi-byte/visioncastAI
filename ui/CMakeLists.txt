cmake_minimum_required(VERSION 3.20)

# --- Qt Configuration ---
find_package(Qt6 COMPONENTS Widgets Core Gui Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Core Gui Network QUIET)
endif()

if(NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(STATUS "Qt not found - skipping UI build")
    return()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- VisionCast UI Executable ---
set(UI_SOURCES
    src/main.cpp
    src/main_window.cpp
    src/preview_panel.cpp
    src/overlay_editor.cpp
    src/talent_manager.cpp
    src/output_config.cpp
    src/monitoring_panel.cpp
    src/control_room.cpp
    src/design_panel.cpp
    src/recognition_panel.cpp
    src/device_scanner.cpp
    src/license_manager.cpp
    src/license_dialog.cpp
    src/license_config.cpp
    src/license_secure_logger.cpp
    src/license_storage.cpp
)

add_executable(visioncast_ui ${UI_SOURCES})

target_include_directories(visioncast_ui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/sdk/include
)

if(Qt6_FOUND)
    target_link_libraries(visioncast_ui PRIVATE
        Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Network
        visioncast_engine_lib
        visioncast_sdk
    )
else()
    target_link_libraries(visioncast_ui PRIVATE
        Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network
        visioncast_engine_lib
        visioncast_sdk
    )
endif()

# --- License API defaults (can be overridden at build time) ---
# Usage: cmake -DVISIONCAST_LICENSE_API_URL="https://..." -DVISIONCAST_LICENSE_API_KEY="..."
if(DEFINED VISIONCAST_LICENSE_API_URL)
    target_compile_definitions(visioncast_ui PRIVATE
        VISIONCAST_LICENSE_API_URL="${VISIONCAST_LICENSE_API_URL}")
endif()
if(DEFINED VISIONCAST_LICENSE_API_KEY)
    target_compile_definitions(visioncast_ui PRIVATE
        VISIONCAST_LICENSE_API_KEY="${VISIONCAST_LICENSE_API_KEY}")
endif()

# --- Install Rules ---
install(TARGETS visioncast_ui RUNTIME DESTINATION bin)

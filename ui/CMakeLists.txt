cmake_minimum_required(VERSION 3.20)

# --- Qt Configuration ---
find_package(Qt6 COMPONENTS Widgets Core Gui Network Quick QuickControls2 QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Core Gui Network Quick QuickControls2 QUIET)
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
    src/python_launcher.cpp
)

# --- Windows: embed icon into the executable via .rc (only if the .ico exists) ---
if(WIN32)
    set(_ico_file ${CMAKE_SOURCE_DIR}/packaging/icons/visioncast.ico)
    set(_rc_file  ${CMAKE_SOURCE_DIR}/packaging/icons/visioncast.rc)
    if(EXISTS ${_ico_file} AND EXISTS ${_rc_file})
        list(APPEND UI_SOURCES ${_rc_file})
    else()
        message(STATUS "VisionCast UI: visioncast.ico not found — skipping icon embedding "
                       "(generate it from packaging/icons/visioncast.svg; see packaging/icons/README.md)")
    endif()
endif()

add_executable(visioncast_ui ${UI_SOURCES})

# --- macOS: attach .icns to the application bundle ---
if(APPLE)
    set(APP_ICON_MACOS ${CMAKE_SOURCE_DIR}/packaging/icons/visioncast.icns)
    if(EXISTS ${APP_ICON_MACOS})
        set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(visioncast_ui PRIVATE ${APP_ICON_MACOS})
    endif()
    set_target_properties(visioncast_ui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE visioncast.icns
    )
endif()

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
if(APPLE)
    install(TARGETS visioncast_ui
        BUNDLE DESTINATION .
    )
else()
    install(TARGETS visioncast_ui
        RUNTIME DESTINATION bin
    )
endif()

# ======================================================================
# QML-based executable (visioncast_ui_qml)
# ======================================================================

# Only build the QML target when Qt Quick is available
set(_have_quick FALSE)
if(Qt6_FOUND AND TARGET Qt6::Quick AND TARGET Qt6::QuickControls2)
    set(_have_quick TRUE)
elseif(Qt5_FOUND AND TARGET Qt5::Quick AND TARGET Qt5::QuickControls2)
    set(_have_quick TRUE)
endif()

if(_have_quick)
    # QML resource file
    if(Qt6_FOUND)
        qt_add_resources(QML_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/qml/qml.qrc)
    else()
        qt5_add_resources(QML_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/qml/qml.qrc)
    endif()

    # Shared backend sources (everything except main.cpp and Widget panel files)
    set(QML_UI_SOURCES
        src/main_qml.cpp
        src/qml_bridge.cpp
        src/device_scanner.cpp
        src/license_manager.cpp
        src/license_dialog.cpp
        src/license_config.cpp
        src/license_secure_logger.cpp
        src/license_storage.cpp
        src/python_launcher.cpp
        ${QML_RESOURCES}
    )

    # Windows icon embedding (reuse the same .rc as the Widgets target)
    if(WIN32)
        set(_ico_file ${CMAKE_SOURCE_DIR}/packaging/icons/visioncast.ico)
        set(_rc_file  ${CMAKE_SOURCE_DIR}/packaging/icons/visioncast.rc)
        if(EXISTS ${_ico_file} AND EXISTS ${_rc_file})
            list(APPEND QML_UI_SOURCES ${_rc_file})
        endif()
    endif()

    add_executable(visioncast_ui_qml ${QML_UI_SOURCES})

    # macOS bundle
    if(APPLE)
        set(APP_ICON_MACOS ${CMAKE_SOURCE_DIR}/packaging/icons/visioncast.icns)
        if(EXISTS ${APP_ICON_MACOS})
            set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
            target_sources(visioncast_ui_qml PRIVATE ${APP_ICON_MACOS})
        endif()
        set_target_properties(visioncast_ui_qml PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_ICON_FILE visioncast.icns
        )
    endif()

    target_include_directories(visioncast_ui_qml
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/sdk/include
    )

    if(Qt6_FOUND)
        target_link_libraries(visioncast_ui_qml PRIVATE
            Qt6::Core Qt6::Gui Qt6::Network Qt6::Quick Qt6::QuickControls2
            visioncast_engine_lib
            visioncast_sdk
        )
    else()
        target_link_libraries(visioncast_ui_qml PRIVATE
            Qt5::Core Qt5::Gui Qt5::Network Qt5::Quick Qt5::QuickControls2
            visioncast_engine_lib
            visioncast_sdk
        )
    endif()

    # License API definitions (same as Widgets target)
    if(DEFINED VISIONCAST_LICENSE_API_URL)
        target_compile_definitions(visioncast_ui_qml PRIVATE
            VISIONCAST_LICENSE_API_URL="${VISIONCAST_LICENSE_API_URL}")
    endif()
    if(DEFINED VISIONCAST_LICENSE_API_KEY)
        target_compile_definitions(visioncast_ui_qml PRIVATE
            VISIONCAST_LICENSE_API_KEY="${VISIONCAST_LICENSE_API_KEY}")
    endif()

    # Install
    if(APPLE)
        install(TARGETS visioncast_ui_qml BUNDLE DESTINATION .)
    else()
        install(TARGETS visioncast_ui_qml RUNTIME DESTINATION bin)
    endif()

    message(STATUS "VisionCast QML UI target enabled (visioncast_ui_qml)")
else()
    message(STATUS "Qt Quick / QuickControls2 not found — skipping visioncast_ui_qml target")
endif()

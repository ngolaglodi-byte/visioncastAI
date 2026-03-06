cmake_minimum_required(VERSION 3.20)
project(VisionCast VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Sub-projects
add_subdirectory(sdk)
add_subdirectory(engine)
add_subdirectory(ui)

# ============================================================
# Install rules for data files
# ============================================================
install(DIRECTORY python/         DESTINATION python
    PATTERN "__pycache__" EXCLUDE
    PATTERN "*.pyc"       EXCLUDE
    PATTERN ".pytest_cache" EXCLUDE
    PATTERN "tests"       EXCLUDE)
install(DIRECTORY config/         DESTINATION config)
install(DIRECTORY overlays/       DESTINATION overlays)
install(DIRECTORY talents/        DESTINATION talents)
install(DIRECTORY docs/           DESTINATION docs)
install(FILES     README.md       DESTINATION .)

# ============================================================
# CPack – cross-platform installer generation
# ============================================================
set(CPACK_PACKAGE_NAME "VisionCast")
set(CPACK_PACKAGE_VENDOR "VisionCast-AI")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "AI-powered broadcast production suite with real-time face recognition")
set(CPACK_RESOURCE_FILE_README  "${CMAKE_SOURCE_DIR}/README.md")

# ---------- Windows (NSIS) ----------
set(CPACK_NSIS_DISPLAY_NAME "VisionCast ${PROJECT_VERSION}")
set(CPACK_NSIS_PACKAGE_NAME "VisionCast")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/ngolaglodi-byte/visioncast-AI")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\VisionCast UI.lnk' '$INSTDIR\\\\bin\\\\visioncast_ui.exe'
     CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\VisionCast Engine.lnk' '$INSTDIR\\\\bin\\\\visioncast_engine.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\VisionCast UI.lnk'
     Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\VisionCast Engine.lnk'")

# ---------- macOS (DragNDrop / productbuild) ----------
set(CPACK_DMG_VOLUME_NAME "VisionCast ${PROJECT_VERSION}")
set(CPACK_DMG_FORMAT "UDBZ")
set(CPACK_PRODUCTBUILD_IDENTITY_NAME "VisionCast")

# ---------- Generator selection per platform ----------
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;productbuild")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

# ===========================================================================
# src/CMakeLists.txt
#
# Builds two targets:
#   * lumine_lib — a static library with all application logic. Keeping the
#     logic in a library lets the test suite link the exact same code.
#   * lumine     — the thin executable that wires the library to an entry
#     point.
# ===========================================================================

# ---------------------------------------------------------------------------
# Generated version header (core/Version.hpp) from the .in template.
# ---------------------------------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/core/Version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/core/Version.hpp"
    @ONLY)

# ---------------------------------------------------------------------------
# lumine_lib — application logic, UI, rendering and platform code.
# ---------------------------------------------------------------------------
add_library(lumine_lib STATIC
    core/ImageCache.cpp
    core/ImageCache.hpp
    core/Logging.cpp
    core/Logging.hpp
    core/RecentFiles.cpp
    core/RecentFiles.hpp
    image/AsyncImageLoader.cpp
    image/AsyncImageLoader.hpp
    image/ImageInfo.cpp
    image/ImageInfo.hpp
    image/ImageList.cpp
    image/ImageList.hpp
    image/ImageLoader.cpp
    image/ImageLoader.hpp
    image/ThumbnailLoader.cpp
    image/ThumbnailLoader.hpp
    platform/PlatformInfo.cpp
    platform/PlatformInfo.hpp
    rendering/ImageView.cpp
    rendering/ImageView.hpp
    rendering/ZoomMath.hpp
    ui/InfoPanel.cpp
    ui/InfoPanel.hpp
    ui/MainWindow.cpp
    ui/MainWindow.hpp
    ui/ThumbnailStrip.cpp
    ui/ThumbnailStrip.hpp)

# Qt classes deriving from QObject need the meta-object compiler.
set_target_properties(lumine_lib PROPERTIES AUTOMOC ON)

# Source tree for "module/Header.hpp" includes; binary tree for the
# generated core/Version.hpp.
target_include_directories(lumine_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(lumine_lib
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
    PRIVATE
        lumine_warnings)

# ---------------------------------------------------------------------------
# lumine — the executable entry point.
# ---------------------------------------------------------------------------
qt_add_executable(lumine
    app/main.cpp)

target_link_libraries(lumine
    PRIVATE
        lumine_lib
        lumine_warnings)

# ---------------------------------------------------------------------------
# Installation — binary plus Wayland desktop integration files.
# ---------------------------------------------------------------------------
install(TARGETS lumine
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES "${CMAKE_SOURCE_DIR}/assets/io.github.lumine.Lumine.desktop"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")

install(FILES "${CMAKE_SOURCE_DIR}/assets/icons/io.github.lumine.Lumine.svg"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps")

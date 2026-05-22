# ===========================================================================
# Lumine — a fast, modern image viewer for Wayland.
# Top-level build configuration.
# ===========================================================================
cmake_minimum_required(VERSION 3.21)

project(Lumine
    VERSION 0.1.0
    DESCRIPTION "A fast, modern image viewer for Wayland"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global C++ settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to an optimised build when the developer did not pick one.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release"
        CACHE STRING "Build type (Debug, Release, RelWithDebInfo)" FORCE)
endif()

# Emit compile_commands.json for clang-tidy, clangd and IDE integration.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(LUMINE_BUILD_TESTS "Build the Lumine test suite" ON)
option(LUMINE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Compiler warning policy — defines the INTERFACE target `lumine_warnings`.
# Included after the options above so they can influence the flag set.
include(LumineWarnings)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
add_subdirectory(src)

if(LUMINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------
# Configuration summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "Lumine ${PROJECT_VERSION} configured")
message(STATUS "  Build type        : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Qt version        : ${Qt6_VERSION}")
message(STATUS "  Build tests       : ${LUMINE_BUILD_TESTS}")
message(STATUS "  Warnings as errors: ${LUMINE_WARNINGS_AS_ERRORS}")
message(STATUS "")

# ===========================================================================
# tests/CMakeLists.txt
#
# Builds the Catch2-based test runner. Catch2 is taken from the system when
# available and fetched from source otherwise, so a fresh checkout still
# builds its tests as long as the network is reachable.
# ===========================================================================

find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
    message(STATUS "Catch2 not found locally — fetching v3.5.4 via FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(lumine_tests
    test_async_image_loader.cpp
    test_image_cache.cpp
    test_image_list.cpp
    test_image_loader.cpp
    test_platform_info.cpp
    test_zoom_math.cpp)

target_link_libraries(lumine_tests
    PRIVATE
        lumine_lib
        lumine_warnings
        Catch2::Catch2WithMain)

add_test(NAME lumine_tests COMMAND lumine_tests)

# Keep the test runner headless and deterministic, even under a Wayland
# session, in case future tests construct a QGuiApplication.
set_tests_properties(lumine_tests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

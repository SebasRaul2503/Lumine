# ===========================================================================
# LumineWarnings.cmake
#
# Defines the INTERFACE target `lumine_warnings`, which carries Lumine's
# compiler warning policy. Link it PRIVATE-ly into first-party targets only,
# so third-party code (Qt, Catch2) is never held to these flags.
# ===========================================================================

add_library(lumine_warnings INTERFACE)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lumine_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wcast-align
        -Wunused
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2)
    if(LUMINE_WARNINGS_AS_ERRORS)
        target_compile_options(lumine_warnings INTERFACE -Werror)
    endif()
elseif(MSVC)
    target_compile_options(lumine_warnings INTERFACE /W4)
    if(LUMINE_WARNINGS_AS_ERRORS)
        target_compile_options(lumine_warnings INTERFACE /WX)
    endif()
endif()

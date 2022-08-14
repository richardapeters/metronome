cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(EMIL_ENABLE_DOCKER_TOOLS false)

include(FetchContent)

FetchContent_Declare(
    emil
    GIT_REPOSITORY https://github.com/philips-software/embeddedinfralib.git
    GIT_TAG        9ecc7209c6b04b993f71d0bf22f398a44c23461e
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/emil
)

FetchContent_Declare(
    preview
    GIT_REPOSITORY https://github.com/philips-internal/amp-preview.git
    GIT_TAG        11b9f1b3e8436c8d13c31524c9e32099ef7d7a23
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/preview
)

FetchContent_Declare(
    halst
    GIT_REPOSITORY https://github.com/philips-internal/amp-hal-st.git
    GIT_TAG        dc6c0fcb0810b30219eb865e7fef2b8cb7c239fe
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/halst
)

FetchContent_MakeAvailable(emil preview halst)

add_subdirectory(local_hal)
add_subdirectory(metronome)

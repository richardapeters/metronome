cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include(FetchContent)

FetchContent_Declare(
    emil
    GIT_REPOSITORY https://github.com/philips-software/embeddedinfralib.git
    GIT_TAG        e50e7a11524b8d87aa5073183c642108b7ee081f
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/emil
)

FetchContent_Declare(
    preview
    GIT_REPOSITORY https://github.com/philips-internal/amp-preview.git
    GIT_TAG        932d60d79c25125f1b49e4752cae8594b721d18e
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/preview
)

FetchContent_Declare(
    halst
    GIT_REPOSITORY https://github.com/philips-internal/amp-hal-st.git
    GIT_TAG        5618b6eae9851bcde91b0b664e832825a3e2c147
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/halst
)

FetchContent_MakeAvailable(emil preview halst)

add_subdirectory(local_hal)
add_subdirectory(metronome)

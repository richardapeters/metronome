cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include(FetchContent)

FetchContent_Declare(
    emil
    GIT_REPOSITORY https://github.com/philips-software/embeddedinfralib.git
    GIT_TAG        5085134881231c1010b076616c9674c0cead2218
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/emil
)

FetchContent_Declare(
    preview
    GIT_REPOSITORY https://github.com/philips-internal/amp-preview.git
    GIT_TAG        bd39f1635fc01cb04036080561b099bb777b6da8
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/preview
)

FetchContent_Declare(
    halst
    GIT_REPOSITORY https://github.com/philips-internal/amp-hal-st.git
    GIT_TAG        f925e313e9f154aedfb058ab5bde133c1e3022ed
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/halst
)

FetchContent_MakeAvailable(emil preview halst)

add_subdirectory(local_hal)
add_subdirectory(metronome)

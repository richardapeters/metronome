cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(EMIL_ENABLE_DOCKER_TOOLS false)

include(FetchContent)

FetchContent_Declare(
    emil
    GIT_REPOSITORY https://github.com/philips-software/embeddedinfralib.git
    GIT_TAG        7881492bb5c601d9f446023e2f00b0d84d964743
    SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/Deps/emil
)

FetchContent_Declare(
    preview
    GIT_REPOSITORY https://github.com/philips-internal/amp-preview.git
    GIT_TAG        be008cd527d32c04a679b00cc01e3dec39940a06
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

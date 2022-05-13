cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include(FetchContent)

FetchContent_Declare(
    emil
    GIT_REPOSITORY https://github.com/philips-software/embeddedinfralib.git
    GIT_TAG        a83e3e3c6f3a8e7508d2c66f234a3f89e2c9f21f
)

FetchContent_Declare(
    preview
    GIT_REPOSITORY https://github.com/philips-internal/amp-preview.git
    GIT_TAG        cdc3efd770a106c28c52272b5d656f31520e15b5
)

FetchContent_Declare(
    halst
    GIT_REPOSITORY https://github.com/philips-internal/amp-hal-st.git
    GIT_TAG        5618b6eae9851bcde91b0b664e832825a3e2c147
)

FetchContent_MakeAvailable(emil preview halst)

add_subdirectory(local_hal)
add_subdirectory(metronome)

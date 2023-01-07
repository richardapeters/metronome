cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(EMIL_ENABLE_DOCKER_TOOLS false)
set(EMIL_BUILD_TESTS true)

add_subdirectory(EmbeddedInfraLib)
add_subdirectory(Preview)
add_subdirectory(HalSt)

add_subdirectory(local_hal)
add_subdirectory(metronome)

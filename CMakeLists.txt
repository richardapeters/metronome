cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(EMIL_ENABLE_DOCKER_TOOLS false)
enable_testing()

set(EMIL_BUILD_EXAMPLES true)
set(EMIL_INCLUDE_THREADX true)

add_subdirectory(amp-embedded-infra-lib)
add_subdirectory(amp-hal-st)
add_subdirectory(amp-preview)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Suppress warnings about parameter passing for argument of type '...' changed in GCC 7.1
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-psabi>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-psabi>)
endif()

add_subdirectory(local_hal)
add_subdirectory(metronome)

emil_folderize_all_targets()
emil_generate_antora_docs_target(antora-playbook.yml)
emil_clangformat_directories(metronome DIRECTORIES local_hal metronome)

cmake_minimum_required(VERSION 3.21)

project(Metronome)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(EMIL_ENABLE_DOCKER_TOOLS false)
enable_testing()

set(emil_POPULATED TRUE)
add_subdirectory(EmbeddedInfraLib)
add_subdirectory(Preview)
add_subdirectory(HalSt)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Suppress warnings about parameter passing for argument of type '...' changed in GCC 7.1
    add_compile_options(-Wno-psabi)
endif()

add_subdirectory(local_hal)
add_subdirectory(metronome)

emil_folderize_all_targets()
emil_generate_antora_docs_target(antora-playbook.yml)
emil_clangformat_directories(metronome DIRECTORIES local_hal metronome)

cmake_minimum_required(VERSION 3.2.3)

project(Metronome)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include("${CCOLA_DIR}/ccola.inc")

ccola_programmer(
    cubeide-j-link
)

ccola_project(Train)

    ccola_subdirectories(
        EmbeddedInfraLib/external/googletest
        EmbeddedInfraLib/external/crypto
        EmbeddedInfraLib/infra
        EmbeddedInfraLib/hal
        EmbeddedInfraLib/services
        HalSt/hal_st
        Preview/external/SDL2-2.0.5
        Preview/preview
        local_hal
        metronome
    )

ccola_end_project()

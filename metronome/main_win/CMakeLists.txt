if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)

    add_executable(metronome.main_win WIN32)

    target_include_directories(metronome.main_win PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    target_sources(metronome.main_win PRIVATE
        DoubleBufferDisplayAdaptedFromDirectDisplay.cpp
        DoubleBufferDisplayAdaptedFromDirectDisplay.hpp
        MultiBufferDisplayAdaptedFromDirectDisplay.cpp
        MultiBufferDisplayAdaptedFromDirectDisplay.hpp
        Main.cpp
    )

    target_link_libraries(metronome.main_win PUBLIC
        metronome.instantiations
        hal.generic
        preview.sdl
    )

endif()

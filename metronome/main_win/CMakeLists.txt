if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)

    add_executable(metronome.main_win WIN32)

    target_sources(metronome.main_win PRIVATE
        DoubleBufferDisplayAdaptedFromDirectDisplay.cpp
        DoubleBufferDisplayAdaptedFromDirectDisplay.hpp
        Main.cpp
    )

    target_link_libraries(metronome.main_win PUBLIC
        metronome.application
        hal.generic
        preview.sdl
    )

endif()

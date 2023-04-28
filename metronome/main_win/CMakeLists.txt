add_executable(metronome.main_win WIN32)
emil_build_for(metronome.main_win HOST Windows)

target_compile_definitions(metronome.main_win PUBLIC NOMINMAX)

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

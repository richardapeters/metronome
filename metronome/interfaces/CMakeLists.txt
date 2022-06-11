add_library(metronome.interfaces INTERFACE)

target_include_directories(metronome.interfaces INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(metronome.interfaces INTERFACE
    BeatController.hpp
    BeatTimer.hpp
    BpmSelection.hpp
    MetronomePainter.hpp
    Notes.hpp
)

target_link_libraries(metronome.interfaces INTERFACE
    infra.util
)

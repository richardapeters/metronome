add_library(metronome.instantiations STATIC)

target_include_directories(metronome.instantiations PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(metronome.instantiations PRIVATE
    Metronome.cpp
    Metronome.hpp
)

target_link_libraries(metronome.instantiations PUBLIC
    metronome.application
    metronome.views
)

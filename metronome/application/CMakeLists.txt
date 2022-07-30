add_library(metronome.application STATIC)

target_include_directories(metronome.application PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(metronome.application PRIVATE
    BeatControllerImpl.cpp
    BeatControllerImpl.hpp
    BpmSelectionInteractor.cpp
    BpmSelectionInteractor.hpp
    Ds3231.cpp
    Ds3231.hpp
    BeatPainter.cpp
    BeatPainter.hpp
    NotesMidi.cpp
    NotesMidi.hpp
    StuckI2cReleaser.cpp
    StuckI2cReleaser.hpp
    TouchFt5x06.cpp
    TouchFt5x06.hpp
    Wm8994.cpp
    Wm8994.hpp
)

target_link_libraries(metronome.application PUBLIC
    services.util
    metronome.interfaces
    preview.touch
)

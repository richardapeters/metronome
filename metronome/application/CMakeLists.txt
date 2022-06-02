add_library(metronome.application STATIC)

target_include_directories(metronome.application PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(metronome.application PRIVATE
    BeatController.cpp
    BeatController.hpp
    BpmSelectionInteractor.cpp
    BpmSelectionInteractor.hpp
    Ds3231.cpp
    Ds3231.hpp
    Metronome.cpp
    Metronome.hpp
    NoteFont.cpp
    NoteFont.hpp
    NotesMidi.cpp
    NotesMidi.hpp
    StuckI2cReleaser.cpp
    StuckI2cReleaser.hpp
    TouchFt5x06.cpp
    TouchFt5x06.hpp
    ViewBeatsPerMeasure.cpp
    ViewBeatsPerMeasure.hpp
    ViewBpm.cpp
    ViewBpm.hpp
    ViewDateEntry.cpp
    ViewDateEntry.hpp
    ViewNoteKind.cpp
    ViewNoteKind.hpp
    ViewPainterMetronome.cpp
    ViewPainterMetronome.hpp
    ViewSprocket.cpp
    ViewSprocket.hpp
    ViewStart.cpp
    ViewStart.hpp
    ViewTapMeasurement.cpp
    ViewTapMeasurement.hpp
    Wm8994.cpp
    Wm8994.hpp
)

target_link_libraries(metronome.application PUBLIC
    preview.fonts
    preview.touch
    services.util
)

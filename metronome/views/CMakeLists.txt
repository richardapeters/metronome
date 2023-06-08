add_library(metronome.views STATIC)

target_include_directories(metronome.views PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(metronome.views PRIVATE
    NoteFont.cpp
    NoteFont.hpp
    ViewBeatsPerMeasure.cpp
    ViewBeatsPerMeasure.hpp
    ViewBpm.cpp
    ViewBpm.hpp
    ViewDateEntry.cpp
    ViewDateEntry.hpp
    ViewGap.cpp
    ViewGap.hpp
    ViewNoteKind.cpp
    ViewNoteKind.hpp
    ViewSprocket.cpp
    ViewSprocket.hpp
    ViewStart.cpp
    ViewStart.hpp
    ViewTapMeasurement.cpp
    ViewTapMeasurement.hpp
    ViewTimeline.cpp
    ViewTimeline.hpp
)

target_link_libraries(metronome.views PUBLIC
    preview.fonts
    preview.touch
)

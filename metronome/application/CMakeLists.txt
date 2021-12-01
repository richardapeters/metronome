ccola_component(application static_library)

    ccola_sources(
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

    ccola_dependencies(
        preview.fonts
        preview.touch
        services.util
    )

ccola_end_component()

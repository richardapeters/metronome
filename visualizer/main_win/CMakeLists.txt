ccola_component(main_win win: executable other: noncompile)

    ccola_sources(
        Main.cpp
    )

    ccola_dependencies(
        hal.generic
        preview.sdl
    )

    set(CCOLA_COMPONENT_SUBSYSTEM WIN32)

ccola_end_component()

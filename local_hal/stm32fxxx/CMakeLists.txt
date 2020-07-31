ccola_component(stm32fxxx stm32f0xx: static_library stm32f2xx: static_library stm32f3xx: static_library stm32f4xx: static_library stm32f7xx: static_library other: noncompile)

    ccola_inheritable_include_directories(../..)

    ccola_sources(
        SaiStm.cpp
        SaiStm.hpp
    )

    ccola_dependencies(
        hal_st.stm32fxxx
        preview.stm32fxxx
    )

ccola_end_component()

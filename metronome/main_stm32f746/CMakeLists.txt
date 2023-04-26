if (TARGET_MCU STREQUAL stm32f746)

    add_executable(metronome.main_stm32f746)

    target_include_directories(metronome.main_stm32f746 PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    target_sources(metronome.main_stm32f746 PRIVATE
        Flash.cpp
        Flash.hpp
        Lcd.cpp
        Lcd.hpp
        LowPowerTimer.cpp
        LowPowerTimer.hpp
        Main.cpp
        MetronomeBeatTimerStm.cpp
        MetronomeBeatTimerStm.hpp
        PeripheralI2c.hpp
        Sai.cpp
        Sai.hpp
        SdStm.cpp
        SdStm.hpp
        Touch.cpp
        Touch.hpp
        Tracer.cpp
        Tracer.hpp
    )

    emil_target_range_source(metronome.main_stm32f746 ClickAccent.wav clickAccent clicks ClickAccent)
    emil_target_range_source(metronome.main_stm32f746 Click.wav click clicks Click)

    target_link_libraries(metronome.main_stm32f746 PUBLIC
        metronome.instantiations
        hal_st.stm32fxxx
        hal_st.synchronous_stm32fxxx
        services.tracer
        local_hal.stm32fxxx
        m
    )

    halst_target_default_linker_scripts(metronome.main_stm32f746)
    halst_target_default_init(metronome.main_stm32f746)

    emil_generate_artifacts(TARGET metronome.main_stm32f746 LST MAP BIN HEX)

endif()

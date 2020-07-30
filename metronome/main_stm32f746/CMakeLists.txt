ccola_component(main_stm32f746 stm32f746: executable other: noncompile)

    ccola_sources(
        Flash.cpp
        Flash.hpp
        Lcd.cpp
        Lcd.hpp
        LowPowerTimer.cpp
        LowPowerTimer.hpp
        Main.cpp
        PeripheralI2c.hpp
        Touch.cpp
        Touch.hpp
        Tracer.cpp
        Tracer.hpp
    )

    ccola_add_binary(Click.wav click)

    ccola_dependencies(
        application
        hal_st.default_init
        hal_st.stm32fxxx
        hal_st.synchronous_stm32fxxx
        services.tracer
        local_hal.stm32fxxx
    )

    ccola_external_libraries(
        m
    )

ccola_end_component()

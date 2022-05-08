macro(ccola_make_absolute_from_source absolute_path_var path)

    get_filename_component(absolute "${path}" ABSOLUTE)
    if("${absolute}" STREQUAL "${path}")
        set(${absolute_path_var} "${path}")
    else()
        get_filename_component(${absolute_path_var} "${CMAKE_CURRENT_SOURCE_DIR}/${path}" ABSOLUTE)
    endif()

endmacro()


macro(ccola_add_binary target source symbol)

    # This macro adds a source file as binary data to an application.
    # Two symbols will be defined: ${symbol}_start and ${symbol}_end,
    # which point to the data.

    ccola_make_absolute_from_source(absolute_source "${source}")
    get_filename_component(path "${absolute_source}" DIRECTORY)
    get_filename_component(name "${absolute_source}" NAME)
    string(REPLACE "." "_" clean_name "${name}")

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.o"
        COMMAND ${CCOLA_OBJ_COPY_TOOL} -I binary -O elf32-littlearm -B arm --redefine-sym _binary_${clean_name}_start=${symbol}_start --redefine-sym _binary_${clean_name}_end=${symbol}_end "${name}" "${CMAKE_CURRENT_BINARY_DIR}/${name}.o"
        DEPENDS "${absolute_source}"
        WORKING_DIRECTORY "${path}"
    )

    target_sources(${target} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/${name}.o"
    )

endmacro()

if (${TARGET_BUILD_CORTEX} AND ${TARGET_MCU} STREQUAL STM32F746)

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
        metronome.application
        hal_st.stm32fxxx
        hal_st.synchronous_stm32fxxx
        services.tracer
        local_hal.stm32fxxx
        m
    )

    target_link_options(metronome.main_stm32f746 PRIVATE
        "LINKER:-Map=$<TARGET_FILE:metronome.main_stm32f746>.map"
    )

    add_custom_command(
        TARGET metronome.main_stm32f746
        POST_BUILD
        COMMAND ${CCOLA_OBJ_COPY_TOOL} -O binary "$<TARGET_FILE:metronome.main_stm32f746>" "$<TARGET_FILE:metronome.main_stm32f746>.bin"
        COMMAND ${CCOLA_OBJ_COPY_TOOL} -O ihex "$<TARGET_FILE:metronome.main_stm32f746>" "$<TARGET_FILE:metronome.main_stm32f746>.hex"
    )

endif()

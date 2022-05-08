if (TARGET_BUILD_CORTEX)

    add_library(local_hal.stm32fxxx STATIC)

    target_include_directories(local_hal.stm32fxxx PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>/../.."
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    target_sources(local_hal.stm32fxxx PRIVATE
        SaiStm.cpp
        SaiStm.hpp
    )

    target_link_libraries(local_hal.stm32fxxx PUBLIC
        hal_st.stm32fxxx
        preview.stm32fxxx
    )

endif()

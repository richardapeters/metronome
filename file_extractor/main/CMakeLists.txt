if (EMIL_HOST_BUILD)

    add_executable(file_extractor.main)

    target_include_directories(file_extractor.main PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    target_sources(file_extractor.main PRIVATE
        Main.cpp
    )

    target_link_libraries(file_extractor.main PUBLIC
        args
    )

    include(file_extractor.cmake)

    extract_from_file(Main.cpp extracted.txt)

    add_custom_target(file_extractor.test ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generated/extracted.txt")

    #set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/generated/extracted.txt" PROPERTIES GENERATED true)
    #target_sources(file_extractor.test "${CMAKE_CURRENT_BINARY_DIR}/generated/extracted.txt")

endif()

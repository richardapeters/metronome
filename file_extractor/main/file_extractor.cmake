function(extract_from_file input output)

    cmake_path(SET generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    cmake_path(ABSOLUTE_PATH input NORMALIZE OUTPUT_VARIABLE input_abs)
    cmake_path(SET generated_file "${generated_dir}/${output}")

    add_custom_command(
        OUTPUT "${generated_file}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${generated_dir}"
        COMMAND $<TARGET_FILE:file_extractor.main> extract --input="${input_abs}" --output="${generated_file}"
        DEPENDS "${input_abs}"
        DEPENDS file_extractor.main
    )

endfunction()

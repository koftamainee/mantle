function(add_error_codes TARGET)
    find_package(Python3 REQUIRED)
    set(ERROR_CODES_FILE "${CMAKE_BINARY_DIR}/devinfo/error_codes.json")

    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/devinfo"
        COMMAND ${Python3_EXECUTABLE}
            "${CMAKE_SOURCE_DIR}/scripts/gen_error_codes.py"
            "${ERROR_CODES_FILE}"
        COMMENT "Generating error codes → ${ERROR_CODES_FILE}"
        VERBATIM
    )
endfunction()

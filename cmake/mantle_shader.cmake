function(mantle_shader TARGET)
    cmake_parse_arguments(SHD "" "NAME;ENTRIES" "SOURCES" ${ARGN})

    if(NOT SHD_NAME)
        message(FATAL_ERROR "mantle_shader: NAME is required")
    endif()
    if(NOT SHD_SOURCES)
        message(FATAL_ERROR "mantle_shader: SOURCES is required")
    endif()
    if(NOT SHD_ENTRIES)
        set(SHD_ENTRIES "vert_main;frag_main")
    endif()

    if(NOT DEFINED SLANGC_EXECUTABLE)
        if(DEFINED ENV{VULKAN_SDK})
            find_program(SLANGC_EXECUTABLE
                NAMES slangc slangc.exe
                HINTS
                    $ENV{VULKAN_SDK}/Bin
                    $ENV{VULKAN_SDK}/bin
            )
        else()
            message(FATAL_ERROR "VULKAN_SDK is not set and SLANGC_EXECUTABLE not provided")
        endif()
    endif()
    if(NOT EXISTS "${SLANGC_EXECUTABLE}")
        message(FATAL_ERROR "slangc not found at: ${SLANGC_EXECUTABLE}")
    endif()

    set(SHD_SOURCES_ABS)
    foreach(src IN LISTS SHD_SOURCES)
        get_filename_component(src_abs "${src}" ABSOLUTE)
        list(APPEND SHD_SOURCES_ABS "${src_abs}")
    endforeach()

    set(OUTPUT_SPV "${CMAKE_BINARY_DIR}/target/assets/shaders/${SHD_NAME}.spv")

    set(SHD_ENTRY_FLAGS)
    foreach(entry IN LISTS SHD_ENTRIES)
        list(APPEND SHD_ENTRY_FLAGS "-entry" "${entry}")
    endforeach()

    add_custom_command(
            OUTPUT ${OUTPUT_SPV}
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/target/assets/shaders"
            COMMAND ${SLANGC_EXECUTABLE} ${SHD_SOURCES_ABS}
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            ${SHD_ENTRY_FLAGS}
            -o ${OUTPUT_SPV}
            DEPENDS ${SHD_SOURCES_ABS}
            COMMENT "Compiling shader ${SHD_NAME}"
            VERBATIM
    )

    add_custom_target(${TARGET}_shader_${SHD_NAME} DEPENDS ${OUTPUT_SPV})
    add_dependencies(${TARGET} ${TARGET}_shader_${SHD_NAME})
endfunction()

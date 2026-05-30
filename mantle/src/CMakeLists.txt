add_subdirectory(core)
add_subdirectory(window)
add_subdirectory(renderer)
add_subdirectory(camera)
add_subdirectory(engine)
add_subdirectory(math)
add_subdirectory(world)
add_subdirectory(noise)

mantle_executable(NAME mantle MAIN main.cpp DEPS engine)

set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/mantle/assets/shaders")

mantle_shader(mantle
        NAME mesh_chunks
        ENTRIES main
        SOURCES "${SHADERS_DIR}/mesh_chunks.slang"
)

mantle_shader(mantle
        NAME write_indirect
        ENTRIES main
        SOURCES "${SHADERS_DIR}/write_indirect.slang"
)

mantle_shader(mantle
        NAME mesh_render
        SOURCES "${SHADERS_DIR}/mesh_render.slang"
)
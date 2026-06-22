mantle_module(NAME assets DEPS core renderer ecs mintload glm)

target_include_directories(assets PRIVATE
    "${CMAKE_SOURCE_DIR}/mantle/third_party/ktx/lib/include"
)
target_link_libraries(assets PRIVATE ktx)

find_package(Vulkan REQUIRED)
target_include_directories(assets PRIVATE ${Vulkan_INCLUDE_DIRS})